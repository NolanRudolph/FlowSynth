#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>

int main() {

	struct timespec start, end;	
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	float now;

	int i;
	for (i = 0; i < 1000000; ++i) {
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		now = (float)(end.tv_sec - start.tv_sec) +   // Seconds +
               (float)(end.tv_nsec > start.tv_nsec ? // Conditional
               end.tv_nsec - start.tv_nsec :         // True
               start.tv_nsec - end.tv_nsec)          // False
               / 1000000000;
		printf("Now is %f.\n", now);
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);

	unsigned long d_sec = (end.tv_sec - start.tv_sec);
	printf("Seconds: %lu\n", d_sec);
	
	float d_nan;
	d_nan = end.tv_nsec > start.tv_nsec ? end.tv_nsec - start.tv_nsec : 
										  start.tv_nsec - end.tv_nsec;
	printf("Nanoseconds: %lu\n", d_nan);

	float net = (float)d_sec;
	printf("Net is %f\n", net);

	float extra = d_nan / 1000000000;
	printf("We want to add extra: %f\n", extra);
	
	net += extra;
	
	printf("Net Time: %f\n", net);
	return 0;

}
