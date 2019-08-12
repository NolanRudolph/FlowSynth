#include "threads.h"
#include "structs.h"

// External Variable Thread Pool Definition
thread_pool_t main_pool;

// External Variable Flow Pool Definition
flow_list_t main_fpool[MAX_POOL];

void testing() {
	printf("Beginning Testing...\n");
        pthread_t thread;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        int ret = pthread_create(&thread, &attr, __thread_fill_fpool, NULL);

        sleep(20);
        printf("Received Thread. Ending.\n");
}

void thread_pool_init(void) {
        pthread_attr_init(&main_pool.pool_attr);
        pthread_attr_setdetachstate(&main_pool.pool_attr, PTHREAD_CREATE_DETACHED);
        main_pool.n_idle = MAX_THREADS;
        main_pool.n_active = 0;

	// Init all threads to INACTIVE
        int i;
	for (i = 0; i < MAX_THREADS; ++i) {
                main_pool.thread_list[i].status = INACTIVE;
        }

    	for (i = 0; i < MAX_POOL; ++i) {
        	main_fpool[i].nflows = 0;
    	}

}

void * __thread_fill_fpool(void *args) {
        printf("Filling Pool...\n");
        int response = 1;
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        float now = 0.0;

        int min, i, next_i;
        while (response) {
                min = MAX_FLOWS;
                // Deciding on what flow list to append to
                for (i = 0; i < MAX_POOL; ++i) {
                        if (main_fpool[i].nflows == 0) {
                                next_i = i;
                                break;
                        }
                        else if (main_fpool[i].nflows < min) {
                                min = main_fpool[i].nflows;
                                next_i = i;
                        }
                }
                // Setting new time
                clock_gettime(CLOCK_MONOTONIC_RAW, &end);
                now = (float)(end.tv_sec - start.tv_sec) +   // Seconds +
                      (float)(end.tv_nsec - start.tv_nsec) / 1000000000;  // Nanoseconds
                response = __thread_add_candidates(now, &main_fpool[next_i]);
        }
}

int __thread_add_candidates(double time, flow_list_t *curList) {
    grand_packet_t *next;
    grand_packet_t *listPtr = curList -> flows;
    int size = curList -> nflows;

    // Change assesses if anything other than the first entry was implemented
    int res;
    int change = 0;

    if (size == 0 && (res = get_next(&listPtr[size], time))) {
        printf("Added at time %f\n", time);
        // Setting first to itself for round_robin scheduler
        listPtr[0].next = &listPtr[0];
        listPtr[0].last = &listPtr[0];
        size = 1;
    }

    // While loop because many packets will come in at each second
    while (res = get_next(&listPtr[size], time)) {
        // Reached EOF
        if (res == -1)
            break;

        // Link packets together for round-robin scheduler
        listPtr[size - 1].next = &listPtr[size];
        listPtr[size].last = &listPtr[size - 1];
        ++size;
    }

    // Complete round robin circle
    listPtr[size - 1].next = &listPtr[0];
    listPtr[0].last = &listPtr[size - 1];

    // Store stize value back into curList
    curList -> nflows = size;

    // If we have reached EOF, this function shouldn't be called anymore
    if (res == -1) {
            printf("We're done here!\n");
            sleep(3);
        return 0;
    }
    else
        return 1;
}
