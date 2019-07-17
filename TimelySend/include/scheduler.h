#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <time.h>
#include <linux/if_packet.h>

// For getifaddrs(3)
#include <sys/types.h>
#include <ifaddrs.h>

// For sockaddr_ll
#include <linux/if_packet.h>

#include <unitsd.h>
#include "next.h"

void round_robin_init(char *interface);

// Main Round Robin Scheduler
void round_robin(void);

int add_candidates(double time);

void send_packet(struct grand_packet packet);

#endif
