#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// from the moodle page: By using two threads (one for receiving server's reply, 
//and another for reading the user message to be sent to the server), 
//the client can simultaneously read a new request, and receive (and show) a request reply from the server.


//what should be in the main function???

//1_extract the Ip address ans the port number from the args

//2_ create our UDP socket
//do we need a port number for the client?? maybe yes
//we create a sockaddr_in where we stock the client info(port number)
//and ofc we bind the client UDP socket and our beloved sockaddr_in
// we will have a full socket then 

//3_ create the dest sockaddr_in where we stock there info extracted from the args

//4_ create a first thread responsible for sending the message
//it takes as arg a sned_message_to_server function
//the args of this function==> idk

//5_ create a second thread responsible for receving the message from the server
// it takes ofc as args a function receive_message_from_server
// args of this function idk 

//for testing we may add a sleep function as id it takes too much time
// that why i will let the main thread empty 

#define CLIENT_PORT 9999
#define MAX_LEN_MSG 1000




pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct client_args{
    struct sockaddr_in addr;
    int socket;
} client_args;

void *send_message(void *);
void *receive_message(void *);


int main(int argc, char* argv[]){
    if (argc < 2){
        fprintf(stderr, "Missing argument, pleasse enter the IP address and the port number\n\n");
    }
    else if (argc < 3){
        fprintf(stderr, "Missing argument, pleasse enter the port number\n\n");
    }
    //1_ define the server server
    char* IP_addr = argv[1];
    int port  = atoi(argv[2]);

    //2_ create ou full socket 
    int client_socket= socket(AF_INET, SOCK_DGRAM, 0);

    //build sockadrr_in where we stock client info( port number)
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(CLIENT_PORT);
    client_addr.sin_addr.s_addr = INADDR_ANY;
	for (int k = 0; k < 8; k++) client_addr.sin_zero[k] = 0;
    //bind
    if (bind(client_socket, (struct sockaddr*) &client_addr, sizeof(struct sockaddr))){
        fprintf(stderr, "binding problem\n\n");
    }

    //3_ build sockadrr_in where we stock server info
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
	if (inet_pton(AF_INET, IP_addr, &(server_addr.sin_addr)) != 1) printf("inet_pton problem\n\n"); 
    // inet_pton(AF_INET, IP_addr, &(server_addr.sin_addr));
    
    
    client_args sock_and_addr;
    sock_and_addr.addr = server_addr;
    sock_and_addr.socket = client_socket;

    pthread_t send_thread;
	if (pthread_create(&send_thread, NULL, send_message, &sock_and_addr)) {
		fprintf(stderr, "Failed to create thread\n");
		return 1;
	}
	
	pthread_t receive_thread;
	if (pthread_create(&receive_thread, NULL, receive_message, &sock_and_addr)) {
		fprintf(stderr, "Failed to create thread\n");
		return 1;
	}
	
	// Main threads does nothing
    // if the main thread ends all the threads are shut down
	while (1) usleep(1000);	
	
	return 0;


}


// sending the message 
// read the message 
//send it 
//verfy if the message was sent entirely
//do not forget to lock the area
void *send_message(void *var){

    client_args V = *((client_args *) var);
    int sock=V.socket;
    // sockaddr_in addr = var->addr;
    while (1){ //we will end the loop when the user enters nothing(use the button enter)
    usleep(1000);
    pthread_mutex_lock(&mutex);

    //get the message from the user
    char message[MAX_LEN_MSG];
	char *msg = message;
	printf("========================================\nPlease input a message to send:\n");
	fgets(message, MAX_LEN_MSG, stdin);
	int msg_len = strlen(msg);// to verify if the message was sent entirely or not
    
    //send the read message
    int sent_len= sendto(sock, msg, msg_len, 0, (struct sockaddr*) &V.addr, sizeof(struct sockaddr) );
    if (sent_len!=msg_len) {
        fprintf(stderr, "the message was not entirely sent!!!!\n\n");

    }
    pthread_mutex_unlock(&mutex);



    }
}

void *receive_message(void *var){
    client_args V = *((client_args *) var);

    int sock=V.socket;
    while (1) {	
	
		// Building addr_in to identify the replying server
		struct sockaddr_in from;
		int *from_len;
		int temp = (int) sizeof(struct sockaddr);
		from_len = &temp;
		
		// Building received message buffer
		int buf_len = 1000;
		char buffer[buf_len];
		char *buf = buffer;
		memset(buf, 0, 1000);
		
		// Receiving message
		int recv_len = recvfrom(sock, buf, buf_len, 0, (struct sockaddr*) &from, from_len);
		pthread_mutex_lock(&mutex);
        printf("========================================\nLength of the received message: %d\n", recv_len);
		printf("Received message: \n%s\n", buf);
		pthread_mutex_unlock(&mutex);
	
	}
    
}
