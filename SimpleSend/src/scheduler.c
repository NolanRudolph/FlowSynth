#include "scheduler.h"


void round_robin(struct grand_packet *grand_list, int size) {
    const time_t start = time(NULL);
    printf("Time is %lld\n", start);
    
    struct grand_packet *temp;
    temp = grand_list;
    
    int i;    
    while (size != 0) {
        for (i = 0; i <= size; ++i) {
            temp -> packets_left -= 1;
            if (temp -> packets_left == 0) {
                another:
                temp -> last -> next = temp -> next;
                temp -> next -> last = temp -> last;
                temp = temp -> next;
                size -= 1;
                
                // In the odd occurrence that two consecutive packets
                // have the same number of packets, we repeat this process
                if (temp -> packets_left == 0)
                    goto another;
            }
            temp = temp -> next;
        }
        printf("Size is %d\n", size);
    }

    printf("Done\n");
}

void its_time(struct grand_packet *packet, int list_i) {
    
}