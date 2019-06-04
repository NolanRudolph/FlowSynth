#include "conf.h"

void configure_ETHER(struct ether_header *ether, char *dst, char *src) {
    int i;
    for (i = 0; dst[i] != '\0'; ++i) {
        ether -> ether_dhost[i] = dst[i];
    }
    ether -> ether_dhost[i + 1] = '\0';
    
    for (i = 0; src[i] != '\0'; ++i) {
        ether -> ether_shost[i] = src[i];
    }
    ether -> ether_shost[i + 1] = '\0';
    
}

void configure_IP(struct ip *ip, int version, int tos, char *src, \
				  char *dst, int proto) {

    ip -> ip_v = version;
    ip -> ip_hl = 5;
    ip -> ip_tos = tos;
    ip -> ip_p = proto;
    ip -> ip_ttl = 255;

    // Temporary Testing on Localhost
    ip -> ip_src.s_addr = inet_addr(src);
    ip -> ip_dst.s_addr = inet_addr(dst);
}


void configure_ICMP(struct icmp *icmp, int type, int code) {

	icmp -> icmp_type = type;
    icmp -> icmp_code = code;
    // Add icmp_cksum
}


void configure_IGMP(struct igmp *igmp, int type, int code) {

    igmp -> igmp_type = type;
    igmp -> igmp_code = code;
    // Add igmp_cksum, maybe in_addr.igmp_group
 
}
  
void configure_TCP(struct tcphdr *tcp, int source, int dest) {
    tcp -> source = source;
    tcp -> dest = dest;
    // Skipping sequence number and ACK number

}


void configure_UDP(struct udphdr *udp, int source, int dest) {
                                             	
    udp -> source = source;
    udp -> dest = dest;
    // Add uh_ulen & uh_sum

}
