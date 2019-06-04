#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <time.h>
#include <linux/if_packet.h>

#include "next.h"

void round_robin_init(void);

// Main Round Robin Scheduler
void round_robin(struct grand_packet *grand_list, int size);

void its_time(struct grand_packet *packet, int list_i);

#endif
