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
    ip -> ip_id = 0;
    ip -> ip_off = 0;
    ip -> ip_tos = tos;
    ip -> ip_p = proto;
    ip -> ip_ttl = 255;
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
    tcp -> seq = 0;
    tcp -> ack_seq = 0;
    tcp -> res1 = 0;
    tcp -> window = htons(8192);
    
    // Flags
    tcp -> fin = 0;
    tcp -> syn = 0;
    tcp -> rst = 0;
    tcp -> psh = 0;
    tcp -> ack = 0;
    tcp -> urg = 0;
    
    tcp -> urg_ptr = 0;

}

void configure_UDP(struct udphdr *udp, unsigned short int source, \
                   unsigned short int dest) {
                            
    udp -> source = htons(source);
    udp -> dest = htons(dest);
    udp -> len = htons(8); // Change me once constructing real packets

}

// Calculates IP/TCP/UDP checksums (given length in 2 byte units)
// Credit goes to Chris Misa for the algorithm
uint16_t calcCheck(uint8_t *data, int len) {
    uint32_t sum = 0;
    while (len > 1) {
        printf("Adding %#010x\n and %#010x\n together.\n", data[0] << 8, data[1]);
        sum += (data[0] << 8) + data[1];
        if (sum & 0x80000000) {
            printf("Overflow, sum: %#010x will go to %#010x\n", sum, \
                                        (sum & 0xFFFF) + (sum >> 16));
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        len -= 2;
        data += 2;
    }

    if (len) {
        sum += (uint32_t)(data[0]);
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    printf("Resulting checksum is %#010x\n", ~sum & 0xFFFF);

    return htons(~sum);
}
