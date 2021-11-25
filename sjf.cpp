#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
#include <queue>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

struct process
{
    int P_ID;       //process ID
    char name[100]; // process name
    int AT;         //arrival time
    int BT;         //burst time
    int CT;         //completion time
    int TAT;        //turn around time
    int WT;         //waiting time
    int RT;         //response time
};

bool cmp_min(process &one, process &two)
{
    if (one.AT == two.AT)
    {
        if (one.BT == two.BT)
        {
            return one.P_ID < two.P_ID;
        }
        return one.BT < two.BT; 
    }
    return one.AT < two.AT;
}

struct CompareProcessMin
{
    bool operator()(process const &p1, process const &p2)
    {
        if (p1.BT == p2.BT)
        {
            if (p1.AT == p2.AT)
            {
                return p1.P_ID > p2.P_ID;
            }
            return p1.AT > p2.AT;
        }
        return p1.BT > p2.BT;
    }
};


void handleCPUBound(vector<process>& info,ofstream& line_graph_stream,ofstream& gant_chart_stream, ofstream& stack_bar_graph_stream){
    int num_process = info.size();
    process temp;
    int current_time = 0;
    priority_queue<process, vector<process>, CompareProcessMin> minHeap;
    sort(info.begin(), info.end(), cmp_min);
    int index_in_array = 0;
    minHeap.push(info[index_in_array]);
    current_time = info[index_in_array].AT;
    index_in_array++;
    vector<process> ans;
    while (minHeap.empty() == false)
    {
        temp = minHeap.top();
        minHeap.pop();
        temp.RT = current_time - temp.AT;
        temp.CT = temp.BT + current_time;
        current_time = temp.CT;
        temp.TAT = temp.CT - temp.AT;
        temp.WT = temp.TAT - temp.BT;
        ans.push_back(temp);
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

    cout << "PID\tNAME\t\tAT\tBT\tCT\tWT\tTAT\n";
    for(int i=0;i<ans.size();i++){
        printf("%d\t%s\t\t%d\t%d\t%d\t%d\t%d\n",ans[i].P_ID,ans[i].name,ans[i].AT,ans[i].BT,ans[i].CT,ans[i].WT,ans[i].TAT);
        line_graph_stream << ans[i].P_ID << ",";
        line_graph_stream << ans[i].name << ",";    
        line_graph_stream << 1 << ",";
        line_graph_stream << ans[i].RT << ",";
        line_graph_stream << ans[i].WT << ",";
        line_graph_stream << ans[i].CT << ",";
        line_graph_stream << ans[i].TAT << "\n";

        gant_chart_stream << ans[i].P_ID << ",";
        gant_chart_stream << ans[i].name << ",";
        gant_chart_stream << 1 << ",";
        gant_chart_stream << ans[i].AT << ",";
        gant_chart_stream << ans[i].CT << "\n";

        stack_bar_graph_stream << ans[i].name << ",";
        stack_bar_graph_stream << ans[i].BT << ",";
        stack_bar_graph_stream << ans[i].WT << "\n";
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

void handleIOBound(vector<process>& info,ofstream& line_graph_stream,ofstream& gant_chart_stream,ofstream& stack_bar_graph_stream){
    int num_process = info.size();
    for(int i=0;i<num_process;i++){
        info[i].CT=info[i].AT+info[i].BT;
        info[i].TAT=info[i].BT;
    }

    // cout << "PID\tNAME\t\t\tAT\tBT\tCT\tWT\tTAT\n";
    for(int i=0;i<info.size();i++){
        // printf("%d\t%s\t\t\t%d\t%d\t%d\t%d\t%d\n",info[i].P_ID,info[i].name,info[i].AT,info[i].BT,info[i].CT,info[i].WT,info[i].TAT);
        line_graph_stream << info[i].P_ID << ",";
        line_graph_stream << info[i].name << ",";    
        line_graph_stream << 0 << ",";
        line_graph_stream << info[i].RT << ",";
        line_graph_stream << info[i].WT << ",";
        line_graph_stream << info[i].CT << ",";
        line_graph_stream << info[i].TAT << "\n";

        gant_chart_stream << info[i].P_ID << ",";
        gant_chart_stream << info[i].name << ",";
        gant_chart_stream << 0 << ",";
        gant_chart_stream << info[i].AT << ",";
        gant_chart_stream << info[i].CT << "\n";

        stack_bar_graph_stream << info[i].name << ",";
        stack_bar_graph_stream << info[i].BT << ",";
        stack_bar_graph_stream << info[i].WT << "\n";
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
    ofstream line_graph_stream,gant_chart_stream,stack_bar_graph_stream;
    
    process temp;
    
    line_graph_stream.open("sjf_line.csv", fstream::out);
    gant_chart_stream.open("sjf_gant.csv",fstream::out);
    stack_bar_graph_stream.open("sjf_stack_bar.csv",fstream::out);
    

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
        temp.BT=stoi(params[4]);
        temp.WT = 0;
        temp.TAT = 0;
        temp.RT = 0;
        temp.CT = 0;
        if(params[2]=="I/O-bound"){
            io_bound_processes.push_back(temp);
        }
        else{
            cpu_bound_processes.push_back(temp);
        }
    }

    // reading input from text file
    handleCPUBound(cpu_bound_processes,line_graph_stream,gant_chart_stream,stack_bar_graph_stream);
    handleIOBound(io_bound_processes,line_graph_stream,gant_chart_stream,stack_bar_graph_stream);

    line_graph_stream.close();
    gant_chart_stream.close();
    stack_bar_graph_stream.close();
    return 0;
}