import matplotlib.pyplot as plt
import pickle

# xAxis = ["FCFS","SJF","SRTF","NP-PR","P-PR","RR","MLQ","MLFQ"]

xAxis = ["SJF","SRTF","FCFS","Round Robin","Multi level queue","Multi level Feedback q","Priority-Non-Preemptive","Priority-Preemptive"]

'''
MLQ - RR - 2nd queue TQ2
MLFQ - RR - 1st queue 2 2nd queue 4
'''



file = open("./allout.txt", "r")


avgWT = []
avgTAT = []
avgRT = []
avgCT = []

c=0

li= [ 6 , 8 , 9 , 11 ]

while(True):
    c+=1
    line = file.readline()
    if not line:
        break
    if c in li:
        continue
    data = line.split(',')
    #data[3]=data[3][:len(data[3])-1]
    avgTAT.append(data[0])
    avgCT.append(data[1])
    avgRT.append(data[2])
    avgWT.append(data[3])
for i in range(len(avgWT)-1):
    avgWT[i]=avgWT[i][:len(avgWT[i])-1]
for i in range(len(avgWT)):
    avgWT[i]=float(avgWT[i])
    avgRT[i]=float(avgRT[i])
    avgCT[i]=float(avgCT[i])
    avgTAT[i]=float(avgTAT[i])

fig,ax = plt.subplots()

plt.bar(xAxis,avgWT)
plt.title('average weighting time')
plt.text(4,avgWT[4]//2,"2nd queue-RR-TQ-2",ha="center")
plt.text(5,avgWT[5],"1st queue-RR-TQ-2",ha="center")
plt.text(5,avgWT[5]//2,"2nd queue-RR-TQ-4",ha="center")
plt.savefig("avg turn weighting time.png")
plt.clf()


plt.bar(xAxis,avgTAT)
plt.title('average turn around time')
plt.text(4,avgTAT[4]//2,"2nd queue-RR-TQ-2",ha="center")
plt.text(5,avgTAT[5],"1st queue-RR-TQ-2",ha="center")
plt.text(5,avgTAT[5]//2,"2nd queue-RR-TQ-4",ha="center")
plt.savefig("avg turn around time.png")
plt.clf()

plt.bar(xAxis,avgCT)
plt.title('average completion time')
plt.text(4,avgCT[4]//2,"2nd queue-RR-TQ-2",ha="center")
plt.text(5,avgCT[5],"1st queue-RR-TQ-2",ha="center")
plt.text(5,avgCT[5]//2,"2nd queue-RR-TQ-4",ha="center")
plt.savefig("avg completion time.png")
plt.clf()

plt.bar(xAxis,avgRT)
plt.title('average response time')
plt.text(4,avgRT[4]//2,"2nd queue-RR-TQ-2",ha="center")
plt.text(5,avgRT[5],"1st queue-RR-TQ-2",ha="center")
plt.text(5,avgRT[5]//2,"2nd queue-RR-TQ-4",ha="center")
plt.savefig('average response time.png')
plt.clf()