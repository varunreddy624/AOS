#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <bits/stdc++.h>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <filesystem>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <fstream>
#include <algorithm> 
#include <sys/ioctl.h>
#include <signal.h> 

#define KEY_UP 119
#define KEY_DOWN 115
#define KEY_LEFT 97
#define KEY_RIGHT 100
#define ENTER 10
#define HOME 104
#define BACKSPACE 127
#define WINDOW_SIZE 10

using namespace std;

char HOME_LOC[1000],PWD[1000],PWD_COMMAND_MODE[1000];

struct termios term;

int x=1,y=1,start=1;

vector<string> leftFileNameStack;
vector<string> rightFileNameStack;
vector<string> file_vector;

FILE *filepntr;

void switch_to_normal_mode(char *path);


void log(string s){
    filepntr = fopen("/home/varun/Desktop/output.txt","a+");
    fprintf(filepntr,"%s\n",s.c_str());
    fclose(filepntr);
}

void log_int(int a){
    filepntr = fopen("/home/varun/Desktop/output.txt","a+");
    fprintf(filepntr,"%d\n",a);
    fclose(filepntr);
}

void clear_screen()
{
    cout << "\033[H\033[2J\033[3J";
}

void set_cursor()
{
    printf("\033[%d;%dH", x, y);
}

void set_cursor_top_left(){
    x=1;
    y=1;
    set_cursor();
}

char *getTimeInHunamReadableFormat(double size, char *buf)
{
    int i = 0;
    const char *units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while (size > 1024)
    {
        size /= 1024;
        i++;
    }
    sprintf(buf, "%.*f %s", i, size, units[i]);
    return buf;
}

void move_up(){
    if(x!=1)
        x--;
    set_cursor();
}

void move_down(){
    if(x<min((int)file_vector.size(),WINDOW_SIZE))
        x++;
    set_cursor();
}


void print_file_metadata(string curr_file_name)
{
    struct stat st;
    char *modval = (char *)malloc(sizeof(char) * 9 + 1);
    unsigned int size;
    timespec mod_time;
    struct passwd *pw;
    struct group *gr;
    char sizeInHumanReadableFormat[10];

    char temp[1000];
    strcpy(temp,PWD);
    strcat(temp,"/");
    strcat(temp,curr_file_name.c_str());

    if (stat(temp, &st) == 0)
    {
        pw = getpwuid(st.st_uid);
        gr = getgrgid(st.st_gid);

        size = st.st_size;

        mod_time = st.st_mtim;

        mode_t perm = st.st_mode;
        modval[0] = (perm & S_IRUSR) ? 'r' : '-';
        modval[1] = (perm & S_IWUSR) ? 'w' : '-';
        modval[2] = (perm & S_IXUSR) ? 'x' : '-';
        modval[3] = (perm & S_IRGRP) ? 'r' : '-';
        modval[4] = (perm & S_IWGRP) ? 'w' : '-';
        modval[5] = (perm & S_IXGRP) ? 'x' : '-';
        modval[6] = (perm & S_IROTH) ? 'r' : '-';
        modval[7] = (perm & S_IWOTH) ? 'w' : '-';
        modval[8] = (perm & S_IXOTH) ? 'x' : '-';
        modval[9] = '\0';
        if (strlen(curr_file_name.c_str()) <= 15)
            printf("%-17s\t%-10.10s\t%-.10s\t%-.10s\t%-.10s\t%s", curr_file_name.c_str(), getTimeInHunamReadableFormat(size, sizeInHumanReadableFormat), modval, pw->pw_name, gr->gr_name, ctime(&(mod_time.tv_sec)));
        else
            printf("%.15s..\t%-10.10s\t%-.10s\t%-.10s\t%-.10s\t%s", curr_file_name.c_str(), getTimeInHunamReadableFormat(size, sizeInHumanReadableFormat), modval, pw->pw_name, gr->gr_name, ctime(&(mod_time.tv_sec)));
    }
    else{
        cout << curr_file_name.c_str() << endl;
        cout << "error here with code " <<  stat(curr_file_name.c_str(), &st) << endl;
    }
}


void print_file_vector_window(){
    clear_screen();
    for(int i=0;i<WINDOW_SIZE && i+start-1<(int)file_vector.size();i++)
        print_file_metadata(file_vector[i+start-1]);
    set_cursor();
}


void list_dir(char *path)
{
    x=1;
    start=1;
    strcpy(PWD,path);

    file_vector.clear();

    struct dirent *entry;
    DIR *dir = opendir(path);
    if (dir == NULL)
        return;
    
    while ((entry = readdir(dir)) != NULL){
        file_vector.push_back(string(entry->d_name));
    }
    
    print_file_vector_window();

    closedir(dir);
}

void K(){
    if(start!=1){
        start--;
        print_file_vector_window();
    }
}

void L(){
    if(start+WINDOW_SIZE-1<file_vector.size()){
        start++;
        print_file_vector_window();
    }
}

void move_prev_visited_dir(){
    if(leftFileNameStack.size()>0){
        rightFileNameStack.push_back(string(PWD));
        string temp = leftFileNameStack.back();
        leftFileNameStack.pop_back();
        list_dir((char*)temp.c_str());
    }
}

void move_next_visited_dir(){
    if(rightFileNameStack.size()>0){
        leftFileNameStack.push_back(string(PWD));
        string temp = rightFileNameStack.back();
        rightFileNameStack.pop_back();
        list_dir((char*)temp.c_str());
    }
}


void levelUp(){
    int i,j;
    for(i=strlen(PWD)-2;i>=0;i--)
        if(PWD[i]=='/')
            break;
    char next_file_name[i+1];
    for(j=0;j<i;j++)
        next_file_name[j]=PWD[j];
    next_file_name[i]='\0';
    leftFileNameStack.push_back(string(PWD));
    rightFileNameStack.push_back(string(PWD));
    list_dir(next_file_name);
}

void enter(){

    int i,j;
    char *file_name = (char *)file_vector[start+x-2].c_str();

    if(strcmp(file_name,".")==0) 
        return;
    else if(strcmp(file_name,"..")==0 ){
        levelUp();
        return;
    }  

    char temp[1000];
    strcpy(temp,PWD);
    strcat(temp,"/");
    strcat(temp,file_name);

    struct stat st;
    if(stat(temp,&st)==0){
        if(S_ISDIR(st.st_mode)){
            leftFileNameStack.push_back(string(PWD));
            list_dir(temp);
        }
        else{
            pid_t pid=fork();
            if(pid==0){
                execl("/usr/bin/xdg-open","xdg-open",temp,NULL);
                exit(1);
            }
        }
    }
}

void normal()
{
    tcgetattr(STDIN_FILENO, &term);
    struct termios cur = term;
    cur.c_lflag &= ~(ECHO | ICANON);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &cur) != 0)
        cout << "Unable to Switch";
}

void denormal()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

bool recursiveSearchUtil(string path,string target){
    struct dirent *entry;
    struct stat st;
    DIR *dir = opendir(path.c_str());
    if (dir != NULL){
        while ((entry = readdir(dir)) != NULL){
            if(string(entry->d_name)!=".." && string(entry->d_name)!="."){
                if(string(entry->d_name)==target){
                    closedir(dir);
                    return true;
                }
                stat((path+"/"+string(entry->d_name)).c_str(),&st);
                if(S_ISDIR(st.st_mode)){
                    if(recursiveSearchUtil(path+"/"+string(entry->d_name),target)){
                        closedir(dir);
                        return true;
                    }
                }
            }
        }
        closedir(dir);
        return false;
    }
    return false;
}


void recursiveDeleteDirUtil(string path){
    struct dirent *entry;
    struct stat st;
    DIR *dir = opendir(path.c_str());
    if (dir != NULL){
        while ((entry = readdir(dir)) != NULL){
            if(string(entry->d_name)!=".." && string(entry->d_name)!="."){
                stat((path+"/"+string(entry->d_name)).c_str(),&st);
                if(S_ISDIR(st.st_mode))
                    recursiveDeleteDirUtil(path+"/"+string(entry->d_name));
                else
                    remove((path+"/"+string(entry->d_name)).c_str());
            }
        }
        closedir(dir); 
    }
    remove(path.c_str());
}

void copyDirRecursiveUtil(string srcPath,string desnPath,int flag){
    struct dirent *entry;
    struct stat st;
    DIR *dir = opendir(srcPath.c_str());
    if (dir != NULL){
        while ((entry = readdir(dir)) != NULL){
            if(string(entry->d_name)!=".." && string(entry->d_name)!="."){
                stat((srcPath+"/"+string(entry->d_name)).c_str(),&st);
                if(S_ISDIR(st.st_mode)){
                    mkdir((desnPath+"/"+string(entry->d_name)).c_str(),0777);
                    copyDirRecursiveUtil(srcPath+"/"+string(entry->d_name),desnPath+"/"+string(entry->d_name),flag);
                }
                else{
                    FILE *srcPntr,*targetPntr;
                    srcPntr = fopen((srcPath+"/"+string(entry->d_name)).c_str(),"r");
                    targetPntr = fopen((desnPath+"/"+string(entry->d_name)).c_str(),"w");
                    char ch;
                    while((ch = fgetc(srcPntr) ) != EOF )
                        fputc(ch, targetPntr);
                    fclose(srcPntr);
                    fclose(targetPntr);
                }
                if(flag==1)
                    remove((srcPath+"/"+string(entry->d_name)).c_str());
            }
        }
        closedir(dir); 
    }
}


string getFolderPath(string givenDir){
    if(givenDir[0]=='~')
        return string(HOME_LOC)+givenDir.substr(1,givenDir.length()-1);
    else
        return string(PWD_COMMAND_MODE)+givenDir.substr(1,givenDir.length()-1);
}


void handle_command(string command){
    stringstream ss(command);
    string word;
    int i,j;
    ss >> word;
    if(word=="copy" || word=="move"){
        vector<string> filePathList;
        string dirPath,fileName,temp;
        struct stat st;
        while(ss>>temp)
            filePathList.push_back(temp);

        dirPath=getFolderPath(filePathList.back());

        for(i=0;i<filePathList.size()-1;i++){
            stat(getFolderPath(filePathList[i]).c_str(),&st);

            for(j=filePathList[i].length()-1;j>=0;j--)
                if(filePathList[i][j]=='/')
                    break;
                
            fileName = filePathList[i].substr(j+1);

            if(!S_ISDIR(st.st_mode)){

                FILE *srcPntr,*targetPntr;
                
                if(j==-1)
                    srcPntr = fopen((string(PWD_COMMAND_MODE)+"/"+fileName).c_str(),"r");
                else
                    srcPntr = fopen(getFolderPath(filePathList[i]).c_str(),"r");
                    
                
                targetPntr = fopen((dirPath+"/"+fileName).c_str(),"w");

                char ch;
                while((ch = fgetc(srcPntr) ) != EOF )
                    fputc(ch, targetPntr);

                fclose(srcPntr);
                fclose(targetPntr);

                if(word=="move"){ //move ~/mywish2.txt ~/IMPPOINTS.txt ~/TEMP
                    if(j==-1)
                        remove((string(PWD_COMMAND_MODE)+"/"+fileName).c_str());
                    else
                        remove(getFolderPath(filePathList[i]).c_str());
                }
            }
            else{

                mkdir((dirPath+"/"+fileName).c_str(),0777);
                if(word=="copy")
                    copyDirRecursiveUtil(getFolderPath(filePathList[i]),dirPath+"/"+fileName,0);
                
                else{
                    copyDirRecursiveUtil(getFolderPath(filePathList[i]),dirPath+"/"+fileName,1);
                    remove(getFolderPath(filePathList[i]).c_str());   
                }
                
            }
        }
    }
    else if(word=="rename"){
        string oldFileName,newFileName;
        ss >> oldFileName >> newFileName;
        rename((string(PWD_COMMAND_MODE)+"/"+oldFileName).c_str(),(string(PWD_COMMAND_MODE)+"/"+newFileName).c_str());
    }
    else if(word=="create_file"){
        string fileName,folderPath;
        ss >> fileName >> folderPath;
        fclose(fopen((getFolderPath(folderPath)+"/"+fileName).c_str(),"w"));
    }
    else if(word=="create_dir"){
        string dirName,path;
        ss >> dirName >> path;
        mkdir((getFolderPath(path)+"/"+dirName).c_str(),0777);
    }
    if(word=="delete_file"){
        ss >> word;
        string folderPath = getFolderPath(word);
        remove(folderPath.c_str());
    }
    else if(word=="delete_dir"){
        string dirPath;
        ss >> dirPath;
        recursiveDeleteDirUtil(getFolderPath(dirPath));
        unlink(getFolderPath(dirPath).c_str());
    }
    else if(word=="goto"){
        ss >> word;
        strcpy(PWD_COMMAND_MODE,(getFolderPath(word)).c_str());
        
        // log("goto PWD_IN_CMD_MODE "+string(PWD_COMMAND_MODE));
    }
    else if(word=="search"){
        string fileName;
        ss >> fileName;

        if(recursiveSearchUtil(string(PWD_COMMAND_MODE),fileName)){
            cout << "true\n";
        }
        else{
            cout << "false\n";
        }
    }
}

void switch_to_command_mode(){
    // log(HOME_LOC);
    strcpy(PWD_COMMAND_MODE,HOME_LOC);
    clear_screen();
    struct winsize sz;
    ioctl( 0, TIOCGWINSZ, &sz );
    x=sz.ws_row;
    set_cursor();
    char c;
    char command[1000];
    int i=0,f=0;
    while((c=getchar())!=27){
        if(c=='q' && f==0){
            denormal();
            exit(1);
        }
        else if(c=='\n'){
            f=0;
            command[i]='\0';
            clear_screen();
            x=sz.ws_row;
            set_cursor();
            handle_command(string(command));
            i=0;
        }
        else{
            f=1;
            command[i++]=c;
            cout << c;
        }
    }
    switch_to_normal_mode(PWD);
}


void switch_to_normal_mode(char *path)
{
    normal();
    list_dir(path);
    char c=' ';
    while ((c=getchar())!=':')
    {
        switch (c)
        {
            case KEY_UP:
                move_up(); //key up
                break;
            case KEY_DOWN:
                move_down(); // key down
                break;
            case KEY_LEFT:
                move_prev_visited_dir(); // key left
                break;
            case KEY_RIGHT:
                move_next_visited_dir(); // key right
                break;
            case ENTER:
                enter();
                break;
            case BACKSPACE:
                levelUp();
                break;
            case HOME:
                list_dir(HOME_LOC);
                leftFileNameStack.push_back(string(PWD));
                rightFileNameStack.push_back(string(PWD));
                break;
            case 107:
                K();
                break;
            case 108:
                L();
                break;
            default:
                clear_screen();
                cout << endl << "wrong key entered" << endl; // not arrow
                denormal();
                exit(1);
        }
    }
    switch_to_command_mode();
}

int main()
{
    fclose(fopen("/home/varun/Desktop/output.txt","w"));
    getcwd(HOME_LOC, 1000);
    switch_to_normal_mode(HOME_LOC);
}
