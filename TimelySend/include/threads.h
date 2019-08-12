#ifndef THREADS_H
#define THREADS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include "next.h"
#include "structs.h"
#define MAX_THREADS 100
#define MAX_FLOWS 10000
#define MAX_POOL 100
#define ACTIVE 0x01
#define INACTIVE 0x00

// Declare Main Thread Pool Variable
extern thread_pool_t main_pool;

/* Functions */

// Initializes a Global Thread Pool
void thread_pool_init(void);

// Thread Utilized Function for Filling main_fpool
void * __thread_fill_fpool(void *);

// Thread Utilized Function for Adding Flows
int __thread_add_candidates(double time, flow_list_t *curList);

#endif
