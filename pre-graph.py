import matplotlib.pyplot as plt
import csv
x = []
minTat = []
minct = []
minwt = []
minrt = []
x_coordinate = 0
with open('pri_pre_tat1.csv', 'r') as csvfile:
    lines = csv.reader(csvfile, delimiter=',')
    for row in lines:
        x.append(x_coordinate)
        x_coordinate += 1
        minTat.append(float(row[0]))
with open('pri_pre_rt1.csv', 'r') as csvfile:
    lines = csv.reader(csvfile, delimiter=',')
    for row in lines:
        minrt.append(float(row[0]))
with open('pri_pre_wt1.csv', 'r') as csvfile:
    lines = csv.reader(csvfile, delimiter=',')
    for row in lines:
        minwt.append(float(row[0]))
with open('pri_pre_ct1.csv', 'r') as csvfile:
    lines = csv.reader(csvfile, delimiter=',')
    for row in lines:
        minct.append(float(row[0]))

figure, axis = plt.subplots(2, 2)
# For TAT Function
axis[0, 0].plot(x, minTat, color='g', linestyle='dashed',
                marker='o', label="Small Number => High priority")
axis[0, 0].set_title("Turn Around Time")
axis[0, 0].grid()
# For CT Function
axis[0, 1].plot(x, minct, color='g', linestyle='dashed',
                marker='o')
axis[0, 1].set_title("Completion Time")
axis[0, 1].grid()
# For RT Function
axis[1, 0].plot(x, minrt, color='g', linestyle='dashed',
                marker='o')
axis[1, 0].set_title("Response Time")
axis[1, 0].grid()
# For WT Function
axis[1, 1].plot(x, minwt, color='g', linestyle='dashed',
                marker='o')
axis[1, 1].set_title("Waiting Time")
axis[1, 1].grid()
figure.legend()
plt.show()