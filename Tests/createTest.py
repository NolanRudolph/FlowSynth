"""
This Python module designs CSV test cases to be used with the project packetizer

Potential future implementations:
1. TOS Value (currently defaulted as 0)
2. TCP Flags (currently defaulted as 0)
"""

from sys import argv


def main():
    print("")

    if len(argv) != 9:
        print("Use as: $ python " + argv[0] + " [Duration(s)] [Src IP] [Dst IP] [Src Port] [Dst Port] [Proto] "
                                                                                + "[Packet Rate] [Bit Rate]\n")
        exit()

    """
    argv[1]: Duration
    argv[2]: Source IP
    argv[3]: Destination IP
    argv[4]: Source Port
    argv[5]: Destination Port
    argv[6]: IP Protocol
    argv[7]: Packet Rate
    argv[8]: Bit Rate
    argv[9]: Maximum Transmission Unit
    """
    MAX_MTU = 1500

    duration = int(argv[1])
    src_ip = argv[2]
    dst_ip = argv[3]
    src_port = int(argv[4])
    dst_port = int(argv[5])
    proto = int(argv[6])
    p_rate = int(argv[7])
    b_rate = int(argv[8])

    bytes = calc_bytes(b_rate * duration)
    total_packets = p_rate * duration

    mtu_test(bytes, total_packets, MAX_MTU)

    print("Your test CSV line is:")
    print("0.0,{},{},{},{},{},{},0,0,{},{},0,0,0,0,".format(float(duration), src_ip, dst_ip, src_port, dst_port, proto, total_packets, bytes))

    print("")

    return 0


def calc_bytes(bit_rate):
    if bit_rate % 8 != 0:
        print("Warning: You'll be losing " + str(bit_rate % 8) + " bits with this [Bit Rate].   (bit_rate % 8 != 0)\n")

    return int(bit_rate / 8)


def mtu_test(total_bytes, total_packets, max_mtu):
    # MTU Check
    if total_bytes / total_packets > max_mtu:
        print("Bytes per packet exceeds the standard MTU of 1500.")
        print("Your Bytes/Packet: {}".format(total_bytes / total_packets))
        print("Try reducing the [Bit Rate], increasing the [Packet Rate], or increasing the [MTU].\n")
        exit()


if __name__ == "__main__":
    main()
