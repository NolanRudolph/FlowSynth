#ifndef CONF_H
#define CONF_H
#include <stdio.h>
#include <stdlib.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/igmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <sys/socket.h>

void configure_ETHER(struct ether_header *ether, char *dst, char *src);
void configure_IP(struct ip *ip, int version, int tos, char *src, \
                  char *dst, int proto);
void configure_ICMP(struct icmp *icmp, int type, int code);
void configure_IGMP(struct igmp *igmp, int type, int code);
void configure_TCP(struct tcphdr *tcp, int source, int dest); // Changed uint8_t to int
void configure_UDP(struct udphdr *udp, int source, int dest);

#endif 
