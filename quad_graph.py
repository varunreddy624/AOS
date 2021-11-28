import matplotlib.pyplot as plt
import pickle

xAxis = ["SJF", "SRTF", "FCFS", "RR", "MLQ", "MLFQ", "Pr-NPr", "Pr-Pr"]

'''
MLQ - RR - 2nd queue TQ2
MLFQ - RR - 1st queue 2 2nd queue 4
'''
file = open("./allout.txt", "r")

avgWT = []
avgTAT = []
avgRT = []
avgCT = []

c = 0

li = [6, 8, 9, 11]

while(True):
    c += 1
    line = file.readline()
    if not line:
        break
    if c in li:
        continue
    data = line.split(',')
    # data[3]=data[3][:len(data[3])-1]
    avgTAT.append(data[0])
    avgCT.append(data[1])
    avgRT.append(data[2])
    avgWT.append(data[3])
for i in range(len(avgWT)-1):
    avgWT[i] = avgWT[i][:len(avgWT[i])-1]
for i in range(len(avgWT)):
    avgWT[i] = float(avgWT[i])
    avgRT[i] = float(avgRT[i])
    avgCT[i] = float(avgCT[i])
    avgTAT[i] = float(avgTAT[i])

fig, ax = plt.subplots()

plt.bar(xAxis, avgWT, color='maroon')
#plt.bar(xAxis, avgWT)
plt.title('Average Weighting Time')
plt.xlabel('Processes')
plt.ylabel('Time')
plt.savefig("avg turn weighting time.png")
plt.clf()


plt.bar(xAxis, avgTAT, color='maroon')
#plt.bar(xAxis, avgTAT)
plt.title('Average Turn Around Time')
plt.xlabel('Processes')
plt.ylabel('Time')
plt.savefig("avg turn around time.png")
plt.clf()

plt.bar(xAxis, avgCT, color='maroon')
#plt.bar(xAxis, avgCT)
plt.title('Average Completion time')
plt.xlabel('Processes')
plt.ylabel('Time')
plt.savefig("avg completion time.png")
plt.clf()

plt.bar(xAxis, avgRT, color='maroon')
#plt.bar(xAxis, avgRT)
plt.title('Average Response time')
plt.xlabel('Processes')
plt.ylabel('Time')
plt.savefig('average response time.png')
plt.clf()
