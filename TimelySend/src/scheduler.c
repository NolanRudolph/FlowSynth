#include "scheduler.h"

/* Grand_list Variables */
// Grand_list will hold all of currently assessed entries, capped at a million
struct grand_packet grand_list[1000000];
// Size will hold the current size of our grand_list (i.e. how many packets it holds)
int size = 0;

// Response is used to track if add_candidates should be called any further
int response = 1;


/* Packet Sending Variables */
// Socket ID
int sockfd;

// Permanent Address
const char address[] = "127.0.0.1";

// Address for Socket
struct sockaddr_ll addr;

void round_robin_init() {
    // Adding socket address attributes
    addr.sll_ifindex = 1;
    addr.sll_halen = ETH_ALEN;
  
    // Creating socket with error handling
    if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) < 0) {
            perror("socket() error");
            exit(EXIT_FAILURE);
    }
}

void round_robin() {
    const time_t start = time(NULL);
    double now = 0.0;
    
    struct grand_packet *temp;
    temp = &grand_list[0];
    
    int i;    
    while (size != 0 || response) {
        for (i = 0; size != 0 && i < size; ++i) {
            if (temp -> cur_time <= now) {
                // Sending packet & Adjusting packet attributes
                printf("Sending packet %d\n", i);
                send_packet(*temp);
                temp -> packets_left -= 1;
                temp -> cur_time += temp -> d_time;
            
                // If the packet has no packets left, delete it
                if (temp -> packets_left == 0) {
                    temp -> last -> next = temp -> next;
                    temp -> next -> last = temp -> last;
                    temp = temp -> next;
                    size -= 1;
                }
                else
                    temp = temp -> next;
            }
            else
                temp = temp -> next;
        }
        
        now = time(NULL) - start;
        
        // If a new entry falls within the time frame, append to our grand_list
        if (response)
            response = add_candidates(now);
    }

    close(sockfd);
    printf("Done\n");
}

int add_candidates(double time) {
    struct grand_packet *next;
    
    // Change assesses if anything other than the first entry was implemented
    int res;
    int change = 0;
    
    if (size == 0 && (res = get_next(&grand_list[size], time))) {
        printf("Retrived response from passed in %#010x\n", &grand_list[size]);
        
        // Setting first to itself for round_robin scheduler
        grand_list[0].next = &grand_list[0];
        grand_list[0].last = &grand_list[0];
        
        size = 1;
        
        printf("We added first packet with packets_left %d // size is now %d\n", \
                grand_list[size - 1].packets_left, size);
        
        # if 0  // Packet Retrieval Testing
            printf("\n\n***** TESTING *****\n");
            printf("Packets left is %d\n", grand_list[0].packets_left);
            printf("Current time is %lf\n", grand_list[0].cur_time);

            struct tcphdr *temp = (struct tcphdr *)(grand_list[0].buff + \
                                   sizeof(struct ether_header) + sizeof(struct ip));
            printf("TCP source is %d\n", ntohs(temp -> source));

            printf("\n\n");
        #endif
    }
    
    // While loop because many packets will come in at each second
    while (res = get_next(&grand_list[size], time)) {
        printf("Retrived response from passed in %#010x\n", &grand_list[size]);
        
        if (res == -1) {
            printf("But it was the EOF\n");
            break;
        }
        
        // For completing circles
        change = 1;
        
        // Link packets together for round-robin scheduler
        grand_list[size - 1].next = &grand_list[size];
        grand_list[size].last = &grand_list[size - 1];
        
        ++size;
        
        printf("We added new packet with packets_left %d // size is now %d\n", \
                grand_list[size - 1].packets_left, size);
    }
    
    if (change) {
        // Complete the circle for round-robin scheduler
        grand_list[size - 1].next = &grand_list[0];
        grand_list[0].last = &grand_list[size - 1];
        
    }
    
    // If we have reached EOF, this function shouldn't be called anymore
    if (res == -1) {
        printf("Reached EOF at size %d\n", size);
        return 0;
    }
    else
        return 1;
}

void send_packet(struct grand_packet packet) {
    if (sendto(sockfd, &packet.buff, packet.length, 0, \
            (struct sockaddr *)&addr, sizeof(struct sockaddr_ll)) < 0) {
            perror("sendto() error");
            exit(EXIT_FAILURE);
    }
}