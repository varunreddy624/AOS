#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#define CHUNK_SIZE 524288

using namespace std;

using namespace std;

struct thread_data{
    uint32_t address;
    int sockfd;
};

typedef struct thread_data thread_data;

class File{
    public:
        string fileName;
        int numOfChunks;
        vector<string> pieceWiseSHAOfFile;
        unordered_map<string,string> usersContainingWholeFileAndTheirLocalPaths;
        File(int noc){
            numOfChunks=noc;
        }
};


class Group{
    public:
        string groupId,ownerUserId;

        unordered_set<string> members;
        unordered_set<string> pendingRequests;
        unordered_map<string,File> fileNameToFileObj;

        Group(string gId,string ownerId){
            groupId=gId;
            ownerUserId = ownerId;
            members.insert(ownerId);
        }

        void addMember(string userId){
            pendingRequests.erase(userId);
            members.insert(userId);
        }

        int removeMember(string userId){
            int presentOrNot = members.erase(userId);
            if(presentOrNot==1 && userId==ownerUserId){
                if(members.size()==0)
                    return 1;
                
                else{
                    ownerUserId=*members.begin();
                    cout << "changed owner id of " << groupId << " group to " << ownerUserId << endl;
                    return 0;
                }
            }
            return 0;
        } 

        bool operator ==(const Group& p) const
        {
            return groupId == p.groupId;
        }
};

class UserInfo{
    public: 
        string userIP,password;
        bool isLoggedIn;
        int port;
        UserInfo(string ip,string pwd){
            userIP=ip;
            password=pwd;
            isLoggedIn=false;
        }
};

string trackerIP;
int port;

unordered_map <string,UserInfo> usernameToUserInfo;
unordered_map <string,string> ipToUsername;
unordered_map <string,Group> groupNameToGroupObj;

string getIPfromNetworkByteOrder(uint32_t address){
    char IP[40];
	inet_ntop(AF_INET,&address,IP,32);
    return string(IP);
}

string getFileNameFromFilePath(string path){
	int i,n=path.length();
	for(i=n-1;i>=0;i--)
		if(path[i]=='/')
			break;
	return path.substr(i+1);
}

pair<int,string> deserializeSeqNumAndFilePath(string header){
	pair<int,string> res;
	res.first = stoi(header.substr(0,5));
	int i;
	for(i=5;i<100;i++)
		if(header[i]=='/')
			break;
	res.second = header.substr(i+1,100-i-1);
	return res;
}

string serializeSeqNum(int seqnumber){
	string res(5,'0');
	int j=4;
	while(seqnumber>0){
		res[j--]=(char)(48+seqnumber%10);
		seqnumber=seqnumber/10;
	}
	return res;
}

pair<int,string> getIPAndPortFromFileName(char* fn){
	string filename = string(fn);
    vector<string> lines;
    string line;
    ifstream input_file(filename);

    if (!input_file.is_open()) {
        cerr << "Could not open the file - '" << filename << "'" << endl;
		return make_pair(0,"0.0.0.0");
    }

	else{
		getline(input_file, line);
		string trackerIP = line;
		getline(input_file, line);
		int port= stoi(line);
		return make_pair(port,trackerIP);
	}
}


bool stop_sharing(string groupId,string fileName,string userID){
    auto it = groupNameToGroupObj.find(groupId);
    if(it!=groupNameToGroupObj.end() && (it->second).members.find(userID)!=(it->second).members.end() && (it->second).fileNameToFileObj.find(fileName)!=(it->second).fileNameToFileObj.end()){
        (((it->second).fileNameToFileObj.find(fileName))->second).usersContainingWholeFileAndTheirLocalPaths.erase(userID);
        if((((it->second).fileNameToFileObj.find(fileName))->second).usersContainingWholeFileAndTheirLocalPaths.size()==0){
            (it->second).fileNameToFileObj.erase(fileName);
        }
        return true;
    }
    return false;
}

void leave_group(string groupID,string userID){
    auto it = groupNameToGroupObj.find(groupID);
    for(auto& file:(it->second).fileNameToFileObj)
        stop_sharing(groupID,file.first,userID);
        
    if((it->second).removeMember(userID)==1)
        groupNameToGroupObj.erase(groupID);
}


void* manage_peer_request_handler(void* data){
	thread_data* d = (thread_data*)data;
    int client_sock_fd = d->sockfd;
    string clientIP = getIPfromNetworkByteOrder(d->address);

	int SIZE = 1024,n;
	char COMMAND[SIZE];

    n = read(client_sock_fd, COMMAND, SIZE);
    COMMAND[n]='\0';

    char * pch;
    pch = strtok (COMMAND," ");
    vector<string> cmdVector;

    while (pch != NULL)
    {
        cmdVector.push_back(string(pch));
        pch = strtok (NULL, " ");
    }

    if(cmdVector[0]=="create_user"){
        char res[1];
        res[0]='0';
        if(usernameToUserInfo.find(cmdVector[1])==usernameToUserInfo.end()){
            res[0]='1';
            usernameToUserInfo.insert(make_pair(cmdVector[1],UserInfo(clientIP,cmdVector[2])));
            ipToUsername[clientIP]=cmdVector[1];
            cout << clientIP << ":" << cmdVector[0] << " " << cmdVector[1] << endl;
        }
        write(client_sock_fd,res,1);
    }
    else if(cmdVector[0]=="login"){
        char res[1];
        res[0]='0';
        if(usernameToUserInfo.find(cmdVector[1])!=usernameToUserInfo.end() && ((usernameToUserInfo.find(cmdVector[1]))->second).password==cmdVector[2]){
            ((usernameToUserInfo.find(cmdVector[1]))->second).isLoggedIn=true;
            res[0]='1';
            cout << clientIP << ":" << cmdVector[0] << " " << cmdVector[1] << endl;
        }
        write(client_sock_fd,res,1);
    }
    else if(cmdVector[0]=="create_group"){
        char res[1];
        res[0]='0';
        if(groupNameToGroupObj.find(cmdVector[1])==groupNameToGroupObj.end()){
            res[0]='1';

            groupNameToGroupObj.insert(pair<string,Group>(cmdVector[1],Group(cmdVector[1],ipToUsername[clientIP])));
            cout << clientIP << ":" << cmdVector[0] << " " << cmdVector[1] << " " << endl;
        }
        write(client_sock_fd,res,1);
    }
    else if(cmdVector[0]=="join_group"){
        char res[1];
        res[0]='0';
        if(groupNameToGroupObj.find(cmdVector[1])!=groupNameToGroupObj.end()){
            res[0]='1';

            auto it = groupNameToGroupObj.find(cmdVector[1]);
            (it->second).pendingRequests.insert(ipToUsername[clientIP]);
            cout << clientIP << ":" << cmdVector[0] << " " << cmdVector[1] << " " << endl;
        }
        write(client_sock_fd,res,1);
    }
    else if(cmdVector[0]=="list_group"){
        char grpList[CHUNK_SIZE];
        int grpPtr=0;
        for(auto& i:groupNameToGroupObj){
            for(char j:i.first)
                grpList[grpPtr++]=j;
            
            grpList[grpPtr++]='\n';
        }
        cout << clientIP << ":" << cmdVector[0] << endl;
        grpList[grpPtr]='\0';
        write(client_sock_fd,grpList,grpPtr+1);
    }
    else if(cmdVector[0]=="list_requests"){
        char reqList[CHUNK_SIZE];
        int reqPtr=1;
        reqList[0]='0';
        if(groupNameToGroupObj.find(cmdVector[1])!=groupNameToGroupObj.end()){
            reqList[0]='1';
            auto it = groupNameToGroupObj.find(cmdVector[1]);
            for(auto& i:(it->second).pendingRequests){
                for(char j:i)
                    reqList[reqPtr++]=j;
                
                reqList[reqPtr++]='\n';
            }
            cout << clientIP << ":" << cmdVector[0] << " " << cmdVector[1] << endl;
        }
        reqList[reqPtr]='\0';
        write(client_sock_fd,reqList,reqPtr+1);
    }
    else if(cmdVector[0]=="accept_request"){
        char res[1];
        res[0]='0';
        auto it = groupNameToGroupObj.find(cmdVector[1]);
        if(it!=groupNameToGroupObj.end() && (it->second).ownerUserId==ipToUsername[clientIP] && (it->second).pendingRequests.find(cmdVector[2])!=(it->second).pendingRequests.end()){
            res[0]='1';
            (it->second).addMember(cmdVector[2]);
            cout << clientIP << ":" << cmdVector[0] << " " << cmdVector[1] << " " << cmdVector[2] << endl;
        }
        write(client_sock_fd,res,1);
    }
    else if(cmdVector[0]=="reject_request"){
        char res[1];
        res[0]='0';
        auto it = groupNameToGroupObj.find(cmdVector[1]);
        if(it!=groupNameToGroupObj.end() && (it->second).ownerUserId==ipToUsername[clientIP] && (it->second).pendingRequests.find(cmdVector[2])!=(it->second).pendingRequests.end()){
            res[0]='1';
            (it->second).pendingRequests.erase(cmdVector[2]);
            cout << clientIP << ":" << cmdVector[0] << " " << cmdVector[1] << " " << cmdVector[2] << endl;
        }
        write(client_sock_fd,res,1);
    }
    else if(cmdVector[0]=="leave_group"){
        leave_group(cmdVector[1],ipToUsername[clientIP]);
        cout << clientIP << ":" << cmdVector[0] << " " << cmdVector[1] << " " << endl;
    }
    else if(cmdVector[0]=="list_files"){
        char fileList[CHUNK_SIZE];
        fileList[0]='0';
        int fileListPtr=1;
        auto it = groupNameToGroupObj.find(cmdVector[1]);
        if(it!=groupNameToGroupObj.end() && (it->second).members.find(ipToUsername[clientIP])!=(it->second).members.end()){
            fileList[0]='1';
            for(auto& fileObj:(it->second).fileNameToFileObj){
                for(char j:fileObj.first)
                    fileList[fileListPtr++]=j; 
                fileList[fileListPtr++]='\n';
            }
            cout << clientIP << ":" << cmdVector[0] << " " << cmdVector[1] << endl;
        }
        write(client_sock_fd,fileList,fileListPtr+1);
    }
    else if(cmdVector[0]=="upload_file"){
        char res[1];
        res[0]='0';

        string fileName = getFileNameFromFilePath(cmdVector[1]);
        auto it = groupNameToGroupObj.find(cmdVector[2]);
        
        if(it!= groupNameToGroupObj.end() && (it->second).members.find(ipToUsername[clientIP])!= (it->second).members.end() &&(it->second).fileNameToFileObj.find(fileName)==(it->second).fileNameToFileObj.end())
            res[0]='1';
        write(client_sock_fd,res,1);
        if(res[0]=='1'){
            char* fileMetaData = (char*)malloc(CHUNK_SIZE*sizeof(char));
            bzero(fileMetaData,CHUNK_SIZE);
            int offset=0,actually_read;
            while (1) {
                actually_read = read(client_sock_fd, fileMetaData+offset, CHUNK_SIZE);
                if (actually_read <= 0)
                    break;
                offset+=actually_read;
            }
            
            string filemetdata = string(fileMetaData);
            string header = filemetdata.substr(0,100);
            pair<int,string> temp = deserializeSeqNumAndFilePath(header);
            int totalNumOfChunks = temp.first;
            File f(totalNumOfChunks);
            f.fileName=fileName;
            for(int i=0;i<totalNumOfChunks;i++){
                f.pieceWiseSHAOfFile.push_back(filemetdata.substr(100+(40*i),40));
                // cout << f.pieceWiseSHAOfFile.back() << endl;
            }
            
            f.usersContainingWholeFileAndTheirLocalPaths.insert(make_pair(ipToUsername[clientIP],cmdVector[1]));
            (it->second).fileNameToFileObj.insert(make_pair(fileName,f));
            
            cout << clientIP << ":" << cmdVector[0] << " " << cmdVector[1] << " " << cmdVector[2] << endl << flush;
            free(fileMetaData);
        }
    }
    else if(cmdVector[0]=="download_file"){ 
        char* res = (char*)malloc(CHUNK_SIZE*sizeof(char));
        res[0]='0';
        int n=1;
        auto it = groupNameToGroupObj.find(cmdVector[1]);
        if(it!=groupNameToGroupObj.end() && (it->second).members.find(ipToUsername[clientIP])!= (it->second).members.end() && (it->second).fileNameToFileObj.find(cmdVector[2])!=(it->second).fileNameToFileObj.end()){
            res[0]='1';
            int i;

            unordered_map<string,string> userIdsAndThierLocalPath = (((it->second).fileNameToFileObj.find(cmdVector[2]))->second).usersContainingWholeFileAndTheirLocalPaths;
            int totalNumOfChunks = (((it->second).fileNameToFileObj.find(cmdVector[2]))->second).numOfChunks;
            vector<string> pieceWiseSHA = (((it->second).fileNameToFileObj.find(cmdVector[2]))->second).pieceWiseSHAOfFile;
            string seqnum = serializeSeqNum(totalNumOfChunks);
            string chunkSHA="";
            for(i=0;i<totalNumOfChunks;i++)
                chunkSHA+=pieceWiseSHA[i];
            string peerInfo="";
            for(auto& it2: userIdsAndThierLocalPath)
                if(((usernameToUserInfo.find(it2.first))->second).isLoggedIn)
                    peerInfo = peerInfo + " " + ((usernameToUserInfo.find(it2.first))->second).userIP + " " + it2.second;
            string temp = seqnum+chunkSHA+peerInfo;
            n+=temp.length();
            memcpy(res+1,temp.c_str(),temp.length());
            (((it->second).fileNameToFileObj.find(cmdVector[2]))->second).usersContainingWholeFileAndTheirLocalPaths.insert(make_pair(ipToUsername[clientIP],cmdVector[3]+"/"+cmdVector[2]));
            cout << clientIP << ":" << cmdVector[0] << " " << cmdVector[1] << " " << cmdVector[2] << endl << flush;
        }
        res[n]='\0';
        write(client_sock_fd,res,n+1);
        free(res);      
    }
    else if(cmdVector[0]=="stop_sharing"){
        char res[1];
        res[0]='0';
        if(stop_sharing(cmdVector[1],cmdVector[2],ipToUsername[clientIP])){
            res[0]='1';
            cout << clientIP << ":" << cmdVector[0] << " " << cmdVector[1] << " " << cmdVector[2] << endl;
        }
        write(client_sock_fd,res,1);
    }
    else if(cmdVector[0]=="logout"){
        auto it = usernameToUserInfo.find(ipToUsername[clientIP]);
        if(it!=usernameToUserInfo.end()){
            (it->second).isLoggedIn=false;
            cout << clientIP << ":" << cmdVector[0] << endl;
        }
    } 
	return NULL;
}

class Parent{
	public: 
		string IP;
		int port,sock_fd,opt = 1;
		
		struct sockaddr_in addr;

        Parent(string ip){
             if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
				printf("\n Socket creation error \n");
				return ;
			}

			if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))){
				perror("setsockopt");
				exit(EXIT_FAILURE);
			}

			addr.sin_family = AF_INET;
			if(inet_pton(AF_INET, ip.c_str(), &addr.sin_addr)<=0){
				perror("Invalid address Address not supported");
				exit(EXIT_FAILURE);
			}

			if (bind(sock_fd, (struct sockaddr *)&addr,sizeof(addr))<0){
				perror("bind failed");
				exit(EXIT_FAILURE);
			}
        }
		Parent(string ip,int p){
			IP=ip;
			port=p;

			if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
				printf("\n Socket creation error \n");
				return ;
			}

			if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))){
				perror("setsockopt");
				exit(EXIT_FAILURE);
			}

			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);
			if(inet_pton(AF_INET, ip.c_str(), &addr.sin_addr)<=0){
				perror("Invalid address Address not supported");
				exit(EXIT_FAILURE);
			}

			if (bind(sock_fd, (struct sockaddr *)&addr,sizeof(addr))<0){
				perror("bind failed");
				exit(EXIT_FAILURE);
			}
		}
};

class Tracker : public Parent{	
    public:

        struct sockaddr_in client_address;

        int new_sock_fd,addrlen = sizeof(client_address);


        Tracker(string ip,int port):Parent(ip,port){
			if (listen(sock_fd, 1000) < 0){
                perror("listen");
                exit(EXIT_FAILURE);
            }	
			 while(new_sock_fd = accept(sock_fd, (struct sockaddr *)&client_address, (socklen_t*)&addrlen)){
                thread_data *t;
                t=(thread_data*)malloc(sizeof(thread_data));
                t->address=client_address.sin_addr.s_addr;
                t->sockfd=new_sock_fd;

                pthread_t sniffer_thread;

				if(pthread_create( &sniffer_thread , NULL ,  manage_peer_request_handler , (void*)t)){
					perror("could not create thread");
					return ;
				}
            }
		}
};

void* start_server(void*){
    Tracker s(trackerIP,port);
    return NULL;
}

int main(int argc, char* argv[]){
    pair<int,string> details = getIPAndPortFromFileName(argv[2]);
    trackerIP = details.second;
    port = details.first;

    pthread_t server_thread;
    if(pthread_create( &server_thread , NULL ,  start_server , NULL)){
		perror("could not create thread");
		return 0;
	}
    
    char cmd[100];
    while(1){
        cin.getline(cmd,100);
        if(string(cmd)=="quit")   
            exit(0);
    }
}