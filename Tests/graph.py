import plotly.graph_objects as go
import numpy as np
from sys import argv

def main():
	# Check to see if script was called correctly
	if len(argv) != 5:
		print("Use as $ " + argv[0] + " [Packet Rates] [P Accuracy] [Bit Rates] [B Accuracy]")
		print("[Packet Rates] ~ The packet rates evaluated over a network interface.")
		print("[P Accuracy] ~ Accuracy of the recorded packet rates wrt the expected rate.")
		print("[Bit Rates] ~ The bit rates evaluated over a network interface.")
		print("[B Accuracy] ~ Accuracy of the recorded bit rates wrt the expected rate.")
		print("All values should be seperated by new lines.")
		exit()

	# Gather files from command line
	p_list = []
	pr_list = []
	b_list = []
	br_list = []

	with open(argv[1]) as f:
		for line in f:
			p_list.append(float(line))

	with open(argv[2]) as f:
		for line in f:
			pr_list.append(float(line))

	with open(argv[3]) as f:
		for line in f:
			b_list.append(float(line))

	with open(argv[4]) as f:
		for line in f:
			br_list.append(float(line))

	x_list = [5*x for x in range(0, 21)]
	y_list = [0, 1, 2, 4, 8, 16, 32, 40, 50, 64, 70, 84, 88, 92, 94, 96, 97, 98, 99, 100]

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
			title = "Bit Rate",
		),
		yaxis = go.layout.YAxis(
			title = "% Accuracy"
		),
		showlegend=True
	)			

	fig.show()
	fig.write_image("fig1.jpeg")


if __name__ == "__main__":
	main()
