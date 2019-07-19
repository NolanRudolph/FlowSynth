#!/bin/python
import subprocess
from sys import argv

def main():
	if len(argv) != 5:
		print("Please use as $ " + argv[0] + " [Duration] [Packet Rate] [Bit Rate] [Interface]")
		exit()

	duration = float(argv[1]) # -3 because we neglect first and last two recordings
	packet_rate = int(argv[2])
	bit_rate = int(argv[3])
	interface = argv[4]

	lines = subprocess.check_output(["./get_net_usage/get_net_usage", interface, \
									 "-t", "1", "-X", "2"])
	lines = lines.strip().split("\n")
	
	"""
	Explaining lines[2:-4]
	- lines[0] is a notification
	- lines[-1] is a notification
	- lines[1] is sometimes inaccurate being the first entry
	- lines[-2] is sometimes inaccurate being the last entry
	- lines[-3] & lines[-4] are used to track zero traffic to stop the program
	"""
	lines = lines[3:8]

	pack_rate_av = 0.0
	bit_rate_av = 0.0
	for line in lines:
		line = line.split(" ")
		print("Got packet rate of %s" % str(line[4]))
		print("Got bit rate of %s" % str(line[2]))
		pack_rate_av += float(line[4])
		bit_rate_av += float(line[2])

	# Adjust Duration
	print("subtracting %s from duration" % str(duration - len(lines)))
	duration -= (duration - len(lines))
	print("Duration is now %s" % str(duration))

	# Removing Ethernet
	total_ether = packet_rate * duration * 14 * 8
	bit_rate_av -= total_ether

	# Post recording calculations
	print("Bit rate total is %s", str(bit_rate_av))
	print("Pack rate total is %s", str(pack_rate_av))
	pack_rate_av /= duration
	bit_rate_av /= duration
	
	print("Expected Packet Rate: {} | Actual Packet Rate: {}"\
		  .format(packet_rate, str(pack_rate_av)))
	print("Difference: {} | Accuracy: {}%".format\
		(round(packet_rate - pack_rate_av, 2), round((pack_rate_av / packet_rate) * 100, 2)))
	print("Expected Bit Rate: {} | Actual Bit Rate: {}"\
		  .format(bit_rate, bit_rate_av))
	print("Difference: {} | Accuracy: {}%".format\
		(round(bit_rate - bit_rate_av, 2), round((bit_rate_av / bit_rate) * 100, 2)))

if __name__ == "__main__":
	main()
