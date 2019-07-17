#ifndef CONF_H
#define CONF_H
#include <stdio.h>
#include <stdlib.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netinet/igmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void configure_ETHER(struct ether_header *ether, unsigned char *dst, unsigned char *src);
void configure_IP(struct ip *ip, unsigned char version, unsigned \
		short int tos, char *src, char *dst, unsigned char proto);
void configure_ICMP(struct icmphdr *icmp, unsigned short int type, \
					unsigned short int code);
void configure_IGMP(struct igmp *igmp, unsigned short int type, \
					unsigned short int code, char *addr);
void configure_TCP(struct tcphdr *tcp, unsigned short int source, \
				   unsigned short int dest);
void configure_UDP(struct udphdr *udp, unsigned short int source, \
				   unsigned short int dest);
void addPayload(uint8_t *ptr, int size);
uint16_t calcCheck(uint8_t *data, int len);
#endif 
