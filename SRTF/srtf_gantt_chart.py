import matplotlib.pyplot as plt
import csv, operator
import os

data = csv.reader(open('srtf_gantt.csv'),delimiter=',')
data = sorted(data, key=operator.itemgetter(0))
max_x_value = 0
max_y_value = 0
fig, gnt = plt.subplots()
li = []

for row in data:
    max_y_value = max_y_value+1
    if int(row[2]) == int(1):
        li.append(row)
        if max_x_value < int(row[4]):
            max_x_value = int(row[4])
    else:
        li.append(row)
        if max_x_value < int(row[4]):
            max_x_value = int(row[4])
# Setting Y-axis limits
gnt.set_ylim(0, max_y_value*4)
# Setting X-axis limits
gnt.set_xlim(0, max_x_value+3)
gnt.set_xlabel('Seconds since start')
gnt.set_ylabel('Process')
a = max_y_value*3+10
# print(li)
name = []
heights = []
visited = []
a = a - 5
for x in li:
    visited.append(0)
for i in range(len(li)):
    # a = a-3
    temp_list = []
    j = i
    if visited[i] == 0:
        a = a-5
        for _ in range(len(li)):
            if li[i][0] == li[j][0] and visited[j] == 0:
                # process
                lists_ = []
                lists_.append(int(li[j][3]))
                lists_.append(int(li[j][4])-int(li[j][3]))
                temp_list.append(tuple(lists_))
                visited[j] = 1
            j = j+1
            if j >= len(li):
                break
        # if visited[i] == 0:
        if int(li[i][2]) == int(0):
            heights.append(a)
            name.append(li[i][1])
            gnt.broken_barh(temp_list, (a, 5), facecolors=('tab:blue'))
        else:
            heights.append(a)
            name.append(li[i][1])
            gnt.broken_barh(temp_list, (a, 5), facecolors=('tab:red'))
        visited[i] = 1
gnt.set_yticks(heights)
gnt.set_yticklabels(name)
gnt.grid(True)
plt.title("Gantt Chart of SRTF")
plt.savefig("srtf_gantt.png")
