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

  a = rdtsc();

  for(i=0; i<N; i++ ) {
	n = i;
  }

  b = rdtsc() - a;
  printf("Time a = %llu\n", a);
  printf("Time b = %llu\n", b);
  printf("Net time = %.6f\n", b / CLOCKS_PER_SEC);
  return 0;
}
