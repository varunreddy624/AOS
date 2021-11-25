import matplotlib.pyplot as plt
import csv
x = []
minTat = []
minct = []
minwt = []
minrt = []
x_coordinate=0

with open('srtf_line.csv', 'r') as csvfile:
    lines = csv.reader(csvfile, delimiter=',')
    for row in lines:
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
plt.savefig("srtf_line.jpg")