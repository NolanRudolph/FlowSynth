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
    const time_t start = time(NULL);
    double now = 0.0;
    
    struct grand_packet *temp;
    temp = &grand_list[0];
    
    int i;    
    while (size != 0 || response) {
        for (i = 0; size != 0 && i < size; ++i) {
            while (temp -> cur_time < now) {
                // Sending Packet
                printf("Sending packet.\n");
                if (sendto(sockfd, temp -> buff, temp -> length, 0, \
                   (struct sockaddr *)&addr, sizeof(struct sockaddr_ll)) < 0) {
                        perror("sendto() error");
                        exit(EXIT_FAILURE);
                }

                // Adjusting Packet Attributes
                temp -> packets_left -= 1;
                temp -> cur_time += temp -> d_time;
                printf("My new time is %f\n", temp -> cur_time);
            
                // If the packet has no packets left, delete it
                if (temp -> packets_left == 0) {
                    temp -> last -> next = temp -> next;
                    temp -> next -> last = temp -> last;
                    temp = temp -> next;
                    size -= 1;
                }
            }
            temp = temp -> next;
        }
        
        now = time(NULL) - start;
        
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
