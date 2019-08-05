import plotly.graph_objects as go
import plotly
import numpy as np
from sys import argv

def main():
	# Check to see if script was called correctly
	if len(argv) < 2:
		print("Use as $ " + argv[0] + " bits [Bit Rates] [Rate Accuracy]")
		print("OR")
		print("Use as $ " + argv[0] + " packets [Packet Rates] [Rate Accuracy]")
		print("OR")
		print("Use as $ " + argv[0] + " flows [# Flows] [Rate Accuracy] [Bit Rate]")
		print("\nFor more information on a specific branch, use $ " + argv[0] + " [Branch]")
		print("Potential branches: 'bits' // 'packets' // 'flows'")
		exit()

	if (argv[1] == "bits"):
		if len(argv) != 4:
			print("Use as $ " + argv[0] + " bits [Bit Rates] [Rate Accuracy]")
			print("[Bit Rates] ~ File containing the bit rates evaluated over a network interface.")
			print("[Rate Accuracy] ~ File containing the accuracy of the recorded bit rates wrt the expected rate.")
			print("Note: All files containing entries must be separated by new lines.")
			exit()
		graph_bit_rate(argv[2], argv[3])

	elif (argv[1] == "packets"):
		if len(argv) != 4:
			print("Use as $ " + argv[0] + " packets [Packet Rates] [Rate Accuracy]")
			print("[Packet Rates] ~ File containing packet rates evaluated over a network interface.")
			print("[Rate Accuracy] ~ File containing the accuracy of the recorded packet rates wrt the expected rate.")
			print("Note: All files containing entries must be separated by new lines.")
			exit()
		graph_packet_rate(argv[2], argv[3])

	elif (argv[1] == "flows"):
		if len(argv) != 5:
			print("Use as $ " + argv[0] + " flows [Bit Rate] [# Flows] [Rate Accuracy]")
			print("[Bit Rate] ~ A single number that the list of differnt number of flows tried to reach.")
			print("[# Flows] ~ File containing the number of flows (e.g. 1\\n2\\n3\\n => 1, 2, and 3 flows tried to reach a bit rate")
			print("[Rate Accuracy] ~ File containing the accuracy of the alternate # of flows to reach the bit rate.")
			print("Note: All files containing entries must be seperated by new lines.")
		graph_flows(argv[2], argv[3], argv[4])

	else:
		print("Branch '" + argv[1] + "' is not known.")
		print("Potential branches: 'bits' // 'packets' // 'flows'")

def graph_bit_rate(bit_rates_f, actual_rates_f):
	b_list = []
	br_list = []

	with open(bit_rates_f) as f:
		for line in f:
			b_list.append(float(line))

	with open(actual_rates_f) as f:
		for line in f:
			br_list.append(float(line))

	fig = go.Figure()

	fig.add_trace(go.Scatter(
		x = b_list,
		y = br_list,
		mode = "lines+markers",
		name = "Flow 1"
	))

	fig.update_layout(
		title = go.layout.Title(
			text = "Accuracy of Increasing Bit Transfer Rates"
		),
		xaxis = go.layout.XAxis(
			title = "Single Flow",
		),
		yaxis = go.layout.YAxis(
			title = "% Accuracy",
            autorange = False,
            range = [0, 101]
		),
		showlegend=True
	)

	fig.show()
	fig.write_image("bit_rates.jpeg")


def graph_packet_rate(packet_rates_f, actual_rates_f):
	p_list = []
	pr_list = []

	with open(packet_rates_f) as f:
		for line in f:
			p_list.append(float(line))

	with open(actual_rates_f) as f:
		for line in f:
			pr_list.append(float(line))

	fig = go.Figure()

	fig.add_trace(go.Scatter(
		x = p_list,
		y = pr_list,
		mode = "lines+markers",
		name = "Single Flow"
	))

	fig.update_layout(
		title = go.layout.Title(
			text = "Accuracy of Increasing Packet Transfer Rates"
		),
		xaxis = go.layout.XAxis(
			title = "Packet Rate",
		),
		yaxis = go.layout.YAxis(
			title = "% Accuracy",
            autorange = False,
            range = [0, 101]
		),
		showlegend=True
	)

	fig.show()
	fig.write_image("packet_rates.jpeg")


def graph_flows(bit_rate, flows_f, accuarcy_f):
	flows_list = []
	acc_list = []

	with open(flows_f) as f:
		for line in f:
			flows_list.append(float(line))

	with open(accuarcy_f) as f:
		for line in f:
			acc_list.append(float(line))

	fig = go.Figure()

	fig.add_trace(go.Scatter(
		x = flows_list,
		y = acc_list,
		mode = "lines+markers",
		name = "Varying Flow Accuracy"
	))

	fig.update_layout(
		title = go.layout.Title(
			text = str(float(float(bit_rate)/1000000000.0)) + "Gb/s Reached by " + str(float(float(bit_rate)/1000000000.0)) + "Gb/#Flows"
		),
		xaxis = go.layout.XAxis(
			title = "Number of Flows",
		),
		yaxis = go.layout.YAxis(
			title = "% Accuracy",
			autorange = False,
			range = [0, 101]
		),
		showlegend=True
	)

	fig.show()
	fig.write_image("flow_rates.jpeg")



if __name__ == "__main__":
	main()
