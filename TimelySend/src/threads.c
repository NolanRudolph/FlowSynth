#include "threads.h"

thread_pool_t main_pool;

void thread_pool_init(void) {
        pthread_attr_init(&main_pool.pool_attr);
        pthread_attr_setdetachstate(&main_pool.pool_attr, PTHREAD_CREATE_DETACHED);
        main_pool.n_idle = MAX_THREADS;
        main_pool.n_active = 0;

	// Init all threads to INACTIVE
        for (int i = 0; i < MAX_THREADS; ++i) {
                main_pool.thread_list[i].status = INACTIVE;
        }
}

