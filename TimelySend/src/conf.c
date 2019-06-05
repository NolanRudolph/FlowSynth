#include "conf.h"

void configure_ETHER(struct ether_header *ether, char *dst, char *src) {
    int i;
    for (i = 0; dst[i] != '\0'; ++i) {
        ether -> ether_dhost[i] = dst[i];
    }
    for (i = 0; src[i] != '\0'; ++i) {
        ether -> ether_shost[i] = src[i];
    }
    ether -> ether_type = ETHERTYPE_IP;
}

void configure_IP(struct ip *ip, unsigned short int version, unsigned \
        short int tos, char *src, char *dst, unsigned short int proto) {

    ip -> ip_v = version;
    ip -> ip_hl = 5;
    ip -> ip_tos = tos;
    ip -> ip_p = proto;
    ip -> ip_ttl = 255;
    
#if 0  // Big endian
    ip -> ip_v = htons(version);
    ip -> ip_hl = htons(5);
    ip -> ip_tos = htons(tos);
    ip -> ip_p = htons(proto);
    ip -> ip_ttl = htons(255);
#endif

    // Temporary Testing on Localhost
    ip -> ip_src.s_addr = inet_addr(src);
    ip -> ip_dst.s_addr = inet_addr(dst);
}


void configure_ICMP(struct icmp *icmp, unsigned short int type, \
                    unsigned short int code) {
    icmp -> icmp_type = type;
    icmp -> icmp_code = code;
    
#if 0  // Big endian
    icmp -> icmp_type = htons(type);
    icmp -> icmp_code = htons(code);
#endif
}


void configure_IGMP(struct igmp *igmp, unsigned short int type, \
                    unsigned short int code) {

    igmp -> igmp_type = type;
    igmp -> igmp_code = code;
    
#if 0  // Big endian
    igmp -> igmp_type = htons(type);
    igmp -> igmp_code = htons(code);
#endif
 
}
  
void configure_TCP(struct tcphdr *tcp, unsigned short int source, \
                   unsigned short int dest) {
    
    tcp -> source = source;
    tcp -> dest = dest;
    
#if 0  // Big endian
    tcp -> source = htons(source);
    tcp -> dest = htons(dest);
#endif

}


void configure_UDP(struct udphdr *udp, unsigned short int source, \
                   unsigned short int dest) {
                             
    udp -> source = source;
    udp -> dest = dest;
    
#if 0  // Big endian
    udp -> source = htons(source);
    udp -> dest = htons(dest);
#endif

}
