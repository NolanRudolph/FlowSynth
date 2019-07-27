#include "next.h"

// Global File Pointer
FILE *fp;
unsigned char sHost[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char dHost[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

double first_time = -1.0;

void begin(char *fname, unsigned char *src, unsigned char *dst) {
    fp = fopen(fname, "r");
    
    int srcLength = 0;
    int dstLength = 0;
    int i, j;
    // Getting size of Ether sHost input and testing for validity
    for (i = 0; src[i] != '\0' || i == 6; ++i) {
        ++srcLength;
    }
    if (srcLength != 17) {
        printf("Invalid Ethernet Source\n");
        exit(EXIT_FAILURE);
    }
    // Getting size of Ether dHost input and testing for validity
    for (i = 0; dst[i] != '\0' || i == 6; ++i) {
        ++dstLength;
    }
    if (dstLength != 17) {
        printf("Invalid Ethernet Destination\n");
        exit(EXIT_FAILURE);
    }
	
	sscanf(src, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &sHost[0], &sHost[1], &sHost[2], &sHost[3],
                                                 &sHost[4], &sHost[5]);
    sscanf(dst, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &dHost[0], &dHost[1], &dHost[2], &dHost[3],
                                                 &dHost[4], &dHost[5]);
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
 * unsigned int length        // Length of all of a packet's contents
 * unsigned int f_bytes       // Final bytes to include in last packet (%)
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
    int cc = 0; // Comma-Count, method only viable for csv
    char ch;
    unsigned short int val;
    unsigned short int is_ipv6 = 0;

    // IP Attributes
    char IP_source[30];
    char IP_dest[30];
    char _proto[3];
    int proto;

    char TOS[3];
    char TCP_flags[2];

    // ICMP/IGMP Attributes
    char type[2];
    unsigned int code;
    unsigned short int is_dot = 0;

    // TCP/UDP Attributes
    char source[5];
    char dest[5];

    // Packet Sending Attributes
    char start[15];
    char end[15];
    char _packets[10];
    char _bytes[15];
    float d_time;
    uint32_t packets;
    uint32_t bytes;
    int length = 0;
    unsigned int payloadSize;
    unsigned int remainder;

//    // Hard Code Ethernet Hosts
//    unsigned char sHost[6] = 
//    {
//            0x00, 0x01, 0x02, 0x03, 0x04, 0x05
//    };
//    unsigned char dHost[6] =
//    {
//            0x00, 0x09, 0x08, 0x07, 0x06, 0x05
//    };    
    
    /* Reading from File */
    // Error Handling (No more file left to read)
    while ((ch = getc(fp)) != '\n' && ch != EOF) {
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
                        type[i] = ch;
                        type[++i] = '\0';
                    }
                    else {
                        code = val; // We found a dot, must be ICMP | IGMP
                    }
                    break;
                case 6: 
                    _proto[i] = ch; // Fill protocol buffer
                    _proto[++i] = '\0';
                    break;
                case 7:
                    TOS[i] = ch;
                    TOS[++i] = '\0';
                    break;
                case 8:
                    TCP_flags[i] = ch;
                    TCP_flags[++i] = '\0';
                    break;
                case 9:
                    _packets[i] = ch;
                    _packets[++i] = '\0';
                    break;
                case 10:
                    _bytes[i] = ch;
                    _bytes[++i] = '\0';
                    break;
            }
        }
    }

    // Reduce function calls with int variables
    proto = atoi(_proto);
    packets = atoi(_packets);
    bytes = atoi(_bytes);
    
    // d_time configuration
    d_time = atof(end) - atof(start);  // Get net time
    d_time /= packets;           // Divide that by number of packets
    
    #if 0 // Testing for Assuring Accuracy
    printf("\n*** PACKET DETAILS ***\n\n");
    printf("Protocol is %d\n", proto);
    printf("Start time is %s\n", start);
    printf("End time is %s\n", end);
    printf("IP_source is %s\n", IP_source);
    printf("IP_dest is %s\n", IP_dest);
    printf("Source is %d\n", atoi(source));
    printf("Dest is %d\n", atoi(dest));
    printf("Type may be %d\n", atoi(type));
    printf("Code may be %d\n", code);
    printf("TOS may be %d\n", TOS);
    printf("Packets is %d\n", packets);
    printf("Bytes is %d\n", bytes);
    printf("TCP_flags may be %d\n", atoi(TCP_flags));
    printf("D_time is %f\n\n", d_time);
    #endif
    
    /* Initialize Structures */
    struct ether_header *ether;
    struct ip *ip;
    struct icmphdr *icmp;
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
            icmp = (struct icmphdr *)(grand_ret.buff + sizeof(struct ether_header) + \
                                                     sizeof(struct ip));
            
            // Ether Configuration
            configure_ETHER(ether, dHost, sHost);
            
            // IP Configuration
            if (!is_ipv6)
                configure_IP(ip, '4', atoi(TOS), IP_source, IP_dest, 1); 
            else
                configure_IP(ip, '6', atoi(TOS), IP_source, IP_dest, 1);
            
            // ICMP Configuration
            configure_ICMP(icmp, atoi(type), code);
            
            // Check needs to be the last ICMP member evaluated
            // (change this method when involving payloads)
            icmp -> checksum = 0;
            
            // Adjust length of packet for correct buffer sending
            length = sizeof(struct ip) + sizeof(struct icmphdr);
            
            if (bytes/packets - length < 0) {
                printf("Received bad entry. Payload is negative bytes long.");
                exit(EXIT_FAILURE);
            }
            
            payloadSize = bytes/packets - length;
            
            remainder = bytes % packets;
            
            // Set IP length to correct size
            ip -> ip_len = htons(sizeof(struct ip) + sizeof(struct icmphdr) + \
                                                                payloadSize);
            
            // IMPORTANT: Prior to recording, the Ethernet header had been
            // stripped. Thus all calculations with length should not have
            // the Ethernet header involved.
            length += sizeof(struct ether_header);
            
            // Add on a fake payload that mimics the flow data
            addPayload((uint8_t *)icmp + sizeof(struct icmphdr), payloadSize);

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
                configure_IP(ip, '4', atoi(TOS), IP_source, IP_dest, 2);
            else
                configure_IP(ip, '6', atoi(TOS), IP_source, IP_dest, 2);
            
            // IGMP Configuration
            configure_IGMP(igmp, atoi(type), code, IP_dest);
            
            // Check needs to be the last IGMP member evaluated
            // (change this method when involving payloads)
            igmp -> igmp_cksum = 0;
            
            // Adjust length of packet for correct buffer sending
            length = sizeof(struct ip) + sizeof(struct igmp);
            
            if (bytes/packets - length < 0) {
                printf("Received bad entry. Payload is negative bytes long.");
                exit(EXIT_FAILURE);
            }
            
            payloadSize = bytes/packets - length;
            
            remainder = bytes % packets;
            
            // Set IP length to correct size
            ip -> ip_len = htons(sizeof(struct ip) + sizeof(struct igmp) + \
                                                                payloadSize);
            
            // IMPORTANT: Prior to recording, the Ethernet header had been
            // stripped. Thus all calculations with length should not have
            // the Ethernet header involved.
            length += sizeof(struct ether_header);
            
            // Add on a fake payload that mimics the flow data
            addPayload((uint8_t *)igmp + sizeof(struct igmp), payloadSize);

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
                configure_IP(ip, '4', atoi(TOS), IP_source, IP_dest, 6);
            else
                configure_IP(ip, '6', atoi(TOS), IP_source, IP_dest, 6);

            // TCP Configuration
            configure_TCP(tcp, atoi(source), atoi(dest));
            
            // Check needs to be the last TCP member evaluated
            // (change this method when involving payloads)
            tcp -> check = 0;
            
            // Adjust length of packet for correct buffer sending
            length = sizeof(struct ip) + sizeof(struct tcphdr);
            
            if (bytes/packets - length < 0) {
                printf("Received bad entry. Payload is negative bytes long.\n");
                exit(EXIT_FAILURE);
            }
            
            payloadSize = bytes/packets - length;
            
            remainder = bytes % packets;
            
            // Set IP length to correct size
            ip -> ip_len = htons(sizeof(struct ip) + sizeof(struct tcphdr) + \
                                                                payloadSize);
            
            // IMPORTANT: Prior to recording, the Ethernet header had been
            // stripped. Thus all calculations with length should not have
            // the Ethernet header involved.
            length += sizeof(struct ether_header);
            
            // Add on a fake payload that mimics the flow data
            addPayload((uint8_t *)tcp + sizeof(struct tcphdr), payloadSize);
            
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
                configure_IP(ip, '4', atoi(TOS), IP_source, IP_dest, 17);
            else
                configure_IP(ip, '6', atoi(TOS), IP_source, IP_dest, 17);           

            // UDP Configuration
            configure_UDP(udp, atoi(source), atoi(dest));
            
            // Check needs to be the last UDP member evaluated
            // (change this method when involving payloads)
            udp -> check = 0;
            
            // Adjust length of packet for correct buffer sending
            length = sizeof(struct ip) + sizeof(struct udphdr);
            
            // Check to make sure the entry's bytes per packet is viable
            if (bytes/packets - length < 0) {
                printf("Received bad entry. Payload is negative bytes long.");
                exit(EXIT_FAILURE);
            }
            
            payloadSize = bytes/packets - length;
            
            remainder = bytes % packets;
            
            // Set IP length to correct size
            ip -> ip_len = htons(sizeof(struct ip) + sizeof(struct udphdr) + \
                                                                payloadSize); 
            
            // Set UDP length to correct size
            udp -> len = htons(8 + payloadSize);
            
            // IMPORTANT: Prior to recording, the Ethernet header had been
            // stripped. Thus all calculations with length should not have
            // the Ethernet header involved.
            length += sizeof(struct ether_header);
            
            // Add on a fake payload that mimics the flow data
            addPayload((uint8_t *)udp + sizeof(struct udphdr), payloadSize);

            break;
    }
    
    // Check needs to be the last IP member evaluated
    ip -> ip_sum = 0;
    ip -> ip_sum = calcCheck((uint8_t *)ip, 20);
    
    // Adjust length to accommodate for payload size
    length += payloadSize;
    
    // Setting remainder of grand_packet's attributes
    grand_ret.packets_left = packets;
    grand_ret.d_time = d_time;
    grand_ret.cur_time = atof(start) - first_time;
    grand_ret.length = length;
    grand_ret.f_bytes = remainder;

    // Lastly, copy over the memory of the grand_packet to position in grand_list
    memcpy(placeHere, &grand_ret, sizeof(struct grand_packet));
    
    // Return 1 to notify that there are more entries in the dataset
    if (ch == EOF)
		return -1;
	else
		return 1;
}
