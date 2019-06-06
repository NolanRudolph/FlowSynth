#ifndef NEXT_H
#define NEXT_H
#define MAXBUF 1024
#include <string.h>
#include <math.h>
#include "conf.h"

struct grand_packet
{
	char buff[MAXBUF];
	unsigned int packets_left;
	float d_time;
	double cur_time;
	unsigned short length;
	struct grand_packet *last;
	struct grand_packet *next;
};

void begin(char *fname);

void stop(void);

void get_first(void);

struct grand_packet * get_next(void);

double get_next_time(void);

#endif
