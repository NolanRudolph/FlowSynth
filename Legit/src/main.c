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

void send_packet(int socket, struct grand_packet packet, struct sockaddr_in addr);


int main(int argc, char* argv[]) {

    // Usage Error Handling
    if (argc != 2) {
            printf("Usage: $ ./a.out [FILE]\n");
            exit(EXIT_FAILURE);
    }	

    /* Socket Creation */
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
            perror("socket() error");
            exit(EXIT_FAILURE);
    }

    /* Initialize Delivery Address */

    // Specify Address
    char address[] = "127.0.0.1";

    // Specifying Address for Socket
    struct sockaddr_in sendto_addr;
    sendto_addr.sin_family = AF_INET;
    sendto_addr.sin_port = htons(9001);
    sendto_addr.sin_addr.s_addr = inet_addr(address);


    /* Initialize All Protocol Structures */
    struct ether_header *ether = (struct ether_header *)\
                                  malloc(sizeof(struct ether_header));
    ether -> ether_type = ETHERTYPE_IP; // Ethernet type remains constant
    
    struct ip *ip = (struct ip *)malloc(sizeof(struct ip));
    
    struct icmp *icmp = (struct icmp *)malloc(sizeof(struct icmp));

    struct igmp *igmp = (struct igmp *)malloc(sizeof(struct igmp));

    struct tcphdr *tcp = (struct tcphdr *)malloc(sizeof(struct tcphdr));

    struct udphdr *udp = (struct udphdr *)malloc(sizeof(struct udphdr));

    /* Coordinating with next.c to generate packets based off argv[1] */
    // Essentially starting up the module, initializing global variables, etc.
    begin(argv[1]);  // Relay file to "next.c"
    
    struct grand_packet *grand_list = (struct grand_packet *)malloc\
                                      (sizeof(struct grand_packet) * 1000);
    int list_i = 0; // Number of grand_packet's already allocated
    
    printf("The size of a packet is %d\n", sizeof(struct grand_packet));
    printf("Grand List pointer starts at %#010x\n", &grand_list);
    struct grand_packet *ret_packet;
    while ((ret_packet = get_next(ether, ip, icmp, igmp, tcp, udp)) != NULL) {
        printf("Received packet at %#010x\n", ret_packet);

        memcpy(grand_list + list_i, ret_packet, sizeof(struct grand_packet));
        ++list_i;
    }
    
#if 0  // TCP Specefic Testing
    struct tcphdr *temp = (struct tcphdr *)(ret_packet -> buff + \
                           sizeof(struct ether_header) + sizeof(struct ip));

    // printf("Looking for TCP source at %#010x\n", ret_packet -> buff + \
                           sizeof(struct ether_header) + sizeof(struct ip));
    //printf("TCP source is %d\n", temp->source); 
#endif

#if 1 // Testing if data is in correct location
    printf("\n\n*** TESTING FOR CORRECT GRAND PACKET ATTRIBUTES ***\n\n");
    int i;
    for (i = 0; i < list_i; ++i) {
        printf("Got to %d\n", i);
        printf("Packets Left: %d\n", grand_list[i].packets_left);
        printf("Delta Time: %f\n", grand_list[i].d_time);
        printf("Current Time: %f\n", grand_list[i].cur_time);
        printf("Length of Packet: %d\n\n", grand_list[i].length);
    }

#endif

	
    free(ether);
    free(ip);
    free(icmp);
    free(igmp);
    free(tcp);
    free(udp);
    close(sockfd);

    return 0;

}

void send_packet(int socket, struct grand_packet packet, struct sockaddr_in addr){
    if (sendto(socket, &packet, sizeof(struct grand_packet), 0, \
            (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("TCP sendto() error");
            exit(EXIT_FAILURE);
    }
}