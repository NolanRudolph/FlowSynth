import sys


def main():

	try:
		f_in = open(sys.argv[1], "r")
	except IndexError:
		print("Please use as $ python " + sys.argv[0] + " [FILE]")
		return None

	"""
	0.  Start Time
	1.  End Time
	2.  Source IP
	3.  Destination IP
	4.  Source Port
	5.  Destination Port
	6.  IP Protocol
	7.  Type of Service
	8.  TCP Flags
	9.  Number of Packets
	10. Number of Bytes
	11. Router Ingress Port
	12. Router Egress Port
	13. Source Autonomous System
	14. Destination Autonomous System
	"""

	smallest_TCP = float("inf")
	smallest_UDP = float("inf")

	for line in f_in:
		entry = line.split(",")
		
		proto = int(entry[6])
		
		if proto == 6:
			total_bytes = int(entry[10])
			total_packets = int(entry[9])

			bytes_per = total_bytes / total_packets
			if bytes_per < smallest_TCP:
				print("Found new smallest TCP: %s bytes/packet." % bytes_per)
				smallest_TCP = bytes_per
		
		elif proto == 17:
			total_bytes = int(entry[10])
			total_packets = int(entry[9])

			bytes_per = total_bytes / total_packets
			if bytes_per < smallest_UDP:
				print("Found new smallest UDP: %s bytes/packet." % bytes_per)
				smallest_UDP = bytes_per 
				

	print("Finished dataset.")
	print("Smallest TCP bytes/packet is %s." % smallest_TCP)
	print("Smallest UDP bytes/packet is %s." % smallest_UDP)

	return 0


if __name__ == "__main__":
	main()

