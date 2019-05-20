#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/igmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#include <arpa/inet.h>
#include <sys/time.h>
void configure_IP(struct ip *ip, uint8_t proto);
void configure_ICMP(struct icmp *icmp, uint8_t type, uint8_t code);
void configure_IGMP(struct igmp *igmp, uint8_t type, uint8_t code);
void configure_TCP(struct tcphdr *tcp, uint8_t source, uint8_t dest);
void configure_UDP(struct udphdr *udp, uint8_t source, uint8_t dest);
void configure_socket(int sockfd, uint8_t proto);
void send_icmp(void);

struct icmp_packet
{
	struct ip *ip;
	struct icmp *icmp;
};

struct igmp_packet
{
	struct ip *ip;
	struct igmp *igmp;
};

struct tcp_packet
{
	struct ip *ip;
	struct tcphdr *tcp;
};

struct udp_packet
{
	struct ip *ip;
	struct udphdr *udp;
};

void send_ICMP(int socket, struct icmp_packet packet, struct sockaddr_in sendto_addr);
void send_IGMP(int socket, struct igmp_packet packet, struct sockaddr_in sendto_addr);
void send_TCP(int socket, struct tcp_packet packet, struct sockaddr_in sendto_addr);
void send_UDP(int socket, struct udp_packet packet, struct sockaddr_in sendto_addr);


int main(int argc, char* argv[]) {

	// Usage Error Handling
    if (argc != 2) {
        printf("Usage: $ ./a.out [FILE]\n");
        exit(EXIT_FAILURE);
    }

		/* Socket Creation */	
	
	// ICMP
	int icmp_sock;
    if ((icmp_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

	// IGMP
	int igmp_sock;
    if ((igmp_sock = socket(AF_INET, SOCK_RAW, IPPROTO_IGMP)) < 0) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

	// TCP
	int tcp_sock;
    if ((tcp_sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

	// UDP
	int udp_sock;
    if ((udp_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    // Specify Address
    char address[] = "127.0.0.1";
    
	// Specifying Address for Socket
	struct sockaddr_in sendto_addr;
	sendto_addr.sin_family = AF_INET;
	sendto_addr.sin_port = htons(9001);
	sendto_addr.sin_addr.s_addr = inet_addr(address);
	

		/* Initialize All Protocol Structures */

	// ICMP
	struct icmp_packet *icmp = NULL;
	icmp = (struct icmp_packet *)malloc(sizeof(struct icmp_packet));
	if (icmp == NULL) {
		fprintf(stderr, "Malloc error for ICMP struct.\n");
		exit(EXIT_FAILURE);
	}

    icmp -> ip = (struct ip *)malloc(sizeof(struct ip));
    icmp -> icmp = (struct icmp *)malloc(sizeof(struct icmp));


	// IGMP
	struct igmp_packet *igmp = NULL;
	igmp = (struct igmp_packet *)malloc(sizeof(struct igmp_packet));
    if (igmp == NULL) {
        fprintf(stderr, "Malloc error for IGMP struct.\n");
        exit(EXIT_FAILURE);
    }

	igmp -> ip = (struct ip *)malloc(sizeof(struct ip));
    igmp -> igmp = (struct igmp *)malloc(sizeof(struct igmp));
	
	// TCP
	struct tcp_packet *tcp = NULL;
	tcp = (struct tcp_packet *)malloc(sizeof(struct tcp_packet));
    if (tcp == NULL) {
        fprintf(stderr, "Malloc error for TCP struct.\n");
        exit(EXIT_FAILURE);
    }

	tcp -> ip = (struct ip *)malloc(sizeof(struct ip));
    tcp -> tcp = (struct tcphdr *)malloc(sizeof(struct tcphdr));

	// UDP
	struct udp_packet *udp = NULL;
	udp = (struct udp_packet *)malloc(sizeof(struct udp_packet));
    if (udp == NULL) {
        fprintf(stderr, "Malloc error for UDP struct.\n");
        exit(EXIT_FAILURE);
    }

	udp -> ip = (struct ip *)malloc(sizeof(struct ip));
    udp -> udp = (struct udphdr *)malloc(sizeof(struct udphdr));


	/* READING NETFLOW FROM CSV */

/*
 *  * PACKET ATTRIBUTES *
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
	char proto[3]; 	  // 1 <= proto <= 255 : three ints
	int val;

	// IP Attributes
	char IP_source[20];
	char IP_dest[20];
	memset(IP_source, ' ', sizeof(IP_source));
	memset(IP_dest, ' ', sizeof(IP_dest));

	int TOS = 0;       // 0 <= TOS <= 7 : 1 int (our dataset has 3 digit values?)
	char TCP_flags[2]; // 0 <= TCP_flags <= 31 : 2 ints
		

	// ICMP/IGMP Attributes
	int type = 0;     // 0 <= type <= 7 : 1 int
	char code[2]; 	  // 8 <= code <= 15 : 2 ints
	memset(code, ' ', sizeof(code));
	int is_dot = 0;   // Used to tell when to switch from type to code

	// TCP/UDP Attributes
	char source[5];   // 0 <= src port <= 65535 : 5 ints
	memset(source, ' ', sizeof(source));
	char dest[5];  	  // 0 <= dest port <= 65535 : 5 ints
	memset(dest, ' ', sizeof(dest));

	// Packet Sending Attributes
	char packets[10];
	memset(source, ' ', sizeof(packets));
	char bytes[15];
	memset(source, ' ', sizeof(bytes));

	// Generic IP used for sendto's
	fp = fopen(argv[1], "r");
	while ((ch = getc(fp)) != EOF) {
		if (ch == ',') {
			i = 0;
			++cc;  // Found comma, moving onto next section
		} else {   // Else, lets grab attributes for our packet
			val = ch - '0'; // ch - '0' returns non-ASCII integer value of 0 - 9
			switch(cc) {
				case 2:
					IP_source[i++] = ch;
					break;
				case 3:
					IP_dest[i++] = ch;
					break;
				case 4:
					source[i++] = ch;
					break;
				case 5: // This can be either the dest port or code.type for ICMP | IGMP 
					if (ch == '.') {
						is_dot = 1;
					}
					else if (!is_dot) {
						dest[i++] = ch; // Dest will always record, in case TCP | UDP
						code[i] = ch;
					}
					else {
						type = val; // We found a dot, must be ICMP | IGMP
					}
					break;
				case 6: 
					proto[i++] = ch; // Fill protocol buffer
					break;
				case 7:
					TOS = val;
					break;
				case 8:
					TCP_flags[i++] = ch;
					break;
				case 9:
					packets[i++] = ch;
					break;
				case 10:
					bytes[i++] = ch;
					break;
				case 15:
					// Reset Section
					cc = 0;
					is_dot = 0;
					
					#if 0 // Testing for Assuring Accuracy
		            printf("Protocol is %d\n", atoi(proto));
	            	printf("IP_source is %s\n", IP_source);
    	        	printf("IP_dest is %s\n", IP_dest);
        	    	printf("Source may be %d\n", atoi(source));
            		printf("Dest may be %d\n", atoi(dest));
            		printf("Type may be %d\n", type);
            		printf("Code may be %d\n", atoi(code));
            		printf("TOS may be %d\n", TOS);
					printf("Packets is %d\n", atoi(packets));
					printf("Bytes is %d\n", atoi(bytes));
            		printf("TCP_flags may be %d\n\n\n", atoi(TCP_flags));
	
					memset(IP_source, ' ', sizeof(IP_source));
					memset(IP_dest, ' ', sizeof(IP_dest));
					memset(source, ' ', sizeof(source));
					memset(code, ' ', sizeof(code));
					memset(dest, ' ', sizeof(dest));
					memset(proto, ' ', sizeof(proto));
					memset(TCP_flags, ' ', sizeof(TCP_flags));
					#endif
					// send_packet(proto, mainSock, packet, ip, sendto_addr);
					break;
			}
		}
	}
	
	// configure_IP(tcp -> ip, IPPROTO_TCP);
	// configure_TCP(tcp -> tcp, 123, 123);

	// send_TCP(tcp_sock, *tcp, sendto_addr);	
	
	return 0;

}

void configure_IP(struct ip *ip, uint8_t proto) {

	ip -> ip_v = 4;
	ip -> ip_hl = 10;
	ip -> ip_tos = 0;
	ip -> ip_p = proto;
	ip -> ip_ttl = 255;

	// Temporary Testing on Localhost
	ip -> ip_src.s_addr = inet_addr("127.0.0.1");
	ip -> ip_dst.s_addr = inet_addr("127.0.0.1");

}

void configure_ICMP(struct icmp *icmp, uint8_t type, uint8_t code) {

	icmp -> icmp_type = type;
	icmp -> icmp_code = code;
	// Add icmp_cksum

}

void configure_IGMP(struct igmp *igmp, uint8_t type, uint8_t code) {

    igmp -> igmp_type = type;
    igmp -> igmp_code = code;
    // Add igmp_cksum, maybe in_addr.igmp_group

}

void configure_TCP(struct tcphdr *tcp, uint8_t source, uint8_t dest) {

	tcp -> source = source;
	tcp -> dest = dest;
	// Skipping sequence number and ACK number

}


void configure_UDP(struct udphdr *udp, uint8_t source, uint8_t dest) {
	
	udp -> source = 123;
	udp -> dest = 123;
	// Add uh_ulen & uh_sum

}

void send_ICMP(int socket, struct icmp_packet packet, struct sockaddr_in sendto_addr) {
    int bytes_sent;

    if ((bytes_sent = sendto(socket, &packet, sizeof(struct icmp_packet), 0, \
        (struct sockaddr *)&sendto_addr, sizeof(sendto_addr))) < 0) {
        perror("ICMP sendto() error");
        exit(EXIT_FAILURE);
    }
}

void send_IGMP(int socket, struct igmp_packet packet, struct sockaddr_in sendto_addr){
    int bytes_sent;

    if ((bytes_sent = sendto(socket, &packet, sizeof(struct igmp_packet), 0, \
        (struct sockaddr *)&sendto_addr, sizeof(sendto_addr))) < 0) {
        perror("IGMP sendto() error");
        exit(EXIT_FAILURE);
    }
}

void send_TCP(int socket, struct tcp_packet packet, struct sockaddr_in sendto_addr){
	int bytes_sent;

    if ((bytes_sent = sendto(socket, &packet, sizeof(struct tcp_packet), 0, \
        (struct sockaddr *)&sendto_addr, sizeof(sendto_addr))) < 0) {
        perror("TCP sendto() error");
        exit(EXIT_FAILURE);
    }
}

void send_UDP(int socket, struct udp_packet packet, struct sockaddr_in sendto_addr){
    int bytes_sent;

    if ((bytes_sent = sendto(socket, &packet, sizeof(struct udp_packet), 0, \
        (struct sockaddr *)&sendto_addr, sizeof(sendto_addr))) < 0) {
        perror("TCP sendto() error");
        exit(EXIT_FAILURE);
    }
}

