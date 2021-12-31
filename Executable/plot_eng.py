import matplotlib.pyplot as plt
import matplotlib.ticker as mtick
import numpy as np
import glob, os
import csv

columnColors = ['cornflowerblue', 'lightsteelblue', 'lightcoral', 'darkolivegreen', 'yellowgreen']

os.chdir(".")
for file in glob.glob("*.csv"):
	columns = [[], [], [], [], []]
	with open(file,'r') as csvfile:
		plots = csv.reader(csvfile, delimiter = ';')

		for row in plots:
			if not row:
				break
			numberOfColumns = 5
			for i in range(numberOfColumns):
				columns[i].append(float(row[i]))

	figure = plt.figure()
	ax = figure.add_subplot(1, 1, 1)
	ax.yaxis.set_major_formatter(mtick.PercentFormatter())
	ax.xaxis.set_major_formatter(mtick.ScalarFormatter())
	numberOfGenerations = len(columns[i])
	generations = np.arange(numberOfGenerations)
	ax.plot(generations, columns[0], color = columnColors[0], label='Highest fitness')
	ax.plot(generations, columns[1], color = columnColors[1], label='Mean fitness')
	ax.bar(generations, columns[2], color = columnColors[2], label='Number of winners')
	ax.plot(generations, columns[3], color = columnColors[3], label='Best time overall')
	ax.plot(generations, columns[4], color = columnColors[4], label='Mean time overall')
	plt.legend(loc="best")
	plt.xlabel('Generation')
	plt.savefig(file[:-4] + '.png')
	#plt.show()