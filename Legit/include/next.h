#ifndef NEXT_H
#define NEXT_H
#define MAXBUF 1024
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conf.h"

struct grand_packet
{
	char buff[MAXBUF];
	unsigned int packets_left;
	float d_time;
	double cur_time;
	unsigned short length;
};

void begin(char *fname);

struct grand_packet * get_next(struct ether_header *ether, struct ip *ip, struct icmp *icmp,\
			  struct igmp *igmp, struct tcphdr *tcp, struct udphdr *udp);

#endif
