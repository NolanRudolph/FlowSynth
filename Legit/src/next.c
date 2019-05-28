#include "next.h"

FILE *fp;

void begin(char *fname) {
    fp = fopen(fname, "r");
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
unsigned int val;
unsigned int is_ipv6 = 0;

// IP Attributes
char IP_source[20];
char IP_dest[20];
char proto[3];     // 1 <= proto <= 255 : three ints

unsigned int TOS = 0;       // 0 <= TOS <= 7 : 1 int (our dataset has 3 digit values?)
char TCP_flags[2]; // 0 <= TCP_flags <= 31 : 2 ints

// ICMP/IGMP Attributes
unsigned int type = 0;     // 0 <= type <= 7 : 1 int
char code[2];     // 8 <= code <= 15 : 2 ints
unsigned int is_dot = 0;   // Used to tell when to switch from type to code

// TCP/UDP Attributes
char source[5];   // 0 <= src port <= 65535 : 5 ints
char dest[5];     // 0 <= dest port <= 65535 : 5 ints

// Packet Sending Attributes
char start[15];
char end[15];
char packets[10];
char bytes[15];
float d_time;
unsigned short int length = 0;

/* struct grand_packet attributes:
 * char *buff                 // Used for storing all the data, contingently
 * unsigned int packets_left  // Maintains how many packets are left to send
 * float d_time               // Delta time between sending packets
 * unsigned short int length  // Length of all of a packet's contents
*/

struct grand_packet get_next(struct ether_header *ether, struct ip *ip, struct icmp *icmp,\
              struct igmp *igmp, struct tcphdr *tcp, struct udphdr *udp) {
    while ((ch = getc(fp)) != '\n') {
        if (ch == ',') {
            i = 0;
            ++cc;               // Found comma, moving onto next section
            if (cc == 15) {
                goto lastComma; // Yeah yeah bad habit, but I see no other way
            }                   // (goes to case 15 in switch(cc))
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
                    proto[i] = ch; // Fill protocol buffer
                    proto[++i] = '\0';
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
                case 15:
                    lastComma:
                    #if 1 // Testing for Assuring Accuracy
                    printf("Protocol is %d\n", atoi(proto));
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
                    printf("TCP_flags may be %d\n\n\n", atoi(TCP_flags));
                    #endif

                    // Ether will have same configuration amongst all proto
                    configure_ETHER(ether, dest, source);
                    d_time = atof(end) - atof(start);
                    length += sizeof(struct ether_header) + sizeof(struct ip);
                    switch (atoi(proto)) {
                        case 1:
                                if (!is_ipv6)
                                    configure_IP(ip, 4, TOS, IP_source, IP_dest, 1); 
                                else
                                    configure_IP(ip, 6, TOS, IP_source, IP_dest, 1);
                                configure_ICMP(icmp, type, atoi(code));
                                length += sizeof(struct icmp);
                                break;

                        case 2:
                                if (!is_ipv6)
                                    configure_IP(ip, 4, TOS, IP_source, IP_dest, 2);
                                else
                                    configure_IP(ip, 6, TOS, IP_source, IP_dest, 2);
                                configure_IGMP(igmp, type, atoi(code));
                                length += sizeof(struct icmp);
                                break;

                        case 6:
                                if (!is_ipv6)
                                    configure_IP(ip, 4, TOS, IP_source, IP_dest, 6);
                                else
                                    configure_IP(ip, 6, TOS, IP_source, IP_dest, 6);
                                configure_TCP(tcp, atoi(source), atoi(dest));
                                length += sizeof(struct icmp);
                                break;

                        case 17:
                                if (!is_ipv6)
                                    configure_IP(ip, 4, TOS, IP_source, IP_dest, 17);
                                else
                                    configure_IP(ip, 6, TOS, IP_source, IP_dest, 17);
                                configure_UDP(udp, atoi(source), atoi(dest));
                                length += sizeof(struct icmp);
                                break;
                    }
                    // Reset Section
                    cc = 0;
                    is_dot = 0;
                    is_ipv6 = 0;
                    length = 0;
                    memset(ether -> ether_dhost, 0, sizeof(ether -> ether_dhost));
                    memset(ether -> ether_shost, 0, sizeof(ether -> ether_shost));
                    break;
                    // Case 15 ends here
            }
        }
    }
}
