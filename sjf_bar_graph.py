import matplotlib.pyplot as plt
import numpy as np

# xpoints = np.array([1, 8])
# ypoints = np.array([3, 10])

file = open("sjf_stack_bar.csv", "r")

bt_list = []
wt_list = []
proc_names_list = []

while(True):
    line = file.readline()
    if not line:
        break
    data = line.split(',')
    proc_names_list.append(data[0])
    bt_list.append(int(data[1]))
    wt_list.append(int(data[2]))
file.close()


num_bars = len(bt_list)
fig, ax = plt.subplots()
ind = np.arange(num_bars)
bar_width = 0.35

p1 = ax.bar(ind, wt_list, bar_width, label='WT')
p2 = ax.bar(ind, bt_list, bar_width, label='BT')
ax.set_ylabel('Time')
ax.set_title('BT and WT of processes scheduled with SJF Algorithm')
ax.set_xticks(ind, labels=proc_names_list)
ax.set_xlabel('Process name')
ax.legend()
plt.savefig('sjf_wt_bt_bars.png')
plt.show()

