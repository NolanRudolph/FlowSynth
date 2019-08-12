#ifndef NEXT_H
#define NEXT_H
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>

#include "conf.h"
#include "structs.h"

void begin(char *fname, unsigned char *src, unsigned char *dst);

int get_next(struct grand_packet *, float);

#endif
