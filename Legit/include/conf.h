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
void configure_IP(struct ip *ip, uint8_t version, uint8_t tos, char *src, \
                  char *dst, uint8_t proto);
void configure_ICMP(struct icmp *icmp, uint8_t type, uint8_t code);
void configure_IGMP(struct igmp *igmp, uint8_t type, uint8_t code);
void configure_TCP(struct tcphdr *tcp, int source, int dest); // Changed uint8_t to int
void configure_UDP(struct udphdr *udp, uint8_t source, uint8_t dest);
void configure_socket(int sockfd, uint8_t proto); 

#endif 
