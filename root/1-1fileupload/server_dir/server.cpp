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
#include "../../library.h"


void write_file(int sockfd){
  int n,SIZE=512*(1<<10),fd;
  char buffer[SIZE];

  fd = open("recv.pdf", O_CREAT | O_WRONLY, S_IRWXG | S_IRWXU | S_IRWXO);

  while (1) {
    n = recv(sockfd, buffer, SIZE, 0);
	cout << n << endl;
    if (n <= 0){
		close(fd);
      	break;
      	return;
    }
    write(fd,buffer,n);
    bzero(buffer, SIZE);
  }
  close(fd);
  return;
}


int main(int argc, char const *argv[])
{
	struct sockaddr_in server_address,client_address;
	int server_sock_fd, new_socket_fd, valread,opt = 1,addrlen = sizeof(server_address);
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

	if (listen(server_sock_fd, 3) < 0){
		perror("listen");
		exit(EXIT_FAILURE);
	}

	if ((new_socket_fd = accept(server_sock_fd, (struct sockaddr *)&client_address,(socklen_t*)&addrlen))<0){
		perror("accept");
		exit(EXIT_FAILURE);
	}
    
	printf("Client Port - %d\n",ntohs(client_address.sin_port));
	cout << "Client IP address - " << getIPfromNetworkByteOrder(client_address.sin_addr.s_addr) << endl;


	write_file(new_socket_fd);
	// valread = read( new_socket_fd , buffer, 1024);
	// printf("Server: Message recevied from client is - %s\n",buffer );
	// send(new_socket_fd , "Hello from server" , 17 , 0 );
	return 0;
}
