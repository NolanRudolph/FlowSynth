#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdint.h>

uint64_t rdtscp(void);

int main(int argc, char* argv[])
{
	unsigned long long x,y;
    printf("Calculating Clocks Per Second.\n");
	x = rdtscp();
    sleep(1);
    y = rdtscp();
	printf("Clocks Per Second is %llu\n", y - x);

	// Clock Get Time Variables
	struct timespec start, end;
	float now;

	// RDTSC Variables
	unsigned long long a,b;
	int i;

	// Start Times 
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	a = rdtscp();

	for (i=0; i < 10000000; i++) {
		int *a = (int *)malloc(sizeof(int));
		free(a);
	}

	// End Times
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	b = rdtscp();

	// Config Clock Get Time - Net Time
	float d_sec = (end.tv_sec - start.tv_sec);
	float d_nan = (end.tv_nsec - start.tv_nsec);
	now = (float)d_sec + (d_nan / 1000000000);

	printf("Actual Time is %f.\n", now);
	
	unsigned long long rdtsc_now = (b - a);
	printf("RDTSC Testing Time: %f.\n", (float)rdtsc_now / (float)(y - x));
	return 0;
}

uint64_t rdtscp(void) {
  uint32_t lo, hi;
  __asm__ volatile ("rdtscp"
      : /* outputs */ "=a" (lo), "=d" (hi)
      : /* no inputs */
      : /* clobbers */ "%rcx");
  return (uint64_t)lo | (((uint64_t)hi) << 32);
}
