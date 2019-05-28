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

    begin(argv[1]);  // Relay file to "next.c"
    
    struct grand_packet *ret_packet;
    ret_packet = get_next(ether, ip, icmp, igmp, tcp, udp);
    
#if 1 // Testing if data is in correct location
    printf("Received packet at %#010x\n", ret_packet);
    
    struct tcphdr *temp = (struct tcphdr *)(ret_packet -> buff + \
                           sizeof(struct ether_header) + sizeof(struct ip));
    printf("Looking for TCP source at %#010x\n", ret_packet -> buff + \
                           sizeof(struct ether_header) + sizeof(struct ip));
    //printf("TCP source is %d\n", temp->source); // Comment me out if not TCP
    
    printf("\n\n*** TESTING FOR CORRECT GRAND PACKET ATTRIBUTES ***\n\n");
    printf("Packets Left: %d\n", ret_packet -> packets_left);
    printf("Delta Time: %f\n", ret_packet -> d_time);
    printf("Current Time: %f\n", ret_packet -> cur_time);
    printf("Length of Packet: %d\n", ret_packet -> length);
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