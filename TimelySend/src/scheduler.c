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
            if (temp -> cur_time <= now) {
                // Sending packet & Adjusting packet attributes
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
        
        # if 0  // Packet Retrieval Testing
            printf("\n\n***** TESTING *****\n");
            printf("\n*** GRAND PACKET ATTRIBUTES ***\n");
            printf("Packets left is %d\n", grand_list[0].packets_left);
            printf("Current time is %lf\n", grand_list[0].cur_time);
            
            printf("\n*** ETHERNET ATTRIBUTES ***\n");
            struct ether_header *ether = (struct ether_header *)(grand_list[0].buff);
            printf("Ether dest is ");
            int i;
            for (i = 0; i < 6; ++i) {
                printf("%u", ether -> ether_shost[i]);
            }
            printf("\n");
            printf("Ether host is ");
            for (i = 0; i < 6; ++i) {
                printf("%u", ether -> ether_dhost[i]);
            }
            printf("\n");
            
            printf("\n*** IP ATTRIBUTES ***\n");
            struct ip *ip = (struct ip *)(grand_list[0].buff + sizeof(struct ether_header));
            printf("IP protocol is %hu\n", ip -> ip_p);
            printf("IP TTL is %hu\n", ip -> ip_ttl);
            printf("IP's Header Length is %hu\n", ip -> ip_hl);
            printf("IP's Total Length is %hu\n", ntohs(ip -> ip_len));
            printf("IP's Checksum is %hu\n", ntohs(ip -> ip_sum));

            printf("\n*** TCP ATTRIBUTES ***\n");
            struct tcphdr *tcp = (struct tcphdr *)(grand_list[0].buff + \
                                   sizeof(struct ether_header) + sizeof(struct ip));
            printf("TCP source is %d\n", ntohs(tcp -> source));
            printf("TCP dest is %d\n", ntohs(tcp -> dest));
            printf("TCP offset is %d\n", tcp -> doff);
            printf("TCP checksum is %d\n", tcp -> check);

            printf("\n\n");
        #endif
    }
    
    // While loop because many packets will come in at each second
    while (res = get_next(&grand_list[size], time)) {
        
        // Reached EOF
        if (res == -1)
            break;
        
        // For completing circles
        change = 1;
        
        // Link packets together for round-robin scheduler
        grand_list[size - 1].next = &grand_list[size];
        grand_list[size].last = &grand_list[size - 1];
        
        ++size;
        
        # if 0  // Packet Retrieval Testing
            printf("\n\n***** TESTING *****\n");
            printf("\n*** GRAND PACKET ATTRIBUTES ***\n");
            printf("Packets left is %d\n", grand_list[size - 1].packets_left);
            printf("Current time is %lf\n", grand_list[size - 1].cur_time);
            
            printf("\n*** ETHERNET ATTRIBUTES ***\n");
            struct ether_header *ether = (struct ether_header *)\
                                        (grand_list[size - 1].buff);
            printf("Ether dest is ");
            int i;
            for (i = 0; i < 6; ++i) {
                printf("%u", ether -> ether_shost[i]);
            }
            printf("\n");
            printf("Ether host is ");
            for (i = 0; i < 6; ++i) {
                printf("%u", ether -> ether_dhost[i]);
            }
            printf("\n");
            
            printf("\n*** IP ATTRIBUTES ***\n");
            struct ip *ip = (struct ip *)(grand_list[size - 1].buff + \
                            sizeof(struct ether_header));
            printf("IP protocol is %hu\n", ip -> ip_p);
            printf("IP TTL is %hu\n", ip -> ip_ttl);
            printf("IP's Header Length is %hu\n", ip -> ip_hl);
            printf("IP's Total Length is %hu\n", ntohs(ip -> ip_len));
            printf("IP's Checksum is %hu\n", ntohs(ip -> ip_sum));


            if (ip -> ip_p == 6) {
                printf("\n*** TCP ATTRIBUTES ***\n");
                struct tcphdr *tcp = (struct tcphdr *)(grand_list[size - 1].buff + \
                                       sizeof(struct ether_header) + sizeof(struct ip));
                printf("TCP source is %d\n", ntohs(tcp -> source));
                printf("TCP dest is %d\n", ntohs(tcp -> dest));
                printf("TCP offset is %d\n", tcp -> doff);
                printf("TCP checksum is %d\n", tcp -> check);
            }
            printf("\n\n");
        #endif
    }
    
    if (change) {
        // Complete the circle for round-robin scheduler
        grand_list[size - 1].next = &grand_list[0];
        grand_list[0].last = &grand_list[size - 1];
        
    }
    
    // If we have reached EOF, this function shouldn't be called anymore
    if (res == -1) {
        return 0;
    }
    else
        return 1;
}

void send_packet(struct grand_packet packet) {
    # if 0  // Evaluate packet before sending
        printf("\n\n***** SENDING THIS *****\n");
        printf("\n*** GRAND PACKET ATTRIBUTES ***\n");
        printf("Packets left is %d\n", packet.packets_left);
        printf("Current time is %lf\n", packet.cur_time);

        printf("\n*** ETHERNET ATTRIBUTES ***\n");
        struct ether_header *ether = (struct ether_header *)\
                                    (packet.buff);
        printf("Ether dest is ");
        int i;
        for (i = 0; i < 6; ++i) {
            printf("%u", ether -> ether_shost[i]);
        }
        printf("\n");
        printf("Ether host is ");
        for (i = 0; i < 6; ++i) {
            printf("%u", ether -> ether_dhost[i]);
        }
        printf("\n");

        printf("\n*** IP ATTRIBUTES ***\n");
        struct ip *ip = (struct ip *)(packet.buff + \
                        sizeof(struct ether_header));
        printf("IP protocol is %hu\n", ip -> ip_p);
        printf("IP TTL is %hu\n", ip -> ip_ttl);
        printf("IP's Header Length is %hu\n", ip -> ip_hl);
        printf("IP's Total Length is %hu\n", ntohs(ip -> ip_len));


        if (ip -> ip_p == 6) {
            printf("\n*** TCP ATTRIBUTES ***\n");
            struct tcphdr *tcp = (struct tcphdr *)(packet.buff + \
                                   sizeof(struct ether_header) + sizeof(struct ip));
            printf("TCP source is %d\n", ntohs(tcp -> source));
            printf("TCP dest is %d\n", ntohs(tcp -> dest));
            printf("TCP offset is %d\n", tcp -> doff);
            printf("TCP checksum is %d\n", tcp -> check);
        }
        printf("\n\n");
    #endif
    
    #if 0  // Accommodate for remainder bytes from total_bytes/total_packets
    if (packet.packets_left == 1) {        
        addPayload((uint8_t *)(packet.buff + packet.length), packet.f_bytes);
        packet.length += packet.f_bytes;
    }
    #endif
        
    if (sendto(sockfd, packet.buff, packet.length, 0, \
            (struct sockaddr *)&addr, sizeof(struct sockaddr_ll)) < 0) {
            perror("sendto() error");
            exit(EXIT_FAILURE);
    }
}
