#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "../../library.h"

map<string,vector<pair<int,char*>>> fileIndex;

string ip;

struct thread_data{
    in_port_t port;
    uint32_t address;
    int sockfd;
};

typedef struct thread_data thread_data;

class Parent{
	public: 
		string IP;
		int port,sock_fd,opt = 1;
		
		struct sockaddr_in addr;

		Parent(char* ip,int p){
			IP=string(ip);
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
			if(inet_pton(AF_INET, ip, &addr.sin_addr)<=0){
				perror("Invalid address Address not supported");
				exit(EXIT_FAILURE);
			}

			if (bind(sock_fd, (struct sockaddr *)&addr,sizeof(addr))<0){
				perror("bind failed");
				exit(EXIT_FAILURE);
			}

		}
};

class Client : Parent{

	Client(char* ip,int port):Parent(ip,port){

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

class Server : Parent{	

	struct sockaddr_in client_address;

	int new_sock_fd,addrlen = sizeof(client_address);


	Server(char* ip,int port):Parent(ip,port){
		if (listen(sock_fd, 5) < 0){
			perror("listen");
			exit(EXIT_FAILURE);
		}	

		while(new_sock_fd = accept(sock_fd, (struct sockaddr *)&client_address, (socklen_t*)&addrlen)){
			thread_data *t;
			t=(thread_data*)malloc(sizeof(thread_data));
			t->port=ntohs(client_address.sin_port);
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

void* upload_request_handler(void *data){
	thread_data* d = (thread_data*)data;
    int client_sock_fd = d->sockfd;
    string filename,clientMsg,clientIP = getIPfromNetworkByteOrder(d->address);

	int n,SIZE=102,f=0;
	char buffer[SIZE];
	string header;

	int headerReceived=0;
	
	while (1) {
		n = recv(client_sock_fd, buffer, SIZE, 0);
		if (n <= 0)
			break;
		
		header+=string(buffer);
		bzero(buffer, SIZE);
	}

	pair<int,string> temp = deserializeSeqNumAndFilePath(header);

	cout << temp.first << "-" << temp.second << "-" << clientIP << endl;

	return NULL;
}

void* uploadToServer(void* pieceInfo){

	int piece = *(int*)pieceInfo;

	int client_sock_fd = 0, valread,opt = 1;;
	struct sockaddr_in serv_addr,client_addr;

	/* connecting to server with hardcoded IP and well-defined port */


	string header = serializeSeqNumAndSHA(piece,fileIndex["Ref Bit Torrent.pdf"][piece].second);
	cout << "header for piece " << piece << " is " << header << endl;
	
	char* temp=(char*)malloc((46+SIZE)*sizeof(char));
	strcpy(temp,header.c_str());
	strcat(temp,fileIndex["Ref Bit Torrent.pdf"][piece].second);

	write(client_sock_fd,temp,strlen(temp));

	free(temp);

	close(client_sock_fd);
	return NULL;
}

void uploadFileWithGivenFileNameToServer(string fileName){
	int actually_read,i,n=0;
	vector<char*> chunkPtrs;
	char data[SIZE] = {0};

	// ----------- Indexing of file --------------

	int fd = open(fileName.c_str(),O_RDONLY);

	while((actually_read = read(fd,data, SIZE)) > 0) {
		char *temp = (char*)malloc(SIZE*sizeof(char)); // optimize here if possible
		strcpy(temp,data);
		fileIndex[fileName].push_back(make_pair(n++,temp));
		chunkPtrs.push_back(temp);
	}

	close(fd);

	// ----------- Indexing of file --------------

	pthread_t workerThreads[n];


	for(i=0;i<n;i++){
		int *piece = (int*)malloc(sizeof(int));
		*piece=i;
		pthread_create( &workerThreads[i] , NULL ,  uploadToServer , (void*)piece);
	}

	for(i=0;i<n;i++)
		pthread_join(workerThreads[i],NULL);
	
	for(i=0;i<n;i++)
		free(chunkPtrs[i]);

	cout << "file upload from client done" << endl;
	
}



int main(int argc, char const *argv[])
{
	ip = string(argv[1]);
    uploadFileWithGivenFileNameToServer("Ref Bit Torrent.pdf");
	return 0;
}