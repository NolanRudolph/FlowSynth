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
    int change = 0;
    
    if (size == 0 && get_next_time() <= time) {
        printf("Adding first packet.\n");
        
        next = get_next();
        
        // EOF Checking, and will stop scheduler from further add_candidates()
        if (next == NULL) 
            return 0;
        
        // Copy packet contents to first address in grand_packet
        memcpy(&grand_list[0], next, sizeof(struct grand_packet));
        grand_list[0].next = &grand_list[0];
        grand_list[0].last = &grand_list[0];
        
        printf("We added packet with packets_left %d at size %d\n", \
                grand_list[size].packets_left, size);
        size = 1;
    }
    // While loop because many packets will come in at each second
    while (get_next_time() <= time) {
        // Detected a new packet that should be implemented
        change = 1;
        
        next = get_next();
        
        // EOF Checking, we must break the while loop
        if (next == NULL)
            break;
        
        // Copy packet contents to next unused address in grand_packet
        memcpy(&grand_list[size], next, sizeof(struct grand_packet));
        
        // Link packets together, hence "round" robin
        grand_list[size - 1].next = &grand_list[size];
        grand_list[size].last = &grand_list[size - 1];

        printf("We added packet with packets_left %d at size %d\n", \
                grand_list[size].packets_left, size);
        ++size;
    }
    // We changed our list, so we need to complete the circle
    if (change) {
        printf("Adding more...\n");
        // Complete the circle and return to scheduler
        grand_list[size - 1].next = &grand_list[0];
        grand_list[0].last = &grand_list[size - 1];
        
        int i;
        struct grand_packet *temp;
        temp = &grand_list[0];
        for (i = 0; i < size; ++i) {
            printf("Packet has packets_left %d\n", temp -> packets_left);
            temp = temp -> next;
            
        }
    }
    // If we have reached EOF, this function shouldn't be called anymore
    if (get_next_time() == INFINITY) {
        printf("We're ending at size %d\n", size);
        return 0;
    }
    else
        return 1;
}

int p_size;

void send_packet(struct grand_packet packet) {
    p_size = packet.length;
    
#if 0 // TCP Buffer Testing
    struct tcphdr *temp = (struct tcphdr *)(packet.buff + \
                           sizeof(struct ether_header) + sizeof(struct ip));
    printf("TCP source is %d\n", temp -> source);
#endif
    
    if (sendto(sockfd, &packet.buff, p_size, 0, \
            (struct sockaddr *)&addr, sizeof(struct sockaddr_ll)) < 0) {
            perror("sendto() error");
            exit(EXIT_FAILURE);
    }
}