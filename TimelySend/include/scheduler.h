#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <time.h>

// For getifaddrs(3)
#include <sys/types.h>
#include <ifaddrs.h>

// For sockaddr_ll
#include <linux/if_packet.h>

#include "next.h"
#include "threads.h"

#define MAX_FLOWS 10000
#define MAX_POOL 100

struct grand_packet dummy_packet = 
{
	{
		// Ethernet
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x08, 0x00,
		// IP Header
		0x45, 0x01, 0x00, 0x27, 0x00, 0x00, 0x00, 0x00, 0xff, 0x06,
		0xbf, 0x99, 0x9d, 0xf0, 0x0b, 0x23, 0x29, 0x4b, 0x29, 0xc8,
		// TCP Header
		0x01, 0xbb, 0xda, 0x6b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x50, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00
	},
	0,
	0,
	0,
	54, // Length
};

typedef struct flow_list flow_list_t;
struct flow_list {
	// 10000 (MAX_FLOWS) * 100 (flow_pool_t) => 1000000000 Concurrent Flows
	grand_packet_t flows[MAX_FLOWS];
	int flow_n;
} flow_default = {{}, 0};

extern flow_list_t main_fpool[100];

// Initializer for Round Robin Scheduler
// Used for defining sockets, testing socket, etc.
void round_robin_init(char *interface);

// Thread Utilized Function for Filling main_fpool
void * __thread_fill_fpool(void *);

// Temp Testing
int __thread_add_candidates(double time, flow_list_t *curList);

// Main Round Robin Scheduler
void round_robin(void);

// Adding new flows to the round robin when applicable to time frame
int add_candidates(double time);

#endif
