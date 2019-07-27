#ifndef NEXT_H
#define NEXT_H
#define MAXBUF 1500
#include <string.h>
#include <math.h>
#include "conf.h"
#include <unistd.h>

struct grand_packet
{
	char buff[MAXBUF];
	unsigned int packets_left;
	float d_time;
	double cur_time;
	unsigned int length;
	unsigned int f_bytes;
	struct grand_packet *last;
	struct grand_packet *next;
};

void begin(char *fname, unsigned char *src, unsigned char *dst);

void stop(void);

void get_first(void);

int get_next(struct grand_packet *, time_t);

#endif
