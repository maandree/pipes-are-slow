#!/usr/bin/env python3

import sys, math
import matplotlib.pyplot as plot

#plot.xkcd()
fig = plot.figure(figsize = (3 * 2, 4 * 2))
data = sys.stdin.read().split('\n')

graphs = {}
points = set()
values = set()

for line in data:
    if line == '':
        continue
    while '  ' in line:
        line = line.replace('  ', ' ')
    [label, point, time] = line.split(' ')
    [_, point] = point.split('<<')
    point = int(point)
    time = float(time) / 10
    if label not in graphs:
        graphs[label] = {}
    graphs[label][point] = time
    points.add(point)
    values.add(time)

points = sorted(list(points))
values = sorted(list(values))

for label in sorted(graphs.keys()):
    graph_values = list(map(lambda x : x[1], sorted(graphs[label].items(), key = lambda x : x[0])))
    plot.plot(points, graph_values, label = label)

plot.xticks(points, map(str, points))
fig.suptitle('pipe(7) versus unix(7)')
plot.ylabel('millisecond')
plot.xlabel('log₂(n)')
plot.grid(axis = 'x', linestyle = ' ')
plot.legend(loc = 'upper left')
plot.xlim((min(points), max(points)))
plot.ylim((min(values) * 1.05, max(values) * 1.05))
plot.grid(True)
plot.savefig('graph.svg', transparent = False, format = 'svg')
