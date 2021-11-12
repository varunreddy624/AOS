#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <openssl/sha.h>
#include "../../library.h"


void uploadFileWithGivenFileNameToServer(string fileName, int sockfd){
	int n,SIZE=512*(1<<10),actually_read;
	char data[SIZE] = {0};
	char hash[20]={0};
	int fd = open(fileName.c_str(),O_RDONLY);

	while((actually_read = read(fd,data, SIZE)) > 0) {
		SHA1((const unsigned char*)data,actually_read,(unsigned char*)hash);
    	if (send(sockfd, data, actually_read, 0) == -1) {
      		perror("[-]Error in sending file.");
			close(fd);
      		exit(1);
    	}
    	bzero(data, SIZE);
		bzero(hash, 20);
  	}
	close(fd);
}


int main(int argc, char const *argv[])
{
	int client_sock_fd = 0, valread,opt = 1;;
	struct sockaddr_in serv_addr,client_addr;
	char buffer[1024] = {0};

	if ((client_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("\n Socket creation error \n");
		return -1;
	}

	if (setsockopt(client_sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))){
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	/* binding client with CMD provided IP and well-defined port*/

	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(1818);
	if(inet_pton(AF_INET, argv[1], &client_addr.sin_addr)<=0){
		perror("Invalid address Address not supported");
		exit(EXIT_FAILURE);
	}
	
	
	if (bind(client_sock_fd, (struct sockaddr *)&client_addr,sizeof(client_addr))<0){
		perror("bind failed");
		exit(EXIT_FAILURE);
	}


	/* binding client with CMD provided  IP and well-defined port*/


	/* connecting to server with hardcoded IP and well-defined port */

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8080);
	
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0){
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(client_sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		printf("\nConnection Failed \n");
		return -1;
	}

	/* connecting to server with hardcoded IP and well-defined port */

	printf("Server Port - %d\n",ntohs(serv_addr.sin_port));
	cout << "Server IP: - " << getIPfromNetworkByteOrder(serv_addr.sin_addr.s_addr) << endl;


	uploadFileWithGivenFileNameToServer("Ref Bit Torrent.pdf",client_sock_fd);
	// send(client_sock_fd , "Hello from client" , 17 , 0 );
	// valread = read( client_sock_fd , buffer, 1024);
	// printf("Client: Message received from server is - %s\n",buffer );
	return 0;
}
