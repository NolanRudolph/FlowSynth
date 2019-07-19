import sys


def main():

	try:
		f_in = open(sys.argv[1], "r")
	except IndexError:
		print("Please use as $ python " + sys.argv[0] + " [FILE]")
		return None

	lines_since_title = 0

	for line in f_in:
		entry = line.split(",")
		entry[2] = entry[2][:18]
		entry[3] = entry[3][:18]

		"""
		Number of spaces per column
		
		start_time_space = 15
		end_time_space = 15
		src_IP_space = 15
		dst_IP_space = 15
		src_port_space = 9
		dst_port_space = 9
		IP_prot_space = 5
		TOS_val_space = 4
		TCP_flags_space = 10
		num_packets_space = 10
		num_bytes_space = 12
		router_in_space = 8
		router_out_space = 9
		src_ASN_space = 8
		dst_SAN_space = 8
		
		Represented by list below
		"""

		spaces = [15, 15, 19, 19, 9, 9, 5, 4, 6, 10, 12, 8, 9, 8, 8]
		new_str = "| "

		for i in range(15):
			attr = entry[i]
			length = len(attr)

			new_str += attr
			new_str += " " * (spaces[i] - length)
			new_str += "| "

		if lines_since_title % 30 == 0:
			make_titles()
			lines_since_titles = 0
		print(new_str)
		lines_since_title += 1

	return None


def make_titles():
	print("+----------------+----------------+--------------------+--------------------+----------+----------+------+-----+-------+-----------+-------------+---------+----------+---------+---------+")
	print("|   Start Time   |    End Time    |      Source IP     |   Destination IP   | Src Port | Dst Port | Prot | TOS | Flags | # Packets |   # Bytes   | Rout In | Rout Out | Src ASN | Dst ASN |")
	print("+----------------+----------------+--------------------+--------------------+----------+----------+------+-----+-------+-----------+-------------+---------+----------+---------+---------+")
	return None


if __name__ == "__main__":
	main()

