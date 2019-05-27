#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include <netdb.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>

#include <arpa/inet.h>
#include <sys/time.h>

#include <errno.h>
#include "conf.h"

struct grand_packet
{
    char *buff;
    int packets_left;
    int d_time;
};

void send_packet(int socket, struct grand_packet packet, struct sockaddr_in addr);


int main(int argc, char* argv[]) {

    // Usage Error Handling
    if (argc != 2) {
            printf("Usage: $ ./a.out [FILE]\n");
            exit(EXIT_FAILURE);
    }	

    /* Socket Creation */
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
            perror("socket() error");
            exit(EXIT_FAILURE);
    }

    /* Initialize Delivery Address */

    // Specify Address
    char address[] = "127.0.0.1";

    // Specifying Address for Socket
    struct sockaddr_in sendto_addr;
    sendto_addr.sin_family = AF_INET;
    sendto_addr.sin_port = htons(9001);
    sendto_addr.sin_addr.s_addr = inet_addr(address);


    /* Initialize All Protocol Structures */
    struct ether_header *ether = (struct ether_header *)\
                                  malloc(sizeof(struct ether_header));
    ether -> ether_type = ETHERTYPE_IP; // Ethernet type remains constant
    
    struct ip *ip = (struct ip *)malloc(sizeof(struct ip));
    
    struct icmp *icmp = (struct icmp *)malloc(sizeof(struct icmp));

    struct igmp *igmp = (struct igmp *)malloc(sizeof(struct igmp));

    struct tcphdr *tcp = (struct tcphdr *)malloc(sizeof(struct tcphdr));

    struct udphdr *udp = (struct udphdr *)malloc(sizeof(struct udphdr));


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
    FILE * fp;
    char ch;
    int val;
    int is_ipv6 = 0;

    // IP Attributes
    char IP_source[20];
    char IP_dest[20];
    char proto[3];     // 1 <= proto <= 255 : three ints

    int TOS = 0;       // 0 <= TOS <= 7 : 1 int (our dataset has 3 digit values?)
    char TCP_flags[2]; // 0 <= TCP_flags <= 31 : 2 ints

    // ICMP/IGMP Attributes
    int type = 0;     // 0 <= type <= 7 : 1 int
    char code[2];     // 8 <= code <= 15 : 2 ints
    int is_dot = 0;   // Used to tell when to switch from type to code

    // TCP/UDP Attributes
    char source[5];   // 0 <= src port <= 65535 : 5 ints
    char dest[5];     // 0 <= dest port <= 65535 : 5 ints

    // Packet Sending Attributes
    char start[15];
    char end[15];
    char packets[10];
    char bytes[15];
    float d_time;

    fp = fopen(argv[1], "r");
    while ((ch = getc(fp)) != EOF) {
        if (ch == ',') {
                i = 0;
                ++cc;  // Found comma, moving onto next section
        } 
        else {   // Else, lets grab attributes for our packet
            val = ch - '0'; // ch - '0' returns non-ASCII integer value of 0 - 9
            switch(cc) {
                case 0:
                    start[i] = ch;
                    start[++i] = '\0';
                    break;
                case 1:
                    end[i] = ch;
                    end[++i] = '\0';
                    break;
                case 2:
                    IP_source[i] = ch;
                    IP_source[++i] = '\0'; // Using '\0' so I don't have to
                    break;               // constantly reset memory
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
                    switch (atoi(proto)) {
                        case 1:
                                if (!is_ipv6)
                                    configure_IP(ip, 4, TOS, IP_source, IP_dest, 1); 
                                else
                                    configure_IP(ip, 6, TOS, IP_source, IP_dest, 1);
                                configure_ICMP(icmp, type, atoi(code));
                                break;

                        case 2:
                                if (!is_ipv6)
                                    configure_IP(ip, 4, TOS, IP_source, IP_dest, 2);
                                else
                                    configure_IP(ip, 6, TOS, IP_source, IP_dest, 2);
                                configure_IGMP(igmp, type, atoi(code));
                                break;

                        case 6:
                                if (!is_ipv6)
                                    configure_IP(ip, 4, TOS, IP_source, IP_dest, 6);
                                else
                                    configure_IP(ip, 6, TOS, IP_source, IP_dest, 6);
                                configure_TCP(tcp, atoi(source), atoi(dest));
                                printf("tcp source is %d\n", tcp -> source);
                                break;

                        case 17:
                                if (!is_ipv6)
                                    configure_IP(ip, 4, TOS, IP_source, IP_dest, 17);
                                else
                                    configure_IP(ip, 6, TOS, IP_source, IP_dest, 17);
                                configure_UDP(udp, atoi(source), atoi(dest));
                                break;
                    }
                    // Reset Section
                    cc = 0;
                    is_dot = 0;
                    is_ipv6 = 0;
                    memset(ether -> ether_dhost, 0, sizeof(ether -> ether_dhost));
                    memset(ether -> ether_shost, 0, sizeof(ether -> ether_shost));
                    break;
            }
        }
    }
	
    free(ether);
    free(ip);
    free(icmp);
    free(igmp);
    free(tcp);
    free(udp);
    close(sockfd);

    return 0;

}

void send_packet(int socket, struct grand_packet packet, struct sockaddr_in addr){
    if (sendto(socket, &packet, sizeof(struct grand_packet), 0, \
            (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("TCP sendto() error");
            exit(EXIT_FAILURE);
    }
}