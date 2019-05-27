#ifndef NEXT_H
#define NEXT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conf.h"

void begin(char *fname);

void get_next(struct ether_header *ether, struct ip *ip, struct icmp *icmp,\
			  struct igmp *igmp, struct tcphdr *tcp, struct udphdr *udp);

#endif
