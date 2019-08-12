#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>
#define MAX_POOL 100
#define MAX_THREADS 100
#define MAX_FLOWS 10000
#define MTU 1500

// Struct Type for Representing Flows
typedef struct grand_packet grand_packet_t;
struct grand_packet
{
        char buff[MTU];
        unsigned int packets_left;
        float d_time;
        double cur_time;
        unsigned int length;
        unsigned int f_bytes;
        struct grand_packet *last;
        struct grand_packet *next;
};

// Struct Type for Threads
typedef struct thread thread_t;
struct thread {
        pthread_t active_tid;
        unsigned char status:1;
};

// Struct Type for Thread Pool w/ Restrictive Variable Sizes
typedef struct thread_pool thread_pool_t;
struct thread_pool {
        thread_t thread_list[MAX_THREADS];
        pthread_attr_t pool_attr;
        uint8_t n_idle;
        uint8_t n_active;
};

// Struct Type for Flow Pool
typedef struct flow_list flow_list_t;
struct flow_list {
        // 10000 (MAX_FLOWS) * 100 (flow_pool_t) => 1000000000 Concurrent Flows
        grand_packet_t flows[MAX_FLOWS];
        int nflows;
	int id;
};

#endif
