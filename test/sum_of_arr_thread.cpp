#include <stdio.h>
#include <iostream>
#include <bits/stdc++.h>
#include <pthread.h>

using namespace std;

struct startEnd{
    int id,start,end;
};


vector<int> arr,res(4,0);

typedef struct startEnd startEnd;

void* findThreadSum(void* startEndData){
    startEnd* data = (startEnd*)startEndData;
    int start=data->start,end=data->end,id=data->id;
    for(int i=start;i<=end;i++)
        res[id]+=arr[i];
    free(data);
    return 0;
}

int main(){
    int i;
    pthread_t worker[4];
    for(i=1;i<=16;i++)
        arr.push_back(i);
    for(i=0;i<16;i+=4){
        startEnd* s = (startEnd*)malloc(sizeof(startEnd));
        s->id=i/4;
        s->start=i;
        s->end=i+3;
        pthread_create( &worker[i/4] , NULL ,  findThreadSum , (void*)s);
    }
    for(i=0;i<4;i++)
        pthread_join(worker[i],NULL);
    for(i=0;i<4;i++)
        cout << res[i] << endl;
}