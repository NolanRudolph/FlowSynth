#ifndef NEXT_H
#define NEXT_H
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include "conf.h"
#define MTU 1500

typedef struct grand_packet grand_packet_t;
struct grand_packet
{
	char buff[MTU];
	unsigned int packets_left;
	float d_time;
	double cur_time;
	unsigned int length;
	unsigned int f_bytes;
	struct grand_packet *last;
	struct grand_packet *next;
};

void begin(char *fname, unsigned char *src, unsigned char *dst);

int get_next(struct grand_packet *, float);

#endif
