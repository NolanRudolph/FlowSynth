#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include <netdb.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>

#include <arpa/inet.h>
#include <sys/time.h>

#include <errno.h>
#include "conf.h"
#include "next.h"
#include "scheduler.h"


int main(int argc, char* argv[]) {

    // Usage Error Handling
    if (argc != 2) {
            printf("Usage: $ ./a.out [FILE]\n");
            exit(EXIT_FAILURE);
    }	
    
    /* Coordinating with next.c to generate packets based off argv[1] */
    
    // Essentially starting up the module, initializing global variables, etc.
    begin(argv[1]);  // Relay file to "next.c"
    
    /* Round Robin Testing */
    // Initialize sockets
    round_robin_init();
    
    // Actual reading of static dataset and outputting packets
    round_robin();

    return 0;

}