#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#define MAX_THREADS 100
#define ACTIVE 0x01
#define INACTIVE 0x00

static void * worker_thread(void *arg);
void *sleepTwo(void *);

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

static int create_worker(thread_pool_t);

// Set of All Signals
static sigset_t full_set;


int main(int argc, char **argv) {
	// Creating Main Thread Pool and Setting Attributes
	thread_pool_t main_pool;
	pthread_attr_init(&main_pool.pool_attr);
	pthread_attr_setdetachstate(&main_pool.pool_attr, PTHREAD_CREATE_DETACHED);
	main_pool.n_idle = MAX_THREADS;
	main_pool.n_active = 0;

	// Init all threads to INACTIVE
	for (int i = 0; i < MAX_THREADS; ++i) {
		main_pool.thread_list[i].status = INACTIVE;
	}

	if (create_worker(main_pool) == 0) {
		printf("Worker Creation Success!\n");
	}

	sleep(3);
	return EXIT_SUCCESS;
}

static int create_worker(thread_pool_t pool) {
	int err;
	
	pthread_sigmask(SIG_SETMASK, &full_set, NULL);
	thread_t *next_loc = &pool.thread_list[pool.n_active];
	err = pthread_create(&pool.thread_list[pool.n_active].active_tid, &pool.pool_attr, sleepTwo, (void *)next_loc);
		
	return (err);
}

void *sleepTwo(void *arg) {
	thread_t *rec = (thread_t *)(arg);
	printf("(Received) Thread is Inactive/Active: %d\n", rec -> status);

	rec -> status = ACTIVE;	
	printf("(Turning On) Thread is Inactive/Active: %d\n", rec -> status);

	printf("Sleeping with the fishies...\n");
	sleep(2);
	rec -> status = INACTIVE;
	printf("(Turning Off) Thred is Inactive/Active: %d\n", rec -> status);
}
