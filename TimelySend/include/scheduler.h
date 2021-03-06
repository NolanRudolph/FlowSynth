#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <time.h>

// For getifaddrs(3)
#include <sys/types.h>
#include <ifaddrs.h>

// For sockaddr_ll
#include <linux/if_packet.h>

#include "next.h"

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

// Initializer for Round Robin Scheduler
// Used for defining sockets, testing socket, etc.
void round_robin_init(char *interface);

// Main Round Robin Scheduler
void round_robin(void);

// Adding new flows to the round robin when applicable to time frame
int add_candidates(double time);

#endif
