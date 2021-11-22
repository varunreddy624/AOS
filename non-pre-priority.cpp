#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
#include <queue>
#include <random>
#include <fstream>
#include <time.h>
using namespace std;
#define deb(x) cout << #x << " : " << x << "\n";
#define msg(x) cout << x;
struct process
{
    int P_ID;     //process ID
    int AT;       //arrival time
    int BT;       //burst time
    int Priority; //priority time
    int CT;       //completion time
    int TAT;      //turn around time
    int WT;       //waiting time
    int RT;       //response time
};
bool cmp_min(process &one, process &two)
{
    if (one.AT == two.AT)
    {
        if (one.Priority == two.Priority)
        {
            return one.P_ID < two.P_ID;
        }
        return one.Priority < two.Priority; //smaller priority gets high prefereeeence
    }
    return one.AT < two.AT;
}
struct CompareProcessMin
{
    bool operator()(process const &p1, process const &p2)
    {
        if (p1.Priority == p2.Priority)
        {
            if (p1.AT == p2.AT)
            {
                return p1.P_ID > p2.P_ID;
            }
            return p1.AT > p2.AT;
        }
        return p1.Priority > p2.Priority; // small number Higher Priority
    }
};
int main()
{
    srand(time(NULL));
    int choice;
    vector<process> info;
    process temp;
    int num_process;
    ofstream tat1;
    ofstream rt1;
    ofstream wt1;
    ofstream ct1;
    
    tat1.open("pri_non_pre_tat1.csv", fstream::out);
    rt1.open("pri_non_pre_rt1.csv", fstream::out);
    wt1.open("pri_non_pre_wt1.csv", fstream::out);
    ct1.open("pri_non_pre_ct1.csv", fstream::out);
    for (int i = 0; i < 100; ++i)
    {
        info.clear();
        num_process = rand() % 100;
        for (int i = 0; i < num_process; i++)
        {
            temp.P_ID = info.size();
            temp.AT = rand() % 100;
            temp.BT = rand() % 100;
            temp.Priority = rand() % 100;
            temp.WT = 0;
            temp.TAT = 0;
            temp.RT = 0;
            temp.CT = 0;
            info.push_back(temp);
        }
        
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
        tat1 << avg_tat << '\n';
        rt1 << avg_rt << '\n';
        wt1 << avg_wt << '\n';
        ct1 << avg_ct << '\n';
    
    }
    tat1.close();
    rt1.close();
    wt1.close();
    ct1.close();
    return 0;
}