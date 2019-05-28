#ifndef NEXT_H
#define NEXT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conf.h"

struct grand_packet
{
	char *buff;
	unsigned int packets_left;
	float d_time;
	unsigned short int length;
};

void begin(char *fname);

struct grand_packet get_next(struct ether_header *ether, struct ip *ip, struct icmp *icmp,\
			  struct igmp *igmp, struct tcphdr *tcp, struct udphdr *udp);

#endif
