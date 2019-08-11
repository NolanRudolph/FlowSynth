#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#define MAX_THREADS 100
#define ACTIVE 0x01
#define INACTIVE 0x00

/* Structures and New Types */

// Struct Type for Threads
typedef struct thread thread_t;
struct thread {
        pthread_t active_tid;
        unsigned char status:1;
};

// Thread Pool w/ Restrictive Variable Sizes
typedef struct thread_pool thread_pool_t;
struct thread_pool {
        thread_t thread_list[MAX_THREADS];
        pthread_attr_t pool_attr;
        uint8_t n_idle;
        uint8_t n_active;
};

// Declare Main Thread Pool Variable
extern thread_pool_t main_pool;

/* Functions */

// Initializes a Global Thread Pool
void thread_pool_init(void);

// Tells Thread in Thread Pool to Execute a Job
static int create_worker(thread_pool_t pool);
