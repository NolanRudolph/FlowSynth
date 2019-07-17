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
	lines = lines[2:-4]

	pack_rate_av = 0.0
	bit_rate_av = 0.0
	for line in lines:
		line = line.split(" ")
		print("Packet rate is " + line[4])
		print("Bit rate is " + line[2])
		pack_rate_av += float(line[4])
		bit_rate_av += float(line[2])

	# Post recording calculations
	duration -= (duration - len(lines))  # Compensates for lines[2:-4]
	pack_rate_av /= duration
	bit_rate_av /= duration
	
	print("Expected Packet Rate: {} | Actual Packet Rate: {}"\
		  .format(packet_rate, str(pack_rate_av)))
	print("Difference: {} | Accuracy: {}%"\
		  .format(packet_rate - pack_rate_av, (pack_rate_av / packet_rate) * 100))
	print("Expected Bit Rate: {} | Actual Bit Rate: {}"\
		  .format(bit_rate, bit_rate_av))
	print("Difference: {} | Accuracy: {}%"\
		  .format(bit_rate - bit_rate_av, (bit_rate_av / bit_rate) * 100))

if __name__ == "__main__":
	main()
