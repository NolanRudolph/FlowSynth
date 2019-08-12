#include "scheduler.h"

// Dummy Packet for Socket Testing
grand_packet_t dummy_packet =
{
        {
                // Ethernet
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x08, 0x00,
                // IP Header
                0x45, 0x01, 0x00, 0x27, 0x00, 0x00, 0x00, 0x00, 0xff, 0x06,
                0xbf, 0x99, 0x9d, 0xf0, 0x0b, 0x23, 0x29, 0x4b, 0x29, 0xc8,
                // TCP Header
                0x01, 0xbb, 0xda, 0x6b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x50, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00
        },
        0,
        0,
        0,
        54, // Length
};

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
    
    if (sendto(sockfd, dummy_packet.buff, dummy_packet.length, 0, \
       (struct sockaddr *)&addr, sizeof(struct sockaddr_ll)) < 0) {
            perror("sendto() error");
            exit(EXIT_FAILURE);
    }

    thread_pool_init();
}

void * round_robin(void *arg) {
    // Flow List Pointer Cast for Easier Accessibility
    flow_list_t *curList = (flow_list_t *)arg;
    grand_packet_t *listPtr = curList -> flows;
    //printf("Round Robin: Thread %d\n", curList -> id);
    // Local size variable, manipulated & stored into memory in while loop
    int size = curList -> nflows;
    printf("(RR) nflow: %#010x // val: %d\n", &curList -> nflows, curList -> nflows);

    // Time variables for round_robin scheduling
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    float now = 0.0;

    // Current Flow Pointer
    grand_packet_t *pCurFlow;
    pCurFlow = &listPtr[0];

    // Current Flow
    grand_packet_t curFlow;

    int sAddrSize = sizeof(struct sockaddr_ll);
    struct sockaddr *memAddr = (struct sockaddr *)&addr;

    int i;
    while (size != 0) {
        for (i = 0; size != 0 && i < size; ++i) {
            // Dereference Flow Pointer and use Local Variable
	    curFlow = *pCurFlow;
            while (curFlow.cur_time < now) {

		printf("Sending....\n");
                // Sending Packet
                if(sendto(sockfd, curFlow.buff, curFlow.length, 0, \
                      memAddr, sAddrSize) < 0) {
			perror("sendto() error");
			exit(EXIT_FAILURE);
		}

                // Adjusting Packet Attributes
                curFlow.packets_left -= 1;
                curFlow.cur_time += curFlow.d_time;

                // If the packet has no packets left, delete it
                if (!curFlow.packets_left) {
                    pCurFlow -> last -> next = pCurFlow -> next;
                    pCurFlow -> next -> last = pCurFlow -> last;
                    size -= 1;
		    // Store into memory
		    curList -> nflows = size;
                    break;
                }
            }
            *pCurFlow = curFlow;
            pCurFlow = pCurFlow -> next;
        }
	// Setting time for next loop through
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        now = (float)(end.tv_sec - start.tv_sec) +   // Seconds +
              (float)(end.tv_nsec - start.tv_nsec) / 1000000000;  // Nanoseconds
    }
    main_pool.thread_list[curList -> id].status = INACTIVE;
}
