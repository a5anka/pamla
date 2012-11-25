#!/usr/bin/env python2

import csv
import pylab

names = {}
X = {}
y = {}

with open("data_csv", "r") as datafile:
    reader = csv.reader(datafile)
    for row in reader:
        name = row[0]

        if not names.has_key(name):
            names[name] = []
            X[name] = []
            y[name] = []

        names[name].append(row[1])
        X[name].append(row[2])
        y[name].append(row[4])

plot_number = 0
for name in names:
    pylab.figure(plot_number)
    plot_number += 1
    pylab.title(name)
    pylab.xlabel("Array Size(N)")
    pylab.ylabel("Time Elapsed (msec)")
    pylab.scatter(X[name], y[name])
    pylab.xscale('log')
    pylab.yscale('log')

    for label, x_val, y_val in zip(names[name], X[name], y[name]):
        pylab.annotate(label, xy=(x_val, y_val))

pylab.show()
