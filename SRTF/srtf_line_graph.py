import matplotlib.pyplot as plt
import csv, operator
import os
data = csv.reader(open('srtf_line.csv'),delimiter=',')
data = sorted(data, key=operator.itemgetter(0))
x = []
minTat = []
minct = []
minwt = []
minrt = []
x_coordinate=0

for row in data:
    x.append(row[1])
    minrt.append(float(row[3]))
    minwt.append(float(row[4]))
    minct.append(float(row[5]))
    minTat.append(float(row[6]))


plt.plot(x, minct,color='r',label="Completion Time")
plt.plot(x, minwt,color='g',label="Waiting Time")
plt.plot(x, minrt,color='b',label="Burst Time")
plt.plot(x, minTat,color='y',label="TurnAround Time")
plt.legend()
plt.title("Shortest Remaining Time First")
plt.savefig("srtf_line.png")
os.remove('srtf_line.csv')
