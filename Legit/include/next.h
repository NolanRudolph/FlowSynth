#ifndef NEXT_H
#define NEXT_H
#define MAXBUF 1024
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conf.h"

struct grand_packet
{
	char buff[MAXBUF];
	unsigned int packets_left;
	float d_time;
	double cur_time;
	unsigned short length;
	struct grand_packet *next;
};

void begin(char *fname);

void stop(void);

struct grand_packet * get_next(void);

#endif
