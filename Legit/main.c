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
#include <netinet/udp.h>

#include <arpa/inet.h>
#include <sys/time.h>

void create_socket(int* sock, int on, uint8_t proto);
void configure_IP(struct ip *ip, uint8_t proto);
void configure_ICMP(struct icmp *icmp);
void configure_IGMP(struct igmp *igmp);
void configure_UDP(struct udphdr *udp);
void send_icmp_packets(int sock, struct ip *ip, struct icmp *icmp, struct sockaddr_in sendto_addr);
void send_igmp_packets(int sock, struct ip *ip, struct igmp *igmp, struct sockaddr_in sendto_addr);
void send_udp_packets(int sock, struct ip *ip, struct udphdr *udp, struct sockaddr_in sendto_addr);


int main(int argc, char* argv[]) {
	/* Socket Creation and Allowing Socket Options */	
	int sockICMP;
	int on;

	create_socket(&sockICMP, on, IPPROTO_ICMP);

	// This File Descriptor will be used for sendto()
	printf("Created ICMP Socket with File Descriptor %d.\n", sockICMP);

	// Specify Address * CHANGE ME IF NOT TESTING *
	char address[] = "127.0.0.1";

	// Specifying Address for Socket
	struct sockaddr_in sendto_addr;
	sendto_addr.sin_family = AF_INET;
	sendto_addr.sin_port = htons(9001);
	sendto_addr.sin_addr.s_addr = inet_addr(address);
	
		/******** ICMP ********/

	/* Constructing IP Struct */
	struct ip *ipICMP = NULL;
	ipICMP = (struct ip *)malloc(sizeof(struct ip));		
	
	// Memory Error Handling
	if (ipICMP == NULL) {
		fprintf(stderr, "Malloc error for ICMP'S IP struct.\n");
		exit(EXIT_FAILURE);
	}

	configure_IP(ipICMP, IPPROTO_ICMP);

	
	/* Constructing ICMP Struct */
	struct icmp *icmp = (struct icmp *)(ipICMP + 1);	
	configure_ICMP(icmp);

		/********* IGMP *********/

	int sockIGMP;
	create_socket(&sockIGMP, on, IPPROTO_IGMP);
	printf("Created IGMP Socket with File Descriptor %d.\n", sockIGMP);

	struct ip *ipIGMP = NULL;
	ipIGMP = (struct ip *)malloc(sizeof(struct ip));

	if (ipIGMP == NULL) {
		fprintf(stderr, "Malloc error for IGMP's IP struct.\n");
		exit(EXIT_FAILURE);
	}

	configure_IP(ipIGMP, IPPROTO_IGMP);

	struct igmp *igmp = (struct igmp *)(ipIGMP + 1);
	configure_IGMP(igmp); 



		/********* UDP *********/
	
	int sockUDP;
	create_socket(&sockUDP, on, IPPROTO_UDP);
	printf("Created UDP Socket with File Descriptor %d.\n", sockUDP);

	struct ip *ipUDP = NULL;
	ipUDP = (struct ip *)malloc(sizeof(struct ip));

	if (ipUDP == NULL) {
		fprintf(stderr, "Malloc error for UDP'S IP struct.\n");
		exit(EXIT_FAILURE);
	}

	configure_IP(ipUDP, IPPROTO_UDP);

	struct udphdr *udp = (struct udphdr *)(ipUDP + 1);
	configure_UDP(udp);

	/* Transmitting Packets */
	int i;
	send_icmp_packets(sockICMP, ipICMP, icmp, sendto_addr);
	send_igmp_packets(sockIGMP, ipIGMP, igmp, sendto_addr);
	send_udp_packets(sockUDP, ipUDP, udp, sendto_addr);
	return 0;

}

void create_socket(int* sock, int on, uint8_t proto) {
	if ((*sock = socket(AF_INET, SOCK_RAW, proto)) < 0) {
		perror("socket() error");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(*sock, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0) {
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

void configure_UDP(struct udphdr *udp) {
	
	udp -> source = 123;
	udp -> dest = 123;
	// Add uh_ulen & uh_sum

}

void configure_IGMP(struct igmp *igmp) {

	igmp -> igmp_type = 0;
	igmp -> igmp_code = 0;
	// Add igmp_cksum, maybe in_addr.igmp_group

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
void send_udp_packets(int sock, struct ip *ip, struct udphdr *udp, struct sockaddr_in sendto_addr) {	
	int bytes_sent;

	if ((bytes_sent = sendto(sock, ip, sizeof(ip) + sizeof(udp), 0, \
		(struct sockaddr *)&sendto_addr, sizeof(sendto_addr))) < 0) {
		perror("UDP sendto() error");
		exit(EXIT_FAILURE);
	}

}

