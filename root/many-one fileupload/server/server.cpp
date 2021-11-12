#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <openssl/sha.h>
#include <pthread.h>
#include "../../library.h"

struct thread_data{
    in_port_t port;
    uint32_t address;
    int sockfd;
};

typedef struct thread_data thread_data;

string filename="recv.pdf";

// int fd = open(filename.c_str(), O_CREAT | O_WRONLY, S_IRWXG | S_IRWXU | S_IRWXO);


map<string,vector<pair<int,string>>> fileIndex;

void *connection_handler(void *data){
    thread_data* d = (thread_data*)data;
    int client_sock_fd = d->sockfd;
    string filename,clientMsg,clientIP = getIPfromNetworkByteOrder(d->address);

	
	// ----------- part to write to a file -------------------

	int n,SIZE=46+(512*(1<<10)),f=0;
	char buffer[SIZE];
	string header,payload;

	int headerReceived=0;
	
	while (1) {
		n = recv(client_sock_fd, buffer, SIZE, 0);
		if (n <= 0)
			break;
		
		string temp = string(buffer);

		if(f==1)
			payload+=temp;
		else{
			if(n+headerReceived>=45){
				header+=temp.substr(0,45-headerReceived);
				f=1;
			}
			else{
				header+=temp;
				headerReceived+=n;
			}
		}
		bzero(buffer, SIZE);
	}
	// ----------- part to write to a file -------------------

	pair<int,string> seqNumAndSHA = deserializeSeqNumAndSHA(header);

	cout << "sha of piece " << seqNumAndSHA.first << " is " << seqNumAndSHA.second << endl;

    free(data);
    return 0;
}

int main(int argc, char const *argv[])
{
	struct sockaddr_in server_address,client_address;
	int server_sock_fd, new_socket_fd, *new_sock,valread,opt = 1,addrlen = sizeof(server_address);
	char buffer[1024] = {0};
	
	if ((server_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	
	if (setsockopt(server_sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))){
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	/* binding server with well-defined IP and port*/

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons( 8080 );
	if(inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr)<=0){
		perror("Invalid address Address not supported");
		exit(EXIT_FAILURE);
	}
	
	
	if (bind(server_sock_fd, (struct sockaddr *)&server_address,sizeof(server_address))<0){
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	/* binding server with well-defined IP and port*/

	if (listen(server_sock_fd, 5) < 0){
		perror("listen");
		exit(EXIT_FAILURE);
	}

    while(new_socket_fd = accept(server_sock_fd, (struct sockaddr *)&client_address, (socklen_t*)&addrlen)){
        thread_data *t;
        t=(thread_data*)malloc(sizeof(thread_data));
        t->port=ntohs(client_address.sin_port);
        t->address=client_address.sin_addr.s_addr;
        t->sockfd=new_socket_fd;

		pthread_t sniffer_thread;
		if(pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*)t)){
			perror("could not create thread");
			return 1;
		}
    }
	return 0;
}
