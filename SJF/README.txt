sjf file is executed using 

g++ sjf.c -o sjf
./sjf input.txt

After execution, it will write avgerage parametric values (avg at,avg wt etc,.) to allout.txt in parent folder

It will generate 3 csv files
sjf_stack_bar.csv - info to generate bar graph
sjf_line.csv - info to generate line graph
sjf_gantt_chart.csv - info to generae gantt chart

Execution of the 3 py files will generate corresponding graph plots and the .png files are saved in same folder

SRTF is similar to sjf

ISSUES:
Even though values are pushed properly, quad_graph (graph which i should do) is not generated properly