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
	print("Use as $ python " + argv[0] + " [Title] [X Label] [Y Label] [# of Files] [PPSs]")
        print("[Title]: Preferred title for your graph.")
        print("[X Label]: Preferred title for x axis of the graph.")
        print("[Y Label]: Preferred title for y axis of the graph.")
        print("[# of Files]: Number of files to be graphed next to eachother.")
        print("[PPSs]: Text files with Packet Per Second results from each # threads sample in [# Threads], seperated by new lines.")
	exit()
    else:
        num_pps_files = int(argv[4])
        pps_files = []
        for i in range(5, 5 + num_pps_files):
            pps_files.append(argv[i])
        
        graph(argv[1], argv[2], argv[3], pps_files)
    
    return 0


def graph(title, x_title, y_title, pps_files):
    pps_lists = []
    cpu_list = [i for i in range(1, 20)]

    for item in pps_files:
        new_list = []
        with open(item) as f:
            for line in f:
                new_list.append(int(line))
        pps_lists.append(new_list)

    fig = go.Figure()


    fig.add_trace(go.Scatter(
        x = cpu_list,
        y = pps_lists[0],
        mode = "lines+markers",
        name = "MoonGen (DPDK User Space)"
    ))

    fig.add_trace(go.Scatter(
        x = cpu_list,
        y = pps_lists[1],
        mode = "lines+markers",
        name = "Pktgen (Netmap User Space)"
    ))

    fig.add_trace(go.Scatter(
        x = cpu_list,
	y = pps_lists[2],
	mode = "lines+markers",
	name = "TCPReplay (Kernel Space)"
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
            range = [0, max(pps_lists[0]) + 1000000]
	),
        showlegend=True
    )

    fig.show()
    fig.write_image("graph.jpeg")


if __name__ == "__main__":
    main()
