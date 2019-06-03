#ifndef SCHEDULER_H
#define SCHEDULER_H

// Main Round Robin Scheduler
void round_robin(struct grand_packet *grand_list);

// Removes Entry and Replaces with New Entry
void replace_packet(struct grand_packet *packet);

void its_time(struct grand_packet *packet, int list_i);

#endif
