#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <bits/stdc++.h>
#include <openssl/sha.h>
#define CHUNK_SIZE 524288

using namespace std;

#define UPLOAD_PORT 1818

string clientIPaddress;

class DownloadData{
    public:
        string peerIP,chunkSHA,peerLocalPath;
    DownloadData(string sha){
        chunkSHA = sha;
    }
};
vector<DownloadData> chunkMap; 


struct download_thread_data{
    int seqnum,fd;
    char *filename;
};
typedef struct download_thread_data download_thread_data;


struct thread_data{
    uint32_t address;
    int sockfd;
};
typedef struct thread_data thread_data;


string getIPfromNetworkByteOrder(uint32_t address){
    char IP[40];
	inet_ntop(AF_INET,&address,IP,32);
    return string(IP);
}

string serializeSeqNumAndFilePath(int seqnumber,string fileName){
	string data(100,'0');
	int i,j=99;
	for(i=fileName.length()-1;i>=0;i--)
		data[j--]=fileName[i];
	data[j]='/';
	j=4;
	while(seqnumber>0){
		data[j--]=(char)(48+seqnumber%10);
		seqnumber=seqnumber/10;
	}
	return data;
}

string getHexFromBin(char *SHA){
	string finalSHA;
	for(int i=0;i<20;i++){
		stringstream ss;
		ss << hex << (short)SHA[i];	
		string temp = ss.str();
		if(temp.length()>2)
			finalSHA+=ss.str().substr(2,2);
		else if(temp.length()==2)
			finalSHA+=ss.str();
		else
			finalSHA+=("0"+temp);
	}
	return finalSHA;
}

string calculateSHA1_OfPiece(char* data){
	char SHA[21];
	SHA1((const unsigned char*)data,strlen(data),(unsigned char*)SHA);
	return getHexFromBin(SHA);
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

vector<string> returnPieceWiseSHAOfFile(string filePath){
    int actually_read = 0;
    char data[CHUNK_SIZE];
    int fd = open(filePath.c_str(),O_RDONLY);
    vector<string> hashOfEachChunk;

    while((actually_read = read(fd,data, CHUNK_SIZE)) >= 0) {
        hashOfEachChunk.push_back(calculateSHA1_OfPiece(data));
        bzero(data,CHUNK_SIZE);
        if(actually_read==0)
            break;
    }
    close(fd);
    return hashOfEachChunk;
}

void* upload_request_handler(void *data){
	thread_data* d = (thread_data*)data;
    int client_sock_fd = d->sockfd;
    string filename,clientMsg,clientIP = getIPfromNetworkByteOrder(d->address);

	int n,SIZE=102,f=0;
	char buffer[SIZE]={0};
	string header;

	int headerReceived=0;
	
	while (1) {
		n = recv(client_sock_fd, buffer, SIZE, 0);
		if (n <= 0)
			break;
		
		header+=string(buffer);
        headerReceived+=n;
        bzero(buffer, SIZE);
        if(headerReceived==100)
            break;
	}

	pair<int,string> temp = deserializeSeqNumAndFilePath(header);

    int sequencenumber = temp.first;
    filename =temp.second;   

    char* tempPtr = (char*)malloc(512*(1<<10)*sizeof(char));

    int fd = open(filename.c_str(),O_RDONLY);

    int actuallyread = pread(fd,tempPtr,512*(1<<10),sequencenumber*(512*(1<<10)));

    write(client_sock_fd,tempPtr,actuallyread);

    free(tempPtr);
    free(d);
    close(client_sock_fd);

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

class Client : public Parent{
    public:
        Client(string ip):Parent(ip){
        }

        Client(string ip,int port):Parent(ip,port){
        }

        void connectToServer(string serverIP,int serverPort){
            struct sockaddr_in serv_addr;
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(serverPort);
            
            if(inet_pton(AF_INET, serverIP.c_str(), &serv_addr.sin_addr)<=0){
                printf("\nInvalid address/ Address not supported \n");
                return ;
            }

            if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
                printf("\nConnection Failed \n");
                return ;
            }
        }
};

class Server : public Parent{	
    public:

        struct sockaddr_in client_address;

        int new_sock_fd,addrlen = sizeof(client_address);


        Server(string ip,int port):Parent(ip,port){
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

				if(pthread_create( &sniffer_thread , NULL ,  upload_request_handler , (void*)t)){
					perror("could not create thread");
					return ;
				}
            }
		}
};

void* start_server(void*){
    Server p(clientIPaddress,UPLOAD_PORT);
    return NULL;
}

void* download_worker_fun(void* data){
    download_thread_data* d = (download_thread_data*)data;

    int i,sock_fd,n,SIZE=1+(512*(1<<10)),fp =0,seqnum=d->seqnum,filefd=d->fd;


    Client p(clientIPaddress);
    p.connectToServer(chunkMap[seqnum].peerIP,UPLOAD_PORT);

    sock_fd = p.sock_fd;

    string filename = chunkMap[seqnum].peerLocalPath;


    char* PAYLOAD = (char*)malloc(SIZE*sizeof(char));

    write(sock_fd,(serializeSeqNumAndFilePath(seqnum,filename)).c_str(),100);
        
    fp=0;
    bzero(PAYLOAD,SIZE);

    while (1) {
        n = recv(sock_fd, PAYLOAD+fp, SIZE, 0);
        if (n <= 0)
            break;
        fp+=n;
    }
    
    if(chunkMap[seqnum].chunkSHA==calculateSHA1_OfPiece(PAYLOAD))
        pwrite(filefd,PAYLOAD,fp,(seqnum)*(SIZE-1));

    free(d);
    free(PAYLOAD);
    close(sock_fd);

    return NULL;
}

void* download_file(void* fn){
    string filename = string((char*)fn);
    int i,CHUNKS=chunkMap.size();                             // CHUNKS can vary

    int fd = open(filename.c_str(), O_CREAT | O_WRONLY, S_IRWXG | S_IRWXU | S_IRWXO);
    pthread_t download_workers[CHUNKS];

    for(int i=0;i<CHUNKS;i++){
        download_thread_data* data = (download_thread_data*)malloc(sizeof(download_thread_data));
        data->filename=(char*)fn;
        data->seqnum=i;
        data->fd=fd;

        if(pthread_create( &download_workers[i] , NULL ,  download_worker_fun , (void*)data)){
            perror("could not create thread");
            return 0;
	    }
    }

    for(i=0;i<CHUNKS;i++)
        pthread_join(download_workers[i],NULL);

    close(fd);
    chunkMap.clear();

    cout << "download of file-" << string(filename) << " done" << endl;

    return NULL;
}

int main(int argc, char* argv[])
{
    clientIPaddress = string(argv[1]);

    bool isLoggedin = false;

    pair<int,string> temp = getIPAndPortFromFileName(argv[2]);

    int trackerPort= temp.first;
    string trackerIP= temp.second;

    pthread_t server_thread;
    if(pthread_create( &server_thread , NULL ,  start_server , NULL)){
		perror("could not create thread");
		return 0;
	}

    while(1){
        char command[1000];
        cin.getline(command,1000);
        string cmd = string(command);
        char * pch;
        pch = strtok (command," ");
        vector<string> cmdVector;

        while (pch != NULL)
        {
            cmdVector.push_back(string(pch));
            pch = strtok (NULL, " ");
        }

        int n = cmdVector.size();

        if(cmdVector[0]=="create_user"){
            if(n!=3)
                cout << "not valid number of arguments" << endl;

            else{
                Client c(clientIPaddress);
                c.connectToServer(trackerIP,trackerPort);
                int serv_sock_fd = c.sock_fd;

                write(serv_sock_fd,cmd.c_str(),cmd.length());    
                char res[1];
                read(serv_sock_fd,res,1);
                if(res[0]=='0')
                    cout << "user already exists; try another username" << endl;
                else
                    cout << "created account successfully" << endl;
                close(serv_sock_fd);
            }
        }
        else if(cmdVector[0]=="login"){
            if(n!=3)
                cout << "not valid number of arguments" << endl;
            
            else if(isLoggedin)
                cout << "already logged in " << endl;
            
            else{
                Client c(clientIPaddress);
                c.connectToServer(trackerIP,trackerPort);
                int serv_sock_fd = c.sock_fd;

                write(serv_sock_fd,cmd.c_str(),cmd.length());    
                char res[1];
                read(serv_sock_fd,res,1);
                if(res[0]=='0')
                    cout << "invalid credentials" << endl;
                else{
                    isLoggedin=true;
                    cout << "login successfull" << endl;
                }
                close(serv_sock_fd);
            }
        }
        else if(cmdVector[0]=="create_group"){
            if(!isLoggedin)
                cout << "not logged in;login first" << endl;
            else{
                if(n!=2)
                    cout << "not valid number of arguments" << endl;
                else{   
                    Client c(clientIPaddress);
                    c.connectToServer(trackerIP,trackerPort);
                    int serv_sock_fd = c.sock_fd;

                    write(serv_sock_fd,cmd.c_str(),cmd.length());    
                    char res[1];
                    read(serv_sock_fd,res,1);
                    if(res[0]=='0')
                        cout << "group id already exists; try with another group id" << endl;
                    else
                        cout << "group creation successfully" << endl;
                    
                    close(serv_sock_fd);
                }
            }
        }
        else if(cmdVector[0]=="join_group"){
            if(!isLoggedin)
                cout << "not logged in;login first" << endl;
            else{
                if(n!=2)
                    cout << "not valid number of arguments" << endl;
                else{
                    Client c(clientIPaddress);
                    c.connectToServer(trackerIP,trackerPort);
                    int serv_sock_fd = c.sock_fd;

                    write(serv_sock_fd,cmd.c_str(),cmd.length());    
                    char res[1];
                    read(serv_sock_fd,res,1);
                    if(res[0]=='0')
                        cout << "group doesn't exists; try with another group id" << endl;
                    else
                        cout << "join request sent successfully" << endl;
                    
                    close(serv_sock_fd);
                }
            }
        }
        else if(cmdVector[0]=="list_group"){
            if(!isLoggedin)
                cout << "not logged in;login first" << endl;
            else{
                Client c(clientIPaddress);
                c.connectToServer(trackerIP,trackerPort);
                int serv_sock_fd = c.sock_fd;

                write(serv_sock_fd,cmd.c_str(),cmd.length());    
                char grpList[1024];
                read(serv_sock_fd,grpList,1024);
                printf("\n");
                printf("%s",grpList);
                printf("\n");
                close(serv_sock_fd);
            }
        }
        else if(cmdVector[0]=="list_requests"){
            if(!isLoggedin)
                cout << "not logged in;login first" << endl;
            else{
                if(n!=2)
                    cout << "not valid number of arguments" << endl;
                else{
                    Client c(clientIPaddress);
                    c.connectToServer(trackerIP,trackerPort);
                    int serv_sock_fd = c.sock_fd;

                    write(serv_sock_fd,cmd.c_str(),cmd.length());    
                    char* grpList = (char*)malloc(CHUNK_SIZE*sizeof(char));
                    read(serv_sock_fd,grpList,CHUNK_SIZE);
                    if(grpList[0]=='0')
                        cout << "invalid group name; try again" << endl;
                    else{
                        printf("\n");
                        printf("%s",grpList+1);
                        printf("\n");
                    }
                    free(grpList);
                    close(serv_sock_fd);
                }
            }
        }
        else if(cmdVector[0]=="accept_request"){
            if(!isLoggedin)
                cout << "not logged in;login first" << endl;
            else{
                if(n!=3)
                    cout << "not valid number of arguments" << endl;
                else{
                    Client c(clientIPaddress);
                    c.connectToServer(trackerIP,trackerPort);
                    int serv_sock_fd = c.sock_fd;

                    write(serv_sock_fd,cmd.c_str(),cmd.length());    
                    char res[1];
                    read(serv_sock_fd,res,1);
                    if(res[0]=='0')
                        cout << "operation unsucessfull; please check operation again" << endl;
                    else
                        cout << "operation sucessfull" << endl;
                    close(serv_sock_fd);
                }
            }
        }
        else if(cmdVector[0]=="reject_request"){
            if(!isLoggedin)
                cout << "not logged in;login first" << endl;
            else{
                if(n!=3)
                    cout << "not valid number of arguments" << endl;
                else{
                    Client c(clientIPaddress);
                    c.connectToServer(trackerIP,trackerPort);
                    int serv_sock_fd = c.sock_fd;

                    write(serv_sock_fd,cmd.c_str(),cmd.length());    
                    char res[1];
                    read(serv_sock_fd,res,1);
                    if(res[0]=='0')
                        cout << "operation unsucessfull; please check operation again" << endl;
                    else
                        cout << "operation sucessfull" << endl;
                    close(serv_sock_fd);
                }
            }
        }
        else if(cmdVector[0]=="leave_group"){
            if(!isLoggedin)
                cout << "not logged in;login first" << endl;
            else{
                if(n!=2)
                    cout << "not valid number of arguments" << endl;
                else{
                    Client c(clientIPaddress);
                    c.connectToServer(trackerIP,trackerPort);
                    int serv_sock_fd = c.sock_fd;

                    write(serv_sock_fd,cmd.c_str(),cmd.length());    
                    close(serv_sock_fd);
                }
            }
            cout << "operation complete" << endl;
        }
        else if(cmdVector[0]=="list_files"){
            if(!isLoggedin)
                cout << "not logged in;login first" << endl;
            else{
                if(n!=2)
                    cout << "not valid number of arguments" << endl;
                else{
                    Client c(clientIPaddress);
                    c.connectToServer(trackerIP,trackerPort);
                    int serv_sock_fd = c.sock_fd;

                    write(serv_sock_fd,cmd.c_str(),cmd.length());    
                    char* fileList = (char*)malloc(CHUNK_SIZE*sizeof(char));
                    read(serv_sock_fd,fileList,CHUNK_SIZE);
                    if(fileList[0]=='0')
                        cout << "invalid command; check command arguments again" << endl;
                    else{
                        printf("\n");
                        printf("%s",fileList+1);
                        printf("\n");
                    }
                    free(fileList);
                    close(serv_sock_fd);
                }
            }
        }
        else if(cmdVector[0]=="upload_file"){
            if(n!=3){
                cout << "not valid number of arguments" << endl;
                continue;
            }
            else if(!isLoggedin){
                cout << "not logged in;login first" << endl;
                continue;
            }
            else{
                Client c(clientIPaddress);
                c.connectToServer(trackerIP,trackerPort);
                int serv_sock_fd = c.sock_fd;

                write(serv_sock_fd,cmd.c_str(),cmd.length());    
                char res[1];
                read(serv_sock_fd,res,1);
                if(res[0]=='0')
                    cout << "operation unsucessfull; please check operation again" << endl;
                else{
                    string filepath = cmdVector[1];
                    vector<string> pieceWiseSHA = returnPieceWiseSHAOfFile(filepath);
                    char fileMetaData[CHUNK_SIZE];
                    int i,j,fileMetaDataPtr=0;
                    string seqNumPlusFilePath = serializeSeqNumAndFilePath(pieceWiseSHA.size(),filepath);
                    for(i=0;i<100;i++)
                        fileMetaData[fileMetaDataPtr++]=seqNumPlusFilePath[i];
                    for(i=0;i<pieceWiseSHA.size();i++){
                        for(char j:pieceWiseSHA[i])
                            fileMetaData[fileMetaDataPtr++]=j;
                    }
                    fileMetaData[fileMetaDataPtr++]='\0';

                    write(serv_sock_fd,fileMetaData,fileMetaDataPtr);
                    cout << "operation sucessfull"<< endl;
                }
                close(serv_sock_fd);
            }
        }
        else if(cmdVector[0]=="download_file"){
            if(n!=4){
                cout << "not valid number of arguments" << endl;
                continue;
            }
            else if(!isLoggedin){
                cout << "not logged in;login first" << endl;
                continue;
            }
            else{
                Client c(clientIPaddress);
                c.connectToServer(trackerIP,trackerPort);
                int serv_sock_fd = c.sock_fd;
                
                write(serv_sock_fd,cmd.c_str(),cmd.length());

                char* peerIps = (char*)malloc(CHUNK_SIZE*sizeof(char));
                int actually_read = read(serv_sock_fd,peerIps,CHUNK_SIZE);
                if(peerIps[0]=='0')
                    cout << "operation unsucessfull; please check operation again" << endl << flush;
                else{
                    string payLoad = string(peerIps+1);
                    int seqnum = stoi(payLoad.substr(0,5));
                    for(int i=0;i<seqnum;i++){
                        chunkMap.push_back(DownloadData(payLoad.substr(5+(i*40),40)));
                        // cout << chunkMap.back().chunkSHA << endl;
                    }
                    string peerData = payLoad.substr(5+(seqnum*40)+1); // see this once you remove sending total file SHA
                    istringstream ss(peerData);
                    vector<pair<string,string>> peerIpAndPath;
                    string data;
                    int c=0;
                    while(getline(ss, data, ' ')){
                        if(c%2==0)
                            peerIpAndPath.push_back(make_pair(data,""));
                        else
                            peerIpAndPath.back().second=data;
                        c++;
                    }
                    int numberOfPeers = peerIpAndPath.size();
                    for(int i=1;i<seqnum+1;i++){
                        chunkMap[i-1].peerIP=peerIpAndPath[i%numberOfPeers].first;
                        chunkMap[i-1].peerLocalPath=peerIpAndPath[i%numberOfPeers].second;
                    }
                    pthread_t download_thread;
                    char fn[100];
                    strcpy(fn,(cmdVector[3]+"/"+cmdVector[2]).c_str());
                    if(pthread_create( &download_thread , NULL ,  download_file , (void*)fn)){
                        perror("could not create thread");
                        return 0;
                    }
                }
                free(peerIps);
            }
        }
        else if(cmdVector[0]=="stop_sharing"){
            if(n!=3){
                cout << "not valid number of arguments" << endl;
                continue;
            }
            else if(!isLoggedin){
                cout << "not logged in;login first" << endl;
                continue;
            }
            Client c(clientIPaddress);
            c.connectToServer(trackerIP,trackerPort);
            int serv_sock_fd = c.sock_fd;
            write(serv_sock_fd,cmd.c_str(),cmd.length());
            char res[1];
            read(serv_sock_fd,res,1);
            if(res[0]=='0')
                cout << "operation unsucessfull; please check operation again" << endl << flush;
            else
                cout << "operation sucessfull" << endl << flush;
        }   
        else if(cmdVector[0]=="logout"){
            Client c(clientIPaddress);
            c.connectToServer(trackerIP,trackerPort);
            int serv_sock_fd = c.sock_fd;
            write(serv_sock_fd,cmd.c_str(),cmd.length());  
            close(serv_sock_fd);
            isLoggedin=false;
            cout << "logout sucessfull" << endl << flush;
        }
    }

	return 0;
}