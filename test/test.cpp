#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <bits/stdc++.h>
#include <pthread.h>
#include "../root/library.h"

using namespace std;

int main(){
  int fd = open("/home/varun/Desktop/AOS_GIT/ASSIGNMENT3/test/sample1/sample2/sample1.cpp", O_CREAT | O_WRONLY, S_IRWXG | S_IRWXU | S_IRWXO);
  close(fd);
}
