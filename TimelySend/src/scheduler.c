#include "scheduler.h"

/* Grand_list Variables */
// Grand_list will hold all of currently assessed entries, capped at a million
grand_packet_t grand_list[1000000];

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

void round_robin_init(char *interface) {
    // Resolving correct interface number given interface name
    int num_interface = -1;
    struct ifaddrs *addrs;
    getifaddrs(&addrs);
    int i = 1;
    do {
        if (strcmp(addrs -> ifa_name, interface) == 0) {
            num_interface = i;
            break;
        }
        addrs = addrs -> ifa_next;
        ++i;
    } while (addrs -> ifa_next != NULL);
    
    if (num_interface == -1) {
        printf("Could not locate a network interface under this name.\n");
        exit(EXIT_FAILURE);
    }
    
    // Adding socket address attributes
    addr.sll_ifindex = num_interface;
    addr.sll_halen = ETH_ALEN;
  
    // Creating socket with error handling
    if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) < 0) {
            perror("socket() error");
            exit(EXIT_FAILURE);
    }
}

void round_robin() {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    float now = 0.0;
    float decimal;

    grand_packet_t *pCurFlow;
    pCurFlow = &grand_list[0];
    grand_packet_t curFlow;

    int sAddrSize = sizeof(struct sockaddr_ll);
    struct sockaddr *memAddr = (struct sockaddr *)&addr;

    int i;
    while (size != 0 || response) {
        for (i = 0; size != 0 && i < size; ++i) {
            curFlow = *pCurFlow;
            while (curFlow.cur_time < now) {
                // Sending Packet
    		if (unlikely(sendmsg(sockfd, &curFlow.msg, 0) < 0)) {
            		perror("sendto() error");
			exit(EXIT_FAILURE);
		}
                // Adjusting Packet Attributes
                curFlow.packets_left -= 1;
                curFlow.cur_time += curFlow.d_time;

                // If the packet has no packets left, delete it
                if (unlikely(!curFlow.packets_left)) {
                    pCurFlow -> last -> next = pCurFlow -> next;
                    pCurFlow -> next -> last = pCurFlow -> last;
                    size -= 1;
                    break;
                }
            }
            *pCurFlow = curFlow;
            pCurFlow = pCurFlow -> next;
        }
        // Setting new time
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        now = (float)(end.tv_sec - start.tv_sec) +   // Seconds +
              (float)(end.tv_nsec - start.tv_nsec) / 1000000000;  // Nanoseconds

        // If a new entry falls within the time frame, append to our grand_list
        if (response)
            response = add_candidates(now);
    }

    close(sockfd);
}

int add_candidates(double time) {
    struct grand_packet *next;

    // Change assesses if anything other than the first entry was implemented
    int res;
    int change = 0;

    if (size == 0 && (res = get_next(&grand_list[size], time))) {

        // Setting first to itself for round_robin scheduler
        grand_list[0].next = &grand_list[0];
        grand_list[0].last = &grand_list[0];

        size = 1;
    }

    // While loop because many packets will come in at each second
    while (res = get_next(&grand_list[size], time)) {

        // Reached EOF
        if (res == -1)
            break;

        // Link packets together for round-robin scheduler
        grand_list[size - 1].next = &grand_list[size];
        grand_list[size].last = &grand_list[size - 1];

        ++size;
    }

    // Complete round robin circle
    grand_list[size - 1].next = &grand_list[0];
    grand_list[0].last = &grand_list[size - 1];


    // If we have reached EOF, this function shouldn't be called anymore
    if (res == -1) {
        return 0;
    }
    else
        return 1;
}
