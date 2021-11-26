import matplotlib.pyplot as plt
import numpy as np
import os, csv, operator

data = csv.reader(open('sjf_stack_bar.csv'),delimiter=',')
data = sorted(data, key=operator.itemgetter(0))


bt_list = []
wt_list = []
proc_names_list = []

for d in data:
    proc_names_list.append(d[1])
    bt_list.append(int(d[2]))
    wt_list.append(int(d[3]))


num_bars = len(bt_list)
fig, ax = plt.subplots()
ind = np.arange(num_bars)
bar_width = 0.35

p1 = ax.bar(proc_names_list, wt_list, bar_width, label='WT')
p2 = ax.bar(proc_names_list, bt_list, bar_width, label='BT')
ax.set_ylabel('Time')
ax.set_title('BT and WT of processes scheduled with SJF Algorithm')
#ax.set_xticks(ind, labels=proc_names_list)
ax.set_xlabel('Process name')
ax.legend()
plt.savefig('sjf_wt_bt_bars.png')
os.remove("sjf_stack_bar.csv")
