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
	ax.plot(generations, columns[0], color = columnColors[0], label='Największe dopasowanie')
	ax.plot(generations, columns[1], color = columnColors[1], label='Średnie dopasowanie')
	ax.bar(generations, columns[2], color = columnColors[2], label='Liczba zwycięzców')
	ax.plot(generations, columns[3], color = columnColors[3], label='Najkrótszy czas przejazdu')
	ax.plot(generations, columns[4], color = columnColors[4], label='Średni czas życia pojazdu')
	plt.legend(loc="best")
	plt.xlabel('Generacja')
	plt.savefig(file[:-4] + '.png')
	#plt.show()