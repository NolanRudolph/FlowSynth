#include "conf.h"

void configure_ETHER(struct ether_header *ether, char *dst, char *src) {
    int i;
    for (i = 0; dst[i] != '\0'; ++i) {
        ether -> ether_dhost[i] = dst[i];
    }
    for (i = 0; src[i] != '\0'; ++i) {
        ether -> ether_shost[i] = src[i];
    }
    ether -> ether_type = htons(ETHERTYPE_IP);
}

void configure_IP(struct ip *ip, unsigned short int version, unsigned \
        short int tos, char *src, char *dst, unsigned short int proto) {

    ip -> ip_v = htons(version);
    ip -> ip_hl = htons(5);
    ip -> ip_tos = htons(tos);
    ip -> ip_p = htons(proto);
    ip -> ip_ttl = htons(255);

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

}


void configure_UDP(struct udphdr *udp, unsigned short int source, \
                   unsigned short int dest) {
                             
    udp -> source = htons(source);
    udp -> dest = htons(dest);

}
