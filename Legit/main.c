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
 *  1   Start Timestamp in Epoch Format
 *  2   End Timestamp in Epoch Format
 *  3   Source IP Address
 *  4   Destination IP Address
 *  5   Source Port Number
 *  6   Destination Port Number
 *  7   IP Header Protocol Number
 *  8   TOS Value in IP Header
 *  9   TCP Flags
 *  10  Number of Packets
 *  11  Number of Bytes
 *  12  Router Ingress Port
 *  13  Router Egress Port
 *  14  Source ASN
 *  15  Destination ASN
 *  
*/

	int cc = 0; // Comma-Count, method only viable for csv
	FILE * fp;
	char ch;
	char _proto[3];
	int i = 0;
	int proto;
	char packet[65535];  // Initialized to max packet size

	// Generic IP used for sendto's
	/*
	fp = fopen(argv[1], "r");
	while ((ch = getc(fp)) != EOF) {
		if (ch == ',') {
			++cc;
		}
		else if (cc == 6) {
			_proto[i++] = ch;
		}
		if (cc == 15) {
			cc = 0;
			i = 0;
			proto = atoi(_proto);
			memset(_proto, ' ', sizeof(_proto)*sizeof(char));
			memset(packet, 0, 65535);  // Reset content of packet
			send_packet(proto, mainSock, packet, ip, sendto_addr);	
		}
	}
	*/
	configure_IP(tcp -> ip, IPPROTO_TCP);
	configure_TCP(tcp -> tcp, 123, 123);

	send_TCP(tcp_sock, *tcp, sendto_addr);	
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

