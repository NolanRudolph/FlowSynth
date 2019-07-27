#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <time.h>
#include "rdtsc.h"
#define N (1024*1024*2)


int main(int argc, char* argv[])
{
  unsigned long long a,b;
  unsigned long long min,max;
  char* p;
  int i;
  float n;

  p = (char*)malloc(N);
  assert( p!=(char*)0 );

  max = 0;
  min = UINT64_MAX;

  for(i=0; i<N; i++ ) {
    a = rdtsc();
	int k;
	for (k = 0; k < 3; k++) {
		n = (float)k + (float)i;
	}
    b = rdtsc() - a;
    if( b > max ) max = b;
    else if( b < min ) min = b;
  }
  printf("min=%llu\n", min);
  printf("max=%llu\n", max);
  return 0;
}
