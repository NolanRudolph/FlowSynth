#include <stdio.h>
#include <stdlib.h>

#include "conf.h"
#include "next.h"
#include "scheduler.h"

#define _BSD_SOURCE 1
#define __FAVOR_BSD 1


int main(int argc, char* argv[]) {

    // Usage Error Handling
    if (argc != 5) {
            printf("Usage: $ ./a.out [FILE] [Ether sHost] [Ether dHost] [Interface]\n");
            exit(EXIT_FAILURE);
    }	
    
    /* Coordinating with next.c to generate packets based off argv[1] */
    
    // Essentially starting up the module, initializing global variables, etc.
    begin(argv[1], argv[2], argv[3]);  // Relay file to "next.c"
    
    /* Round Robin Testing */
    // Initialize sockets
    round_robin_init(argv[4]);

    // Actual reading of static dataset and outputting packets
    round_robin();

    return 0;

}
