#include "next.h"

// Global File Pointer
FILE *fp;

double first_time = -1.0;

void begin(char *fname) {
    fp = fopen(fname, "r");
}

/*
 *  * FLOW ATTRIBUTES FROM CSV *
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

/* STRUCT GRAND_PACKET ATTRIBUTES:
 * char *buff                 // Used for storing all the packet data adjacently
 * unsigned int packets_left  // Maintains how many packets are left to send
 * float d_time               // Delta time between sending packets
 * float cur_time             // Keeps track of current time, added to by d_time
 * unsigned short int length  // Length of all of a packet's contents
 * struct grand_packet last   // Previous packet in grand_list for round-robin
 * struct grand_packet next   // Next packet in grand_list used for round-robin
*/


// This function is almost identical to get_first(), except that its intentions
// are to continuously pump out packets using the dataset.
int get_next(struct grand_packet *placeHere, time_t cur_time) {
    /* Initializing all variables for storing grand_packet attributes */
    int i = 0;
    
    // Back_track is used for re-reading entries. If a file is read and found
    // to be not compatible with some time window, the back_track is then used
    // to adjust the current file pointer to the beginning of the flow.
    int back_track = 0;
    int cc = 0; 	  // Comma-Count, method only viable for csv
    char ch;
    unsigned short int val;
    unsigned short int is_ipv6 = 0;

    // IP Attributes
    char IP_source[30];
    char IP_dest[30];
    char _proto[3];     // 1 <= proto <= 255 : three ints
    int proto;

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

    // Hard Code Ethernet Hosts
    unsigned char sHost[6] = 
    {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05
    };
    unsigned char dHost[6] =
    {
            0x00, 0x09, 0x08, 0x07, 0x06, 0x05
    };    
    
    /* Reading from File */
    // Error Handling (No more file left to read)
    if ((ch = getc(fp)) == EOF) {
        return -1;
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
                    ++back_track;
                    break;
                case 1:
                    if (first_time == -1.0) {
                        first_time = atof(start);
                    }
                    if (atof(start) - first_time > cur_time) {
                        // Flow should not be read yet, back track file pointer
                        fseek(fp, -back_track - 2, SEEK_CUR);
                        return 0;
                    }
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

    // Get int version of proto
    proto = atoi(_proto);
    
    // d_time configuration
    d_time = atof(end) - atof(start);  // Get net time
    d_time /= atoi(packets);           // Divide that by number of packets
    
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
    printf("TCP_flags may be %d\n", atoi(TCP_flags));
    printf("D_time is %f\n\n", d_time);
    #endif
    
    /* Initialize Structures */
    struct ether_header *ether;
    struct ip *ip;
    struct icmp *icmp;
    struct igmp *igmp;
    struct tcphdr *tcp;
    struct udphdr *udp;
    struct grand_packet grand_ret;
    
    // Configuration of Grand Packets
    switch (proto) {
        // ICMP Flow
        case 1: // ";" because of C's syntax of labeled-statements
            // Setting pointers to correct spots in grand_ret's buffer
            ether = (struct ether_header *)grand_ret.buff;
            ip = (struct ip *)(grand_ret.buff + sizeof(struct ether_header));
            icmp = (struct icmp *)(grand_ret.buff + sizeof(struct ether_header) + \
                                                     sizeof(struct ip));
            
            // Ether Configuration
            configure_ETHER(ether, dHost, sHost);
            
            // IP Configuration
            if (!is_ipv6)
                configure_IP(ip, 4, TOS, IP_source, IP_dest, 1); 
            else
                configure_IP(ip, 6, TOS, IP_source, IP_dest, 1);
            
            // Configure me later
            ip -> ip_len = htons(sizeof(struct ip) + sizeof(struct icmp));
            
            // ICMP Configuration
            configure_ICMP(icmp, type, atoi(code));
            
            // Adjust length of packet for correct buffer sending
            length = sizeof(struct ether_header) + sizeof(struct ip) + \
                    sizeof(struct icmp);

            break;

        // IGMP Flow
        case 2:
            // Setting pointers to correct spots in grand_ret's buffer
            ether = (struct ether_header *)grand_ret.buff;
            ip = (struct ip *)(grand_ret.buff + sizeof(struct ether_header));
            igmp = (struct igmp *)(grand_ret.buff + sizeof(struct ether_header) + \
                                                     sizeof(struct ip));

            // Ether Configuration
            configure_ETHER(ether, dHost, sHost);
            
            // IP Configuration
            if (!is_ipv6)
                configure_IP(ip, 4, TOS, IP_source, IP_dest, 2);
            else
                configure_IP(ip, 6, TOS, IP_source, IP_dest, 2);
            
            // Configure me later
            ip -> ip_len = htons(sizeof(struct ip) + sizeof(struct igmp));
            
            // IGMP Configuration
            configure_IGMP(igmp, type, atoi(code));

            // Adjust length of packet for correct buffer sending
            length = sizeof(struct ether_header) + sizeof(struct ip) + \
                    sizeof(struct igmp);

            break;

        // TCP Flow
        case 6:
            // Setting pointers to correct spots in grand_ret's buffer
            ether = (struct ether_header *)grand_ret.buff;
            ip = (struct ip *)(grand_ret.buff + sizeof(struct ether_header));
            tcp = (struct tcphdr *)(grand_ret.buff + sizeof(struct ether_header) + \
                                                     sizeof(struct ip));

            // Ether Configuration
            configure_ETHER(ether, dHost, sHost);
            
            // IP Configuration
            if (!is_ipv6)
                configure_IP(ip, 4, TOS, IP_source, IP_dest, 6);
            else
                configure_IP(ip, 6, TOS, IP_source, IP_dest, 6);
            
            // Configure me later
            ip -> ip_len = htons(sizeof(struct ip) + sizeof(struct tcphdr));

            // TCP Configuration
            configure_TCP(tcp, atoi(source), atoi(dest));

            // Adjust length of packet for correct buffer sending
            length = sizeof(struct ether_header) + sizeof(struct ip) + \
                    sizeof(struct tcphdr);
            
            break;

        // UDP Flow
        case 17:
            // Setting pointers to correct spots in grand_tcp's buffer
            ether = (struct ether_header *)grand_ret.buff;
            ip = (struct ip *)(grand_ret.buff + sizeof(struct ether_header));
            udp = (struct udphdr *)(grand_ret.buff + sizeof(struct ether_header) + \
                                                     sizeof(struct ip));

            // Ether Configuration
            configure_ETHER(ether, dHost, sHost);
            
            // IP Configuration
            if (!is_ipv6)
                configure_IP(ip, 4, TOS, IP_source, IP_dest, 17);
            else
                configure_IP(ip, 6, TOS, IP_source, IP_dest, 17);
            
            // Configure me later
            ip -> ip_len = htons(sizeof(struct ip) + sizeof(struct udphdr));            

            // UDP Configuration
            configure_UDP(udp, atoi(source), atoi(dest));

            // Adjust length of packet for correct buffer sending
            length = sizeof(struct ether_header) + sizeof(struct ip) + \
                    sizeof(struct udphdr);

            break;
    }
    
    // Setting remainder of grand_packet's attributes
    grand_ret.length = length;
    grand_ret.d_time = d_time;
    grand_ret.cur_time = atof(start) - first_time;
    grand_ret.packets_left = atoi(packets);
    memcpy(placeHere, &grand_ret, sizeof(struct grand_packet));
    
    return 1;
}
