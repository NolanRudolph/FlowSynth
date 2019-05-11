#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>

void create_socket(int* sock, int on);
void configure_IP(struct ip *ip);
void configure_ICMP(struct icmp *icmp);
void send_packets(int sock, struct ip *ip, struct icmp *icmp, struct sockaddr_in sendto_addr);

int main(int argc, char* argv[]) {
	/* Socket Creation and Allowing Socket Options */	
	int sock;
	int on;

	create_socket(&sock, on);

	// This File Descriptor will be used for sendto()
	printf("Created Socket with File Descriptor %d.\n", sock);

	// Specify Address * CHANGE ME IF NOT TESTING *
	char address[] = "127.0.0.1";

	// Specifying Address for Socket
	struct sockaddr_in sendto_addr;
	sendto_addr.sin_family = AF_INET;
	sendto_addr.sin_port = htons(9001);
	sendto_addr.sin_addr.s_addr = inet_addr(address);


	/* Constructing IP Struct */
	struct ip *ip = NULL;
	ip = (struct ip *)malloc(sizeof(struct ip));		
	
	// Memory Error Handling
	if (ip == NULL) {
		fprintf(stderr, "Malloc error for IP struct.\n");
	}

	configure_IP(ip);

	
	/* Constructing ICMP Struct */
	struct icmp *icmp = (struct icmp *)(ip + 1);
	
	configure_ICMP(icmp);


	/* Transmitting Packets */
	send_packets(sock, ip, icmp, sendto_addr);


	return 0;

}

void create_socket(int* sock, int on) {
	if ((*sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		perror("socket() error");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(*sock, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0) {
		perror("setsockopt() failure");
		exit(EXIT_FAILURE);
	}
}

void configure_IP(struct ip *ip) {

	ip -> ip_v = 4;
	ip -> ip_hl = 5;
	ip -> ip_tos = 0;
	ip -> ip_p = IPPROTO_ICMP;
	ip -> ip_ttl = 255;

	// Temporary Testing on Localhost
	ip -> ip_src.s_addr = inet_addr("127.0.0.1");
	ip -> ip_dst.s_addr = inet_addr("127.0.0.1");

}

void configure_ICMP(struct icmp *icmp) {

	icmp -> icmp_type = ICMP_ECHO;
	icmp -> icmp_code = 0;
	icmp -> icmp_id = 123;

}

void send_packets(int sock, struct ip *ip, struct icmp *icmp, struct sockaddr_in sendto_addr) {	

	int bytes_sent;

	if ((bytes_sent = sendto(sock, ip, sizeof(ip) + sizeof(icmp), 0, \
		(struct sockaddr *)&sendto_addr, sizeof(sendto_addr))) < 0) {
		perror("sendto() error");
		exit(EXIT_FAILURE);
	}

}

