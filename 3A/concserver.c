#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>


/*
 
*We start by creating our server socket 
*we create a sockaddr_in where we store the port number
*we bind them

*the receiving loop
*when we end it ?? ==> ?
==> prepare the from sockadrr_in where we stock the client info 
==> prepare the msg buff 
==> receive the msg 
==> where do we use the lock ?? ==> ask this qst balizz

*we count the active thread(How??)
==> if it is less than N we continue the process(resending the message)
==> else resend the message where we tell the client that the sever is too busy

*if we are using a thread we need to define a function
==> what to name it(resend_message)
==>just send it 
==> variables that we need:
	=> the from sockaddr_in
	=> the server socket
	=> the message buffer 
	=> we can (will) add the length buf to verify if the message was sent entirly)
==> we create a struct where we store the 4 variables

**Rqs:
	=> we should increment the active_th counter when we create
	the threand but also decrement it when we end the resend_message
	=>don't forget to free buffers when created


*/

#define N_THREADS 50

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int active = 0;

typedef struct server_args{
	int socket;
	struct sockaddr_in *from;
	int msg_buf_len;
	char* msg_buf;
} server_args;

void *resend_message(void *var);



int main(int argc, char* argv[]){
    if (argc < 2){
        fprintf(stderr, "Missing argument, pleasse enter the the port number\n\n");
		return 1;
	}

	printf("starting the server...\n");
	// get the server's port number 
	int port = atoi(argv[1]);

	int server_socket= socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	for (int k = 0; k < 8; k++) server_addr.sin_zero[k] = 0;
	
	
	if (bind(server_socket, (struct sockaddr*) &server_addr, sizeof(struct sockaddr))!=0){
		fprintf(stderr, "binding problem!!!\n");
	}

	printf("sever started.\n Waiting the client messages\n\n");


	while (1){


		struct sockaddr_in *from = malloc(sizeof(struct sockaddr_in));
		int *from_len;
		int temp = (int) sizeof(struct sockaddr);
		from_len = &temp;


		int msg_buffer_len = 1000;
		char *msg_buffer = malloc(msg_buffer_len * sizeof(char));
		memset(msg_buffer, 0, 1000);		

		//the main thread, the other will be defined while sending this message
		//didn't create one 
		int received_msg_len = recvfrom(server_socket, msg_buffer, msg_buffer_len, 0, (struct sockaddr*) from, from_len);

		pthread_mutex_lock(&mutex);
		printf("===============================================================\n");
		printf("[START] %d is receiving the message \n",pthread_self());
		printf("the length of the received message is: %d\n", received_msg_len);
		printf("received message: %s", msg_buffer);
		printf("==============================\n");
		pthread_mutex_unlock(&mutex);		

	


	if (active<N_THREADS){
		// printf("%d\n", active);
		server_args var;
		var.socket=server_socket;
		var.msg_buf=msg_buffer;
		var.msg_buf_len=received_msg_len;
		var.from =from;

		active++;			
		pthread_t thread;
		if (pthread_create(&thread, NULL, resend_message, &var)) {
			fprintf(stderr, "Failed to create thread\n");
			return 1;
		}

	} else{
		char warning[] = "the server is too busy...\n";
		char *w = warning;
		sendto(server_socket, w, strlen(w), 0, (struct sockaddr*) from, sizeof(struct sockaddr));
	}

	}
}


void *resend_message(void *var){
	server_args V = *((server_args *) var);

	usleep(1000);
	int sent_length = sendto(V.socket, V.msg_buf, V.msg_buf_len, 0, (struct sockaddr*) V.from, sizeof(struct sockaddr));

	if (sent_length != V.msg_buf_len) {
		printf("Message not sent entirely\n\n");
	} else {
		//access to the buffer needs the a lock 
		pthread_mutex_lock(&mutex);
		printf("[END] %d is resending the message\n",pthread_self());
		printf("Sent back message:\n%s\n", V.msg_buf);
		pthread_mutex_unlock(&mutex);

		active--;
	}
	return NULL;
}