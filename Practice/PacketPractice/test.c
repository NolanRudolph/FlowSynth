#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <netinet/ip.h>
#include <netinet/ether.h>
#include <netinet/udp.h>

#include <sys/socket.h>

#include <errno.h>

#define BUFFER 1024

int main(int argc, char *argv[]) {

    /* Socket Creation */
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
            perror("socket() error");
            exit(EXIT_FAILURE);
    }

    // This will hold all of the packet's contents
    char sendbuf[BUFFER];
    
    // This will hold the size of the whole packet
    size_t total_len = 0;

    
    /* Address Stuff */
    in_addr_t address = inet_addr("127.0.0.1");

    struct sockaddr_in sendto_addr;
    sendto_addr.sin_family = AF_INET;
    sendto_addr.sin_port = htons(9001);
    sendto_addr.sin_addr.s_addr = address;


    /* LAYER 2 - Ethernet Header Configuration */
    struct ether_header *ether = (struct ether_header *)sendbuf;
    int i;
    for (i = 0; i < 5; ++i) {
            ether -> ether_dhost[i] = 0x00;
            ether -> ether_shost[i] = 0x00;
    }
    ether -> ether_type = ETHERTYPE_IP;

    total_len += sizeof(struct ether_header);

    
    /* LAYER 3 - IP Header Configuration */
    struct ip *ip = (struct ip *)(sendbuf + sizeof(struct ether_header));

    ip -> ip_hl = 5;
    ip -> ip_v = 4;
    ip -> ip_tos = 0;
    ip -> ip_p = 17;
    ip -> ip_ttl = 255;
    ip -> ip_src.s_addr = address;
    ip -> ip_dst.s_addr = address;

    total_len += sizeof(struct ip);
    
    /* LAYER 4 - UDP Header Configuration */
    struct udphdr *udp = (struct udphdr *)(sendbuf + \
                          sizeof(struct ether_header) + sizeof(struct ip));

    udp -> source = 123;
    udp -> dest = 321;
    udp -> check = 0;
    
    total_len += sizeof(struct udphdr);
    
    // Giberish UDP Data
    sendbuf[total_len++] = 0x01;
    sendbuf[total_len++] = 0x02;
    sendbuf[total_len++] = 0x03;
    sendbuf[total_len++] = 0x04;
    
    udp -> len = htons(total_len - sizeof(struct ether_header) - sizeof(struct ip));
    ip -> ip_len = htons(total_len - sizeof(struct ether_header));
    
    /* Send Packet */
    printf("Sockfd is %d\n", sockfd);
    printf("Total length is %d\n", total_len);
    
    if (sendto(sockfd, sendbuf, total_len, 0, \
        (const struct sockaddr*)&sendto_addr, sizeof(sendto_addr)) < 0) {
            printf("Error sending packet: Error %d.\n", errno);
            perror("sendto Error");
            exit(EXIT_FAILURE);
    } 

    close(sockfd);

}
