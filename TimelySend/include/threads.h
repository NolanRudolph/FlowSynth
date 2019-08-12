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
#include "scheduler.h"
#define ACTIVE 0x01
#define INACTIVE 0x00

// Declare Main Thread Pool Variable
extern thread_pool_t main_pool;

/* Functions */

// Temp Testing Function
void testing(void);

// Initializes a Global Thread Pool
void thread_pool_init(void);

// Thread Utilized Function for Filling main_fpool
void * __thread_fill_fpool(void *);

// Thread Utilized Function for Adding Flows
int __thread_add_candidates(double time, flow_list_t *curList);

// Thread Utilized Function for Round Robin Thread Scheduling
void *  __thread_rr_check(void *);
#endif
