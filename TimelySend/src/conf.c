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
    tcp -> check = 0;
    tcp -> window = 0;
    
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

int calcCheckSum(void *p) {
    int i;
    long int sum = 0;
    int curVal;
    int *ptr = (int *)p;
    printf("First byte is at %#010x\n", ptr);

    // Using null pointer to add all bytes (paired in two using ints)
    for (i = 0; i < 10; ++i) {
        // Ignore checksum byte
        if (i != 5) {
            sum += *ptr;
        }
        ptr++;
    }

    // Calculating length (e.g. 0x532F3 is 5 and 0x32F3 is 4)
    long int temp = sum;
    int len = 0;
    
    while (temp & 0xFFFFF) {
        temp >>= 4;
        len += 1;
    }

    // Calculating Carry (e.g. The carry of 0x532F3 is 5)
    long int carry = (sum & 0xF0000) >> 4 * (len - 1);
    sum &= 0xFFFF;

    // Adding Carry back to Sum
    sum += carry;

    printf("Resulting checksum is %#010x\n", sum);
    
    return sum;
}
