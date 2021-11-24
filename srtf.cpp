#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
#include <queue>
#include <random>
#include <fstream>
#include <time.h>
#include <string>
#include <sstream>
#include <bits/stdc++.h>

using namespace std;

struct process
{
    int P_ID;               //process ID
    char name[100];         // process name
    int AT;                 //arrival time
    int BT;                 //burst time
    int CT;                 //completion time
    int TAT;                //turn around time
    int WT;                 //waiting time
    int RT;                 //response time
    int REMTIME;            // remaining time
    int PREV_ARRIVAL_TIME;  // Useful for gant chart
    bool first_exe; //tells whether it is first execution of process or not
};
bool cmp_min(process &one, process &two)
{
    if (one.AT == two.AT)
    {
        if (one.REMTIME == two.REMTIME)
        {
            return one.P_ID < two.P_ID;
        }
        return one.REMTIME < two.REMTIME;
    }
    return one.AT < two.AT;
}
struct CompareProcessMin
{
    bool operator()(process const &p1, process const &p2)
    {
        if (p1.REMTIME == p2.REMTIME)
        {
            if (p1.AT == p2.AT)
            {
                return p1.P_ID > p2.P_ID;
            }
            return p1.AT > p2.AT;
        }
        return p1.REMTIME > p2.REMTIME;
    }
};

void handleCPUBound(vector<process>& info,ofstream& bar_graph_stream,ofstream& other_graphs_stream){
    int num_process = info.size();

    unordered_map<int,int> procidToInfoInd;

    process temp;
    int current_time = 0;
    priority_queue<process, vector<process>, CompareProcessMin> minHeap;
    sort(info.begin(), info.end(), cmp_min);

    for(int i=0;i<info.size();i++){
        procidToInfoInd[info[i].P_ID]=i;
    }

    int index_in_array = 0;
    minHeap.push(info[index_in_array]);
    current_time = info[index_in_array].AT;
    index_in_array++;
    vector<process> ans;

    vector<vector<int>> tempvector;

    int prev_proc_id = -1;
    while (minHeap.empty() == false)
    {
        temp = minHeap.top();
        minHeap.pop();
        if (temp.first_exe == true)
        {
            temp.RT = current_time-temp.AT;
            temp.first_exe = false;
        }

        if(prev_proc_id!=-1 && temp.P_ID!=prev_proc_id){
            tempvector.push_back(vector<int>{prev_proc_id,info[procidToInfoInd[prev_proc_id]].PREV_ARRIVAL_TIME,current_time});
            info[procidToInfoInd[temp.P_ID]].PREV_ARRIVAL_TIME=current_time;
        }

        prev_proc_id=temp.P_ID;

        if (temp.REMTIME > 1)
        {
            temp.REMTIME = temp.REMTIME - 1;
            current_time++;
            minHeap.push(temp);
        }
        else if (temp.REMTIME == 1)
        {
            temp.REMTIME = 0;
            current_time++;
            temp.CT = current_time;
            temp.TAT = temp.CT - temp.AT;
            temp.WT = temp.TAT - temp.BT;
            ans.push_back(temp);
        }
        while (index_in_array < num_process && info[index_in_array].AT <= current_time)
        {
            minHeap.push(info[index_in_array++]);
        }
        if (minHeap.empty() && index_in_array < num_process)
        {
            minHeap.push(info[index_in_array]);
            current_time = info[index_in_array].AT;
            index_in_array++;
        }
    }
    tempvector.push_back(vector<int>{prev_proc_id,info[procidToInfoInd[prev_proc_id]].PREV_ARRIVAL_TIME,current_time});

    // cout << "PID\tAT\tBT\tCT\tWT\tTAT\n";
    for(int i=0;i<ans.size();i++){
        // printf("%d\t%d\t%d\t%d\t%d\t%d\n",ans[i].P_ID,ans[i].AT,ans[i].BT,ans[i].CT,ans[i].WT,ans[i].TAT);
        bar_graph_stream << ans[i].P_ID << ",";
        bar_graph_stream << ans[i].name << ",";    
        bar_graph_stream << 1 << ",";
        bar_graph_stream << ans[i].CT << ",";
        bar_graph_stream << ans[i].BT << ",";
        bar_graph_stream << ans[i].WT << ",";
        bar_graph_stream << ans[i].TAT << "\n";
    }

    for(int i=0;i<tempvector.size();i++){
        // cout << tempvector[i][0] << " " << tempvector[i][1] << " " << tempvector[i][2] << "\n";
        other_graphs_stream << tempvector[i][0] << ",";
        other_graphs_stream << info[procidToInfoInd[tempvector[i][0]]].name << ",";
        other_graphs_stream << 1 << ",";
        other_graphs_stream << tempvector[i][1] << ",";
        other_graphs_stream << tempvector[i][2] << "\n";
    }

    double avg_tat = 0, avg_rt = 0, avg_wt = 0, avg_ct = 0;
    for (unsigned int i = 0; i < ans.size(); i++)
    {
        avg_tat += ans[i].TAT;
        avg_rt += ans[i].RT;
        avg_wt += ans[i].WT;
        avg_ct += ans[i].CT;
    }
    avg_ct /= ans.size();
    avg_rt /= ans.size();
    avg_wt /= ans.size();
    avg_tat /= ans.size();
    // cout << avg_ct << " " << avg_rt << " " << avg_wt << " " << avg_tat << endl;
}

void handleIOBound(vector<process>& info,ofstream& bar_graph_stream,ofstream& other_graphs_stream){
    int num_process = info.size();
    for(int i=0;i<num_process;i++){
        info[i].CT=info[i].AT+info[i].BT;
        info[i].TAT=info[i].BT;
    }

    // cout << "PID\tNAME\t\t\tAT\tBT\tCT\tWT\tTAT\n";
    for(int i=0;i<info.size();i++){
        // printf("%d\t%s\t\t\t%d\t%d\t%d\t%d\t%d\n",info[i].P_ID,info[i].name,info[i].AT,info[i].BT,info[i].CT,info[i].WT,info[i].TAT);
        bar_graph_stream << info[i].P_ID << ",";
        bar_graph_stream << info[i].name << ",";    
        bar_graph_stream << 0 << ",";
        bar_graph_stream << info[i].CT << ",";
        bar_graph_stream << info[i].BT << ",";
        bar_graph_stream << info[i].WT << ",";
        bar_graph_stream << info[i].TAT << "\n";

        other_graphs_stream << info[i].P_ID << ",";
        other_graphs_stream << info[i].name << ",";
        other_graphs_stream << 0 << ",";
        other_graphs_stream << info[i].AT << ",";
        other_graphs_stream << info[i].CT << "\n";
    }

}


int main(int argc,char* argv[])
{
    #ifndef ONLINE_JUDGE
        freopen("input.txt","r",stdin);
        freopen("output.txt","w",stdout);
        freopen("err.txt","w",stderr);
    #endif

    vector<process> cpu_bound_processes,io_bound_processes;
    ofstream bar_graph_stream,other_graphs_stream;
    
    process temp;
    
    bar_graph_stream.open("srtf_bar.csv", fstream::out);
    other_graphs_stream.open("srtf_other.csv",fstream::out);
    

    // reading input from text file

    ifstream fin(argv[1], ios::in);
    string processstr;
    
    while(getline(fin, processstr))
    {
        stringstream ss(processstr);
        vector<string> params;
        string param;
        for(int i = 0; i < 5; i++)
        {
            if(getline(ss, param, ',')) params.push_back(param);
        }
        temp.P_ID=stoi(params[0]);
        strcpy(temp.name,params[1].c_str());
        temp.AT=stoi(params[3]);
        temp.PREV_ARRIVAL_TIME=temp.AT;
        temp.BT=stoi(params[4]);
        temp.REMTIME=temp.BT;
        temp.WT = 0;
        temp.TAT = 0;
        temp.RT = 0;
        temp.CT = 0;
        temp.first_exe=true;
        if(params[2]=="I/O-bound"){
            io_bound_processes.push_back(temp);
        }
        else{
            cpu_bound_processes.push_back(temp);
        }
    }
    // for(int i=0;i<cpu_bound_processes.size();i++){
    //     cout << cpu_bound_processes[i].P_ID << " " << cpu_bound_processes[i].name << " " << cpu_bound_processes[i].AT << " " << cpu_bound_processes[i].BT << endl;
    // }
    // reading input from text file
    handleCPUBound(cpu_bound_processes,bar_graph_stream,other_graphs_stream);
    handleIOBound(io_bound_processes,bar_graph_stream,other_graphs_stream);

    bar_graph_stream.close();
    other_graphs_stream.close();
    return 0;
}