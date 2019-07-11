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
        short int tos, char *src, char *dst, unsigned char proto) {

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


void configure_ICMP(struct icmphdr *icmp, unsigned short int type, \
                    unsigned short int code) {

    icmp -> type = type;
    icmp -> code = code;

    icmp -> un.echo.id = 0;
    icmp -> un.echo.sequence = 0;


}


void configure_IGMP(struct igmp *igmp, unsigned short int type, \
                    unsigned short int code, char *addr) {

    igmp -> igmp_type = type;
    igmp -> igmp_code = code;
    igmp -> igmp_group.s_addr = inet_addr(addr);
 
}
  
void configure_TCP(struct tcphdr *tcp, unsigned short int source, \
                   unsigned short int dest) {
    
    tcp -> source = htons(source);
    tcp -> dest = htons(dest);
    tcp -> seq = 0;
    tcp -> doff = 5;
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

}

void addPayload(uint8_t *ptr, int size) {
    
    int i;
    for (i = 0; i < size; ++i) {
        *ptr = 0x01;
        ptr++;
    }
    
}

// Calculates IP/TCP/UDP checksums (given length in 2 byte units)
// Credit goes to Chris Misa for the algorithm
uint16_t calcCheck(uint8_t *data, int len) {
    uint32_t sum = 0;
    while (len > 1) {
        sum += (data[0] << 8) + data[1];
        if (sum & 0x80000000) {
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

    return htons(~sum);
}
