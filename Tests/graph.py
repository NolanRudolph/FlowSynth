import plotly.graph_objects as go
import numpy as np
import os

x_list = [5*x for x in range(0, 21)]
y_list = [0, 1, 2, 4, 8, 16, 32, 40, 50, 64, 70, 84, 88, 92, 94, 96, 97, 98, 99, 100]

fig = go.Figure()

fig.add_trace(go.Scatter(
	x = x_list, 
	y = y_list,
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
fig.write_image("fig1.png")
fig.write_image("fig1.jpeg")

