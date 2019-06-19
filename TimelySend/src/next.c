#include "next.h"

// Global File Pointer
FILE *fp;

// Given the method setup, we need a way of delivering the last packet.
int send_last = 0;

/* Initialize All Grand Packets */
struct grand_packet grand_icmp;
struct grand_packet grand_igmp;
struct grand_packet grand_tcp;
struct grand_packet grand_udp;

/* First Time Explained on Line 119 */
double first_time;

/* Next Packet Explained on Line 122 */
struct grand_packet next_packet;
double next_time = 0.0;
struct grand_packet cur_packet;

/* Initialize All Protocol Structures */
struct ether_header *ether;
struct ip *ip;
struct icmp *icmp;
struct igmp *igmp;
struct tcphdr *tcp;
struct udphdr *udp;

void begin(char *fname) {
    fp = fopen(fname, "r");
    
    /* Allocate Memory for Structures */
    ether = (struct ether_header *)malloc(sizeof(struct ether_header));
    ip = (struct ip *)malloc(sizeof(struct ip));
    icmp = (struct icmp *)malloc(sizeof(struct icmp));
    igmp = (struct igmp *)malloc(sizeof(struct igmp));
    tcp = (struct tcphdr *)malloc(sizeof(struct tcphdr));
    udp = (struct udphdr *)malloc(sizeof(struct udphdr));
    
    // Initialize the dataset analyzing process
    get_first();
}

void stop() {
    free(ether);
    free(ip);
    free(icmp);
    free(igmp);
    free(tcp);
    free(udp);
    close(fp);
}

/* READING NETFLOW FROM CSV */

/*
 *  * PACKET ATTRIBUTES *
 *  -= cc stands for comma count, see below =-
 *
 *  cc = 0   Start Timestamp in Epoch Format
 *  cc = 1   End Timestamp in Epoch Format
 *  cc = 2   Source IP Address
 *  cc = 3   Destination IP Address
 *  cc = 4   Source Port Number
 *  cc = 5   Destination Port Number (or type.code if ICMP/IGMP)
 *  cc = 6   IP Header Protocol Number
 *  cc = 7   TOS Value in IP Header
 *  cc = 8   TCP Flags
 *  cc = 9   Number of Packets
 *  cc = 10  Number of Bytes
 *  cc = 11  Router Ingress Port
 *  cc = 12  Router Egress Port
 *  cc = 13  Source ASN
 *  cc = 14  Destination ASN
 *  
*/

int i = 0;
int cc = 0; 	  // Comma-Count, method only viable for csv
char ch;
unsigned short int val;
unsigned short int is_ipv6 = 0;

// IP Attributes
char IP_source[30];
char IP_dest[30];
char _proto[3];     // 1 <= proto <= 255 : three ints
unsigned short int proto;

unsigned short int TOS = 0;       // 0 <= TOS <= 7 : 1 int
char TCP_flags[2]; // 0 <= TCP_flags <= 31 : 2 ints

// ICMP/IGMP Attributes
unsigned short int type = 0;     // 0 <= type <= 7 : 1 int
char code[2];     // 8 <= code <= 15 : 2 ints
unsigned short int is_dot = 0;   // Used to tell when to switch from type to code

// TCP/UDP Attributes
char source[5];   // 0 <= src port <= 65535 : 5 ints
char dest[5];     // 0 <= dest port <= 65535 : 5 ints

// Packet Sending Attributes
char start[15];
char end[15];
char packets[10];
char bytes[15];
float d_time;
unsigned int length = 0;

/* struct grand_packet attributes:
 * char *buff                 // Used for storing all the packet data adjacently
 * unsigned int packets_left  // Maintains how many packets are left to send
 * float d_time               // Delta time between sending packets
 * float cur_time             // Keeps track of current time, added to by d_time
 * unsigned short int length  // Length of all of a packet's contents
 * struct grand_packet last   // Previous packet in grand_list for round-robin
 * struct grand_packet next   // Next packet in grand_list used for round-robin
*/

/* This function serves to get the first packet, which is very important b/c:
 *      1. The start time stamp will be the soonest in the whole dataset. So,
 *         what we can do is subtract this from all other entries to use real
 *         time to send each packet.
 *      2. This will initialize our "next_packet" variable to the first packet
 *         in our dataset. Why do we want to have our next_packet defined prior
 *         to the next "get_next()" call? Good question. The packet scheduler
 *         will only get the next packet if its start time is within currently
 *         assessed time. (e.g. If our scheduler is at 300 seconds, we don't want
 *         a 301 start time entry in our dataset. That's unneeded overhead). So
 *         we can check if our global variable, "next_time", is applicable to
 *         our next round-robin iteration. If so, we append our next_packet to 
 *         our list using scheduler.c's its_time(). If not, disregard and keep 
 *         scheduling the list that is already known to fall within the time frame.
 *      3. Provides error checking on the first packet. Since the same method of
 *         obtaining packet attributes occurs in get_next(), the #if 0 can be
 *         changed to #if 1 to test the accuracy of packet recording and
 *         construction for all packets using just the first packet.
*/ 
void get_first() {
    printf("WE ARE CALLING GET FIRST\n");
    // Reset Section
    cc = 0;
    is_dot = 0;
    is_ipv6 = 0;
    length = 0;
    memset(ether -> ether_dhost, ' ', sizeof(ether -> ether_dhost));
    memset(ether -> ether_shost, ' ', sizeof(ether -> ether_shost));
    
    /* Reading from File */
    // Error Handling (No content in file to read from)
    if ((ch = getc(fp)) == EOF) {
        printf("You gave a blank file.\n");
        exit(EXIT_FAILURE);
    }
    do {
        if (ch == ',') {
            i = 0;
            ++cc;               // Found comma, moving onto next section
        } 
        else {                  // Else, lets grab attributes for our packet
            val = ch - '0';     // ch - '0' returns non-ASCII integer value of 0 - 9
            switch(cc) {
                case 0:
                    start[i] = ch;
                    start[++i] = '\0';  // Using '\0' so I don't have to reset mem
                    break;
                case 1:
                    end[i] = ch;
                    end[++i] = '\0';
                    break;
                case 2:
                    IP_source[i] = ch;
                    IP_source[++i] = '\0'; 
                    break;
                case 3:
                    if (ch == ':') {
                            is_ipv6 = 1;
                    }
                    IP_dest[i] = ch;
                    IP_dest[++i] = '\0';
                    break;
                case 4:
                    source[i] = ch;
                    source[++i] = '\0';
                    break;
                case 5: // This can be either the dest port or code.type for ICMP | IGMP 
                    if (ch == '.') {
                            is_dot = 1;
                    }
                    else if (!is_dot) {
                            dest[i] = ch; // Dest will always record, in case TCP | UDP
                            dest[i + 1] = '\0';
                            code[i] = ch;
                            code[++i] = '\0';
                    }
                    else {
                            type = val; // We found a dot, must be ICMP | IGMP
                    }
                    break;
                case 6: 
                    _proto[i] = ch; // Fill protocol buffer
                    _proto[++i] = '\0';
                    break;
                case 7:
                    TOS = val;
                    break;
                case 8:
                    TCP_flags[i] = ch;
                    TCP_flags[++i] = '\0';
                    break;
                case 9:
                    packets[i] = ch;
                    packets[++i] = '\0';
                    break;
                case 10:
                    bytes[i] = ch;
                    bytes[++i] = '\0';
                    break;
            }
        }
    } while ((ch = getc(fp)) != '\n');
    
    // Error checking to see if correct csv format is passed as the file
    if (cc != 15) {
        printf("Incorrect CSV file type.\n");
        exit(EXIT_FAILURE);
    }
    
    // Get int prototype
    proto = atoi(_proto);
    
    // Set first_time as explained on Line 119
    first_time = atof(start);
    
    #if 0 // Testing for Assuring Accuracy
    printf("\n*** PACKET DETAILS ***\n\n");
    printf("Protocol is %d\n", proto);
    printf("Start time is %s\n", start);
    printf("End time is %s\n", end);
    printf("IP_source is %s\n", IP_source);
    printf("IP_dest is %s\n", IP_dest);
    printf("Source is %d\n", atoi(source));
    printf("Dest is %d\n", atoi(dest));
    printf("Type may be %d\n", type);
    printf("Code may be %d\n", atoi(code));
    printf("TOS may be %d\n", TOS);
    printf("Packets is %d\n", atoi(packets));
    printf("Bytes is %d\n", atoi(bytes));
    printf("TCP_flags may be %d\n\n", atoi(TCP_flags));
    #endif

    // Ether will have same configuration amongst all proto
    configure_ETHER(ether, dest, source);
    
    // d_time configuration
    d_time = atof(end) - atof(start);  // Get net time
    d_time /= atoi(packets);           // Divide that by number of packets
    
    // All packets will have same Ether and IP length
    length += sizeof(struct ether_header) + sizeof(struct ip);
    
    // Configuration of Grand Packets
    switch (proto) {
        case 1:
                // Setting pointers to correct spots in grand_icmp's buffer
                ether = (struct ether_header *)grand_icmp.buff;
                ip = (struct ip *)(grand_icmp.buff + sizeof(struct ether_header));
                icmp = (struct icmp *)(grand_icmp.buff + sizeof(struct ether_header) + \
                                                         sizeof(struct ip));
                if (!is_ipv6)
                    configure_IP(ip, 4, TOS, IP_source, IP_dest, 1); 
                else
                    configure_IP(ip, 6, TOS, IP_source, IP_dest, 1);
                ip -> ip_len = htons(length - sizeof(struct ether_header));
                
                configure_ICMP(icmp, type, (short)atoi(code));
                length += sizeof(struct icmp);
                
                // Setting remainder of grand_packet's attributes
                grand_icmp.length = length;
                grand_icmp.d_time = d_time;
                grand_icmp.cur_time = 0.0;
                grand_icmp.packets_left = atoi(packets);
                break;

        case 2:
                // Setting pointers to correct spots in grand_igmp's buffer
                ether = (struct ether_header *)grand_igmp.buff;
                ip = (struct ip *)(grand_igmp.buff + sizeof(struct ether_header));
                igmp = (struct igmp *)(grand_igmp.buff + sizeof(struct ether_header) + \
                                                         sizeof(struct ip));
                
                // Configuring said pointers to hold correct attributes
                if (!is_ipv6)
                    configure_IP(ip, 4, TOS, IP_source, IP_dest, 2);
                else
                    configure_IP(ip, 6, TOS, IP_source, IP_dest, 2);
                ip -> ip_len = htons(length - sizeof(struct ether_header));
                
                configure_IGMP(igmp, type, (short)atoi(code));

                // Adjust length of packet
                length += sizeof(struct igmp);
                
                // Setting remainder of grand_packet's attributes
                grand_igmp.length = length;
                grand_igmp.d_time = d_time;
                grand_igmp.cur_time = 0.0;
                grand_igmp.packets_left = atoi(packets);
                break;

        case 6:
                // Setting pointers to correct spots in grand_tcp's buffer
                ether = (struct ether_header *)grand_tcp.buff;
                ip = (struct ip *)(grand_tcp.buff + sizeof(struct ether_header));
                tcp = (struct tcphdr *)(grand_tcp.buff + sizeof(struct ether_header) + \
                                                         sizeof(struct ip));
                
                // Configuring said pointers to hold correct attributes
                if (!is_ipv6)
                    configure_IP(ip, 4, TOS, IP_source, IP_dest, 6);
                else
                    configure_IP(ip, 6, TOS, IP_source, IP_dest, 6);
                ip -> ip_len = htons(length - sizeof(struct ether_header));
                
                configure_TCP(tcp, (short)atoi(source), (short)atoi(dest));
                
                // Adjust length of packet
                length += sizeof(struct tcphdr);
                
                // Setting remainder of grand_packet's attributes
                grand_tcp.length = length;
                grand_tcp.d_time = d_time;
                grand_tcp.cur_time = 0.0;
                grand_tcp.packets_left = atoi(packets);
                break;

        case 17:
                // Setting pointers to correct spots in grand_udp's buffer
                ether = (struct ether_header *)grand_udp.buff;
                ip = (struct ip *)(grand_udp.buff + sizeof(struct ether_header));
                udp = (struct udphdr *)(grand_udp.buff + sizeof(struct ether_header) + \
                                                         sizeof(struct ip));

                // Configuring said pointers to hold correct attributes
                if (!is_ipv6)
                    configure_IP(ip, 4, TOS, IP_source, IP_dest, 17);
                else
                    configure_IP(ip, 6, TOS, IP_source, IP_dest, 17);
                ip -> ip_len = htons(length - sizeof(struct ether_header));
                
                configure_UDP(udp, (short)atoi(source), (short)atoi(dest));
                udp -> len = htons(length - sizeof(struct ether_header) - \
                                               sizeof(struct ip));
                
                // Adjust length of packet
                length += sizeof(struct udphdr);
                
                // Setting remainder of grand_packet's attributes
                grand_udp.length = length;
                grand_udp.d_time = d_time;
                grand_udp.cur_time = 0.0;
                grand_udp.packets_left = atoi(packets);
                break;
    }
    
#if 0 // Linked with line 69 in main.c
    printf("\n\n*** PACKET POINTER TRANSFER DETAILS ***\n\n");
    if (proto == 1)
        printf("Got ICMP packet from %#010x\n", &grand_icmp);
    else if (proto == 2)
        printf("Got IGMP packet from %#010x\n", &grand_igmp);
    else if (proto == 6)
        printf("Got TCP packet from %#010x\n", &grand_tcp);
    else if (proto == 17)
        printf("Got UDP packet from %#010x\n", &grand_udp);
#endif

    // Initializing next_packet
    if (proto == 1)
        next_packet = grand_icmp;
    else if (proto == 2)
        next_packet = grand_igmp;
    else if (proto == 6)
        next_packet = grand_tcp;
    else if (proto == 17)
        next_packet = grand_udp;
    else
        next_packet = grand_tcp;
    
    printf("So next_packet has %d packets left.\n", next_packet.packets_left);
}


// This function is almost identical to get_first(), except that its intentions
// are to continuously pump out packets using the dataset.
struct grand_packet * get_next() {
    // Reset Section
    cc = 0;
    is_dot = 0;
    is_ipv6 = 0;
    length = 0;
    memset(ether -> ether_dhost, ' ', sizeof(ether -> ether_dhost));
    memset(ether -> ether_shost, ' ', sizeof(ether -> ether_shost));
    
    /* Reading from File */
    // Error Handling (No more file left to read)
    if ((ch = getc(fp)) == EOF) {
        if (send_last == 0) {
            send_last = 1;
            // Don't forget to set the pre-assessed current time of the packet
            next_packet.cur_time = next_time;
            next_time = INFINITY;
            printf("We're returning the last packet with %d packets_left\n", \
                    next_packet.packets_left);
            return &next_packet;
        }
        else 
            return NULL;
    }
    do {
        if (ch == ',') {
            i = 0;
            ++cc;               // Found comma, moving onto next section
        } 
        else {                  // Else, lets grab attributes for our packet
            val = ch - '0';     // ch - '0' returns non-ASCII integer value of 0 - 9
            switch(cc) {
                case 0:
                    start[i] = ch;
                    start[++i] = '\0';  // Using '\0' so I don't have to reset mem
                    break;
                case 1:
                    end[i] = ch;
                    end[++i] = '\0';
                    break;
                case 2:
                    IP_source[i] = ch;
                    IP_source[++i] = '\0'; 
                    break;
                case 3:
                    if (ch == ':') {
                            is_ipv6 = 1;
                    }
                    IP_dest[i] = ch;
                    IP_dest[++i] = '\0';
                    break;
                case 4:
                    source[i] = ch;
                    source[++i] = '\0';
                    break;
                case 5: // This can be either the dest port or code.type for ICMP | IGMP 
                    if (ch == '.') {
                            is_dot = 1;
                    }
                    else if (!is_dot) {
                            dest[i] = ch; // Dest will always record, in case TCP | UDP
                            dest[i + 1] = '\0';
                            code[i] = ch;
                            code[++i] = '\0';
                    }
                    else {
                            type = val; // We found a dot, must be ICMP | IGMP
                    }
                    break;
                case 6: 
                    _proto[i] = ch; // Fill protocol buffer
                    _proto[++i] = '\0';
                    break;
                case 7:
                    TOS = val;
                    break;
                case 8:
                    TCP_flags[i] = ch;
                    TCP_flags[++i] = '\0';
                    break;
                case 9:
                    packets[i] = ch;
                    packets[++i] = '\0';
                    break;
                case 10:
                    bytes[i] = ch;
                    bytes[++i] = '\0';
                    break;
            }
        }
    } while ((ch = getc(fp)) != '\n');
    // Get int prototype
    proto = atoi(_proto);

    // Ether will have same configuration amongst all proto
    configure_ETHER(ether, dest, source);
    
    // d_time configuration
    d_time = atof(end) - atof(start);  // Get net time
    d_time /= atoi(packets);           // Divide that by number of packets
    
    // All packets will have same Ether and IP length
    length += sizeof(struct ether_header) + sizeof(struct ip);
    
    // Configuration of Grand Packets
    switch (proto) {
        case 1:
                // Setting pointers to correct spots in grand_icmp's buffer
                ether = (struct ether_header *)grand_tcp.buff;
                ip = (struct ip *)(grand_icmp.buff + sizeof(struct ether_header));
                icmp = (struct icmp *)(grand_icmp.buff + sizeof(struct ether_header) + \
                                                         sizeof(struct ip));
                if (!is_ipv6)
                    configure_IP(ip, 4, TOS, IP_source, IP_dest, 1); 
                else
                    configure_IP(ip, 6, TOS, IP_source, IP_dest, 1);
                configure_ICMP(icmp, type, atoi(code));
                length += sizeof(struct icmp);
                
                // Setting remainder of grand_packet's attributes
                grand_icmp.length = length;
                grand_icmp.d_time = d_time;
                grand_icmp.cur_time = atof(start);
                grand_icmp.packets_left = atoi(packets);
                break;

        case 2:
                // Setting pointers to correct spots in grand_igmp's buffer
                ether = (struct ether_header *)grand_tcp.buff;
                ip = (struct ip *)(grand_igmp.buff + sizeof(struct ether_header));
                igmp = (struct igmp *)(grand_igmp.buff + sizeof(struct ether_header) + \
                                                         sizeof(struct ip));
                
                // Configuring said pointers to hold correct attributes
                if (!is_ipv6)
                    configure_IP(ip, 4, TOS, IP_source, IP_dest, 2);
                else
                    configure_IP(ip, 6, TOS, IP_source, IP_dest, 2);
                configure_IGMP(igmp, type, atoi(code));

                // Adjust length of packet
                length += sizeof(struct igmp);
                
                // Setting remainder of grand_packet's attributes
                grand_igmp.length = length;
                grand_igmp.d_time = d_time;
                grand_igmp.cur_time = atof(start);
                grand_igmp.packets_left = atoi(packets);
                break;

        case 6:
                // Setting pointers to correct spots in grand_tcp's buffer
                ether = (struct ether_header *)grand_tcp.buff;
                ip = (struct ip *)(grand_tcp.buff + sizeof(struct ether_header));
                tcp = (struct tcphdr *)(grand_tcp.buff + sizeof(struct ether_header) + \
                                                         sizeof(struct ip));
                
                // Configuring said pointers to hold correct attributes
                if (!is_ipv6)
                    configure_IP(ip, 4, TOS, IP_source, IP_dest, 6);
                else
                    configure_IP(ip, 6, TOS, IP_source, IP_dest, 6);

                configure_TCP(tcp, atoi(source), atoi(dest));
                
                // Adjust length of packet
                length += sizeof(struct tcphdr);
                
                // Setting remainder of grand_packet's attributes
                grand_tcp.length = length;
                grand_tcp.d_time = d_time;
                grand_tcp.cur_time = atof(start);
                grand_tcp.packets_left = atoi(packets);
                break;

        case 17:
                // Setting pointers to correct spots in grand_udp's buffer
                ether = (struct ether_header *)grand_udp.buff;
                ip = (struct ip *)(grand_udp.buff + sizeof(struct ether_header));
                udp = (struct udphdr *)(grand_udp.buff + sizeof(struct ether_header) + \
                                                         sizeof(struct ip));

                // Configuring said pointers to hold correct attributes
                if (!is_ipv6)
                    configure_IP(ip, 4, TOS, IP_source, IP_dest, 17);
                else
                    configure_IP(ip, 6, TOS, IP_source, IP_dest, 17);
                configure_UDP(udp, atoi(source), atoi(dest));
                
                // Adjust length of packet
                length += sizeof(struct udphdr);
                
                // Setting remainder of grand_packet's attributes
                grand_udp.length = length;
                grand_udp.d_time = d_time;
                grand_udp.cur_time = atof(start);
                grand_udp.packets_left = atoi(packets);
                break;
    }
    
    // Temporarily store the next_packet at global variable cur_packet
    cur_packet = next_packet;
    // next_time was set to correct time last iteration
    cur_packet.cur_time = next_time;
    
    // Set the next_packet
    if (proto == 1)
        next_packet = grand_icmp;
    else if (proto == 2)
        next_packet = grand_igmp;
    else if (proto == 6)
        next_packet = grand_tcp;
    else if (proto == 17)
        next_packet = grand_udp;
    else
        next_packet = grand_tcp;
    
    // Set the next_time
    next_time = atof(start) - first_time;
    printf("Start: %lf // First: %lf // Result (Next): %lf\n", atof(start), first_time, next_time);
    
    // Return the temporary variable
    printf("We're returning packet with %d packets_left\n", cur_packet.packets_left);
    return &cur_packet;
}

// How the Round-robin scheduler knows when to append new entries
double get_next_time() {
    return next_time;
}