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

void create_socket(int* sock, int on, uint8_t proto);
void configure_IP(struct ip *ip, uint8_t proto);
void configure_ICMP(struct icmp *icmp);
void configure_IGMP(struct igmp *igmp);
void configure_TCP(struct tcphdr *tcp);
void configure_UDP(struct udphdr *udp);
void send_icmp_packets(int sock, struct ip *ip, struct icmp *icmp, \
						struct sockaddr_in sendto_addr);
void send_igmp_packets(int sock, struct ip *ip, struct igmp *igmp, \
						struct sockaddr_in sendto_addr);
void send_tcp_packets(int sock, struct ip *ip, struct tcphdr *tcp, \
						struct sockaddr_in sento_addr);
void send_udp_packets(int sock, struct ip *ip, struct udphdr *udp, \
						struct sockaddr_in sendto_addr);
void send_packet(int proto, int sock, char *packet, struct ip *ip, \
						struct sockaddr_in sendto_addr);

/*
 * PACKET ATTRIBUTES *
1   Start Timestamp in Epoch Format
2   End Timestamp in Epoch Format
3   Source IP Address
4   Destination IP Address
5   Source Port Number
6   Destination Port Number
7   IP Header Protocol Number
8   TOS Value in IP Header
9   TCP Flags
10  Number of Packets
11  Number of Bytes
12  Router Ingress Port
13  Router Egress Port
14  Source ASN
15  Destination ASN
*/

int main(int argc, char* argv[]) {
	/* Socket Creation and Allowing Socket Options */	
	int mainSock;
	int on = 1;

	//create_socket(&mainSock, on, IPPROTO_RAW);
	mainSock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	

	// Specify Address * CHANGE ME IF NOT TESTING *
	char address[] = "127.0.0.1";
	

	// Specifying Address for Socket
	struct sockaddr_in sendto_addr;
	sendto_addr.sin_family = AF_INET;
	sendto_addr.sin_port = htons(9001);
	sendto_addr.sin_addr.s_addr = inet_addr(address);

	// Specifying IP for Later Configuration
	struct ip *ip = NULL;
    ip = (struct ip *)malloc(sizeof(struct ip));

    if (ip == NULL) {
        fprintf(stderr, "Malloc error for IP struct.\n");
        exit(EXIT_FAILURE);
    }

	
	/* READING NETFLOW FROM CSV */

	if (argc != 2) {
		printf("Usage: $ ./a.out [FILE]\n");
		exit(EXIT_FAILURE);
	}

	int cc = 0; // Comma-Count, method only viable for csv
	FILE * fp;
	char ch;
	char _proto[3];
	int i = 0;
	int proto;
	char packet[65535];  // Initialized to max packet size

	// Generic IP used for sendto's

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

	free(ip);	
	return 0;

}

void create_socket(int* sock, int on, uint8_t proto) {
	if ((*sock = socket(AF_INET, SOCK_RAW, proto)) < 0) {
		perror("socket() error");
		exit(EXIT_FAILURE);
	}
	// IPPROTO_RAW, IP_HDRINCL
	if (setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		perror("setsockopt() failure");
		exit(EXIT_FAILURE);
	}
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

void configure_ICMP(struct icmp *icmp) {

	icmp -> icmp_type = ICMP_ECHO;
	icmp -> icmp_code = 0;
	// Add icmp_cksum

}

void configure_IGMP(struct igmp *igmp) {

    igmp -> igmp_type = 0;
    igmp -> igmp_code = 0;
    // Add igmp_cksum, maybe in_addr.igmp_group

}

void configure_TCP(struct tcphdr *tcp) {

	tcp -> source = 123;
	tcp -> dest = 123;
	// Skipping sequence number and ACK number

}


void configure_UDP(struct udphdr *udp) {
	
	udp -> source = 123;
	udp -> dest = 123;
	// Add uh_ulen & uh_sum

}

void send_icmp_packets(int sock, struct ip *ip, struct icmp *icmp, struct sockaddr_in sendto_addr) {	

	int bytes_sent;

	if ((bytes_sent = sendto(sock, ip, sizeof(ip) + sizeof(icmp), 0, \
		(struct sockaddr *)&sendto_addr, sizeof(sendto_addr))) < 0) {
		perror("ICMP sendto() error");
		exit(EXIT_FAILURE);
	}

}


void send_igmp_packets(int sock, struct ip *ip, struct igmp *igmp, struct sockaddr_in sendto_addr) {	
	int bytes_sent;

	if ((bytes_sent = sendto(sock, ip, sizeof(ip) + sizeof(igmp), 0, \
		(struct sockaddr *)&sendto_addr, sizeof(sendto_addr))) < 0) {
		perror("IGMP sendto() error");
		exit(EXIT_FAILURE);
	}

}

void send_tcp_packets(int sock, struct ip *ip, struct tcphdr *tcp, struct sockaddr_in sendto_addr) {	
	int bytes_sent;

	if ((bytes_sent = sendto(sock, ip, sizeof(ip) + sizeof(tcp), 0, \
		(struct sockaddr *)&sendto_addr, sizeof(sendto_addr))) < 0) {
		perror("TCP sendto() error");
		exit(EXIT_FAILURE);
	}

}

void send_udp_packets(int sock, struct ip *ip, struct udphdr *udp, struct sockaddr_in sendto_addr) {	
	int bytes_sent;

	if ((bytes_sent = sendto(sock, ip, sizeof(ip) + sizeof(udp), 0, \
		(struct sockaddr *)&sendto_addr, sizeof(sendto_addr))) < 0) {
		perror("UDP sendto() error");
		exit(EXIT_FAILURE);
	}

}

void send_packet(int proto, int sock, char *packet, struct ip *ip, struct sockaddr_in sendto_addr) {
	int on;

	// ICMP
	if (proto == 1) {
		struct icmp *icmp = (struct icmp *)(ip + 1);
		configure_ICMP(icmp);
	/*	
		if (setsockopt(sock, IPPROTO_ICMP, IP_HDRINCL, &on, sizeof(on)) < 0) {
        	perror("setsockopt() failure");
        	exit(EXIT_FAILURE);
    	}
	*/
		send_icmp_packets(sock, ip, icmp, sendto_addr);
	} 
	// IGMP
	else if (proto == 2) {
		struct igmp *igmp = (struct igmp *)(ip + 1);
		configure_IGMP(igmp);

		send_igmp_packets(sock, ip, igmp, sendto_addr);
	}
	// TCP 
	else if (proto == 6) {
		struct tcphdr *tcp = (struct tcphdr *)(ip + 1);
		configure_TCP(tcp);

		send_tcp_packets(sock, ip, tcp, sendto_addr);
	} 
	// UDP
	else if (proto == 17) {
		struct udphdr *udp = (struct udphdr *)(ip + 1);
		configure_UDP(udp);

		send_udp_packets(sock, ip, udp, sendto_addr);
	}
}


