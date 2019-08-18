#ifndef NEXT_H
#define NEXT_H
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>
// For sockaddr_ll
#include <linux/if_packet.h>

#include "conf.h"
#define MTU 1500

extern struct sockaddr_ll addr;

typedef struct grand_packet grand_packet_t;
struct grand_packet
{
	struct msghdr msg;
	struct iovec iov;
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
