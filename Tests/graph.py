## Dependencies ##
# 1. Plotly
# 2. Numpy
# 3. Orca (see https://github.com/plotly/orca#installation & use method 1)
#   - Install conda via wget (I use anaconda2, where conda is found in bin)
#   - Include /path/to/anaconda2/bin in $PATH variable (/etc/environment on Ubuntu)
#   - $ conda install -c anaconda libgcc-ng
#   - $ conda install -c conda-forge python-libarchive-c
#   - $ conda install -c plotly plotly-orca

import plotly.graph_objects as go
import plotly
import numpy as np
from sys import argv

def main():
	# Check to see if script was called correctly
	if len(argv) < 6:
		print("Use as $ python " + argv[0] + " [Title] [X Label] [Y Label] [# Threads/CPUs] [PPS]")
                print("[Title]: Preferred title for your graph.")
                print("[X Label]: Preferred title for x axis of the graph.")
                print("[Y Label]: Preferred title for y axis of the graph.")
                print("[# Threads/CPUs]: Text file with # of threads for each sample in PPS, seperated by new lines.")
                print("[PPS]: Text file with Packet Per Second results from each # threads sample in [# Threads], seperated by new lines.")
		exit()
        else:
            graph(argv[1], argv[2], argv[3], argv[4], argv[5])

        return 0
        

def graph(title, x_title, y_title, num_threads, pps):
	threads_list = []
        pps_list = []


	with open(num_threads) as f:
		for line in f:
			threads_list.append(int(line))

	with open(pps) as f:
		for line in f:
			pps_list.append(int(line))

	fig = go.Figure()

	fig.add_trace(go.Scatter(
		x = threads_list,
		y = pps_list,
		mode = "lines+markers",
		name = "Respective PPS"
	))

	fig.update_layout(
		title = go.layout.Title(
			text = title
		),
		xaxis = go.layout.XAxis(
			title = x_title,
		),
		yaxis = go.layout.YAxis(
			title = y_title,
            autorange = False,
            range = [0, max(pps_list) + 100000]
		),
		showlegend=True
	)

	fig.show()
	fig.write_image("graph.jpeg")


if __name__ == "__main__":
    main()
