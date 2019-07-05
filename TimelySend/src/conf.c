#include "conf.h"

void configure_ETHER(struct ether_header *ether, unsigned char *dst, unsigned char *src) {
    int i;
    for (i = 0; i < 6; ++i) {
        ether -> ether_dhost[i] = dst[i];
    }
    for (i = 0; i < 6; ++i) {
        ether -> ether_shost[i] = src[i];
    }
    ether -> ether_type = htons(ETHERTYPE_IP);
}

void configure_IP(struct ip *ip, unsigned char version, unsigned \
        char tos, char *src, char *dst, unsigned char proto) {

    ip -> ip_v = version;
    ip -> ip_hl = 5;
    ip -> ip_tos = tos;
    ip -> ip_p = proto;
    ip -> ip_ttl = 255;
    ip -> ip_sum = 1;
    ip -> ip_src.s_addr = inet_addr(src);
    ip -> ip_dst.s_addr = inet_addr(dst);
    
}


void configure_ICMP(struct icmp *icmp, unsigned short int type, \
                    unsigned short int code) {

    icmp -> icmp_type = htons(type);
    icmp -> icmp_code = htons(code);

}


void configure_IGMP(struct igmp *igmp, unsigned short int type, \
                    unsigned short int code) {

    igmp -> igmp_type = htons(type);
    igmp -> igmp_code = htons(code);
 
}
  
void configure_TCP(struct tcphdr *tcp, unsigned short int source, \
                   unsigned short int dest) {
    
    tcp -> source = htons(source);
    tcp -> dest = htons(dest);
    tcp -> doff = 5;

}


void configure_UDP(struct udphdr *udp, unsigned short int source, \
                   unsigned short int dest) {
                            
    udp -> source = htons(source);
    udp -> dest = htons(dest);
    udp -> len = htons(8); // Change me once constructing real packets

}
