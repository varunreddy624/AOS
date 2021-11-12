# SOCKET PROGRAMMING

Sockets are basically addresses or post boxes for nodes in a network. This is a breakdown of a basic program to communicate one message between a client and server. Sockets (or post boxes in simple terms) need to be set up on both ends each with their own addresses and buffers.

## SERVER SIDE

#### Main steps:

**1. Create a socket -> Get a file descriptor for server socket. (See: man 2 socket)**

```
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd<0){
	perror("Socket creation error");
	exit(1);
}
```

Socket function: Returns file descriptor for socket. Accepts 3 parameters (in order with above example): Address family (AF\_INET means IPV4), Type of connection (SOCK\_STREAM means sequenced and connection based) and protocol (Chosen based on connection type. For most cases, there is only one protocol per connection type, so choosing 0 like now suffices. Here it means TCP).

**2. Define port number for server and create a buffer for communication**

```
char buffer[256]; //256 byte buffer
int portno;
```

Port no. can be hardcoded or can be specified via some form of input

**3. Define server address**

```
struct sockaddr_in server_addr; 

memset((char *) &server_addr, '\0', sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = INADDR_ANY;
server_addr.sin_port = htons(portno);
```

struct sockaddr_in is defined as follows, the important part is that it stores the IP address and port number as address. in\_addr structure stores the IP address.


```
struct sockaddr_in {
	sa_family_t	sin_family;	/* address family: AF_INET */
	in_port_t	sin_port;	/* port in network byte order */
	struct in_addr	sin_addr;	/* internet address */
};

struct in_addr{
	uint32_t	s_addr;	/* address in network byte order */
}
```

memset is being used to make the structure completely empty. The address family AF\_INET used during socket creation is reused here. INADDR\_ANY is a macro to set basically any available IP address, although for our purposes we will likely have to provide an IP address of our own. INADDR\_ANY gives 127.0.0.1 in most cases.

The htons function converts the given port number to network byte order. (See: https://stackoverflow.com/questions/19207745/htons-function-in-socket-programing) Any port number above 1024 is fair game.

Also declare a client address, no need to fill it, we are working on server side right now. The server will RECEIVE the client address via some means and fill this in.

```
struct sockaddr_in client_addr; //Declare
clilen = sizeof(client_addr); //Get size
memset((char *) &client_addr, '\0', sizeof(client_addr)); //Empty
```


**4. Bind the server address to the server socket. (See: man 2 bind)**

We have a socket for the server to send/receive messages, we have also defined an address for it. Now we need to bind them together to complete our server's place in the network. This is done with the bind function.

```
if((bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr))<0)){
	perror("Socket binding error");
	exit(1);
}
```

Bind function accepts 3 paramaters, fd of socket we made, address to bind it to (of type struct sockaddr. We made struct sockaddr_in so we need to typecast it), and the size of the address structure. The end result is that the socket is bound with an address.

**5. Listen for the client message. (See: man 2 listen)**

Now that the socket is created, it needs to listen for any possible messages from the client. 

```
listen(sockfd, 3);
```

Sockfd is to be used purely for accepting connections from clients. That is what listen marks it as. If the socket is already catering to a client and another client requests for a connection, that client is added to a queue. The max size of the queue is defined by the second integer parameter, 3 in this case. So if more than 3 clients were to request a connection at the same time, the first three will be added to the queue and the later ones would be rejected.

**6. Accept connection (See: man 2 accept)**

```
client_sockfd = accept(sockfd, (struct sockaddr *) &client_addr, &clilen); 
if (client_sockfd <= 0){
	perror("Connection accept error");
	exit(1);
}
```

The general order of things here is: 
1. listen function will give a queue of clients requesting a connection.
2. accept will look at the queue associated with the server socket (sockfd) and pick the top one.
3. It will fetch the address of the client from the request and store it to the `client_addr` structure that we created in step 3. The known length `clilen` will be provided for this too.
4. It will return the socket file descriptor used by the client.
	
**7. Read from/write to client**

Now that the connections have all been estabilished, all that is left is to treat the sockets as we would any other files and have the server read/write from/to them accordingly.

All communication to and from the client will be done via the `client_sockfd`. The original `sockfd` was only to manage client connections, this one is to send/receive data with the clients.

General code to exchange a single set of messages with the client given above information is as shown below. Here, one message is wrritten from client side and sent to server, the server reads it and sends an acknowledgement message to client.

```
memset(buffer, '\0', sizeof(buffer));		//Empty the buffer: If it is being reused, this is good practice
	
int n = read(client_sockfd, buffer, 255);	//Read information sent by client and store it in buffer
if (n < 0){
	perror("Error while reading");
	exit(1);
}
printf("Message received: %s\n", buffer);	//Pring received message

n = write(client_sockfd, "I got your message", 19);	//Write a confirmation message to client, they will read this on their end
if (n < 0){
	perror("Error while writing");
	exit(1);
}
printf("Message sent\n");	//Confirmation that message was sent successfully
```


**8. Once all necessary operations with a socket are done, it can be closed with close(sockfd) just like any other file.**

## CLIENT SIDE

#### Main steps:

**1. Create a socket -> Get a file descriptor for the client socket (just like in server side)**

```
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd<0){
	perror("Socket creation error");
	exit(1);
}
```

**2. Define port number for server and create a buffer for communication, just like server side**

```
char buffer[256]; //256 byte buffer
int portno;
```

Port no. can be hardcoded or can be specified via some form of input. Port no. should be the same as that used by server.

**3. Get information about the server that client wants to connect to.** 

First, initialize a sockaddr\_in structure to store server details, just like in step 3 of server side. The client does not need to store any information about its own address, so client\_addr does not need to be made here.

Next, we must know two things about the server to connect to it: port number and ip address. Port number was estabilished in step 2. IP address needs to be known before hand as well, just like port number. 

To get server information via IP address, the gethostbyname function is used.

```
struct hostent *server;
char* ipaddress;

server = gethostbyname(ipaddress);
if (server == NULL){
	perror("No such host exists");
	exit(1);
}
```

The gethostbyname function will accept an ip address as parameter and if there is an active server here, will store the information in a hostent structure of the following form.
(See: man gethostbyname)

```
struct hostent {
	char	*h_name;		/* official name of host */
	char	**h_aliases;		/* alias list */
	int	h_addrtype;		/* host address type */
	int	h_length;		/* length of address */
	char	**h_addr_list;		/* list of addresses */
}
#define h_addr h_addr_list[0]		/* for backward compatibility */
```

The important part here is that for an IPV4 address name, we will simply get one entry in `h_addr_list` that corresponds to the server address. `h_addr` is an alias defined to refer to this one entry. 

This information can be used to populate a local struct sockaddr\_in for server.

```
struct sockaddr_in server_addr;

memset((char *) &server_addr, '\0', sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(portno);

bcopy((char *)server -> h_addr, (char *)&server_addr.sin_addr.s_addr, server -> h_length); //BCopy copies address from hostent structure to sockaddr_in structure, there might be other ways to do this
```

With this, all necessary information regarding the server has been gathered in a desired form (struct sockaddr_in)

**FUN FACT: gethostbyname is apparently deprecated ever since 2008 according to the linux manual for this function. Do we care? Nah.**

**4. Connect with the server (See: man connect)**

Client has all server information, now a connection needs to be estabilished. That is done with the connect system call.

```
if(connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
	perror("Error connecting");
	exit(1);
}
```

This syscall requests a connection from the server. If granted, the server specified in the second argument will now communicate with the client via the socket provided in the first parameter. 

**5. Read from/write to server**

Now that the connection is created and a medium for communication (the client socket) has been made as well, we can treat the client socket as a normal file and read/write from/to it to communicate with the server.

The client side counterpart for the code demonstrated in step 7 of server side is as shown below. It will take a single word input from the terminal, write it to the server and then receive a confirmation message from it for that one message as well.

```
printf("Please enter the message: \n");
memset(buffer, '\0', 256);	\\Empties buffer
scanf("%s", buffer);		\\Takes one word input

n = write(sockfd, buffer, strlen(buffer));	\\Writes to sockfd. This sockfd here is known as client_sockfd to the server. 
if (n<0) {
	perror("Error writing");
	exit(1);
}

memset(buffer, '\0', 256);	\\Empties buffer

n = read(sockfd, buffer, 255);	\\Read from the buffer now that server must have written to it, contains confirmation messsage from server that message has been received
if (n < 0){
	perror("Error reading");
	exit(1);
}

printf("Message received: %s\n", buffer); \\Print confirmation message from buffer
```

**6. Close socket**

If the client is done with its work, it can close its socket using `close(sockfd)` just like a normal file.

