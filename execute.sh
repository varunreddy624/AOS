#!/bin/bash
###################### varun Buggy #######################
cd SJF
#Compiler code
g++ "sjf.cpp" -o "sjf"
#execute
./sjf input.txt
python3 sjf_bar_graph.py
cp sjf_wt_bt_bars.png ./../OUT/sjf_wt_bt_bars.png
python3 sjf_gantt_chart.py
cp sjf_gantt.png ./../OUT/sjf_gantt.png
python3 sjf_line_graph.py
cp sjf_line.png ./../OUT/sjf_line.png
cd ..

cd SRTF
#Compiler code
g++ "srtf.cpp" -o "srtf"
#execute
./srtf input.txt
python3 srtf_bar_graph.py
cp srtf_wt_bt_bars.png ./../OUT/srtf_wt_bt_bars.png
python3 srtf_gantt_chart.py
cp srtf_gantt.png ./../OUT/srtf_gantt.png
python3 srtf_line_graph.py
cp srtf_line.png ./../OUT/srtf_line.png
cd ..
##################### varun Buggy #######################

# ######################SAI Working############################
cd FCFS/
#Compiler code
g++ "FCFS.cpp" -o "FCFS1"
#execute
./FCFS1
#plot Graph
python3 fcfs-bars.py
cp fcfs_wt_bt_bars.png ./../OUT/fcfs_wt_bt_bars.png
python3 fcfs-gnat.py
cp gantt_fcfs.png ./../OUT/gantt_fcfs.png
python3 fcfs-line.py
cp fcfs_line.png ./../OUT/fcfs_line.png
rm "FCFS1"


#Compiler code
g++ "FCFST.cpp" -o "FCFST"
#execute
./FCFST
#plot Graph
python3 fcfs-graph.py
cp fcfs_convoy.png ./../OUT/fcfs_convoy.png
rm "FCFST"
cd ..

cd RR
#Compiler code
g++ "RRT.cpp" -o "RRT"
#execute
./RRT
#plot Graph
python3 rr-graph.py
cp rr_tq.png ./../OUT/rr_tq.png
rm "RRT"


#Compiler code
g++ "RR.cpp" -o "RR"
#execute
./RR
#plot Graph
python3 rr-bars.py
cp rr_wt_bt_bars.png ./../OUT/rr_wt_bt_bars.png
python3 rr-gnat.py
cp ganttrr.png ./../OUT/ganttrr.png
python3 rr-line.py
cp rr_line.png ./../OUT/rr_line.png
rm "RR"
cd ..
# ##############SAI Working ##################

##############Pratik ##################
cd MLQ
cd MLQ_RR2
#Compiler code
g++ "mlq_rr2.cpp" -o "mlq_rr2"
#execute
./mlq_rr2
python3 bars.py
python3 gantt.py
python3 line.py
cp mlq_rr2_bt_wt.png ./../../OUT/mlq_rr2_bt_wt.png
cp mlq_rr2_gantt.png ./../../OUT/mlq_rr2_gantt.png
cp mlq_rr2_line.png ./../../OUT/mlq_rr2_line.png
cd ..


cd MLQ_RR4
#Compiler code
g++ "mlq_rr4.cpp" -o "mlq_rr4"
#execute
./mlq_rr4
python3 bars.py
python3 gantt.py
python3 line.py
cp mlq_rr4_bt_wt.png ./../../OUT/mlq_rr4_bt_wt.png
cp mlq_rr4_gantt.png ./../../OUT/mlq_rr4_gantt.png
cp mlq_rr4_line.png ./../../OUT/mlq_rr4_line.png
cd ..
cd ..



cd MLFQ
cd MLFQ_RR24
#Compiler code
g++ "mlfq_rr24.cpp" -o "mlfq_rr24"
#execute
./mlfq_rr24
python3 bars.py
python3 gantt.py
python3 line.py
cp mlfq_rr24_bt_wt.png ./../../OUT/mlfq_rr24_bt_wt.png
cp mlfq_rr24_gantt.png ./../../OUT/mlfq_rr24_gantt.png
cp mlfq_rr24_line.png ./../../OUT/mlfq_rr24_line.png
cd ..


cd MLFQ_RR48
#Compiler code
g++ "mlfq_rr48.cpp" -o "mlfq_rr48"
#execute
./mlfq_rr48
python3 bars.py
python3 gantt.py
python3 line.py
cp mlfq_rr48_bt_wt.png ./../../OUT/mlfq_rr48_bt_wt.png
cp mlfq_rr48_gantt.png ./../../OUT/mlfq_rr48_gantt.png
cp mlfq_rr48_line.png ./../../OUT/mlfq_rr48_line.png
cd ..
cd ..
##############Pratik ##################

################# Nitin ##########################

cd Priority/Non_pre/
g++ "main.cpp" -o "main"
./main
cd HNHP/SCRIPT/
python3 bars.py
python3 gantt.py
python3 line.py
cd ../../
cd LNHP/SCRIPT/
python3 bars.py
python3 gantt.py
python3 line.py
cd ../../
rm "main"
cp LNHP/OUTPUT/* ./../../OUT/
cp HNHP/OUTPUT/* ./../../OUT/
cd ..


cd Pre/
g++ "main.cpp" -o "main"
./main
cd HNHP/SCRIPT/
python3 bars.py
python3 gantt.py
python3 line.py
cd ../../
cd LNHP/SCRIPT/
python3 bars.py
python3 gantt.py
python3 line.py
cd ../../
rm "main"
cp LNHP/OUTPUT/* ./../../OUT/
cp HNHP/OUTPUT/* ./../../OUT/
cd ..

cd CMP
g++ NPre_HNHP_LNHP_cmp.cpp -o NPre_HNHP_LNHP_cmp
./NPre_HNHP_LNHP_cmp
python3 NPre_CMP.py
g++ Pre_HNHP_LNHP_cmp.cpp -o Pre_HNHP_LNHP_cmp
./Pre_HNHP_LNHP_cmp
python3 Pre_CMP.py
cd ..
cd ..

#################Nitin Testing Pending##########################

python3 quad_graph.py



