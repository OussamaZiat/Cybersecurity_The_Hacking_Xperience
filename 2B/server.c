#include <stdio.h>   
#include <sys/socket.h> 
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

int MAX_LEN_MSG=1000;

int main(int argc,char* argv[]){

if (argc<2){
        fprintf(stderr, "Missing argument, Please enter the IP address and the port number\n");
        return 1;
    }

// get the port argument
int port=atoi(argv[1]);


//set up a UDP socket on the server side and binds it to an IP addr and a port number
//=================================================================================
int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//==> the AF_INET for the IPv4 protocol
//==>SOCK_DGRAM : A UDP socket

struct sockaddr_in server_addr;
//==> a struct used to store the Ip addr and the port number

memset(&server_addr,0,sizeof(server_addr));
//==> fills the struct with 0s which initialize all the struct fields to 0

server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(port);
server_addr.sin_addr.s_addr = INADDR_ANY;


bind(sockfd, (const struct sockaddr *) &server_addr, sizeof(struct sockaddr));
//==> this line binds the sockfd to to the sockaddr_in IP addr and the port number
//==> we should always cast the struct to "a struct sockaddr" while using the bind 
//====================================================================================&


//store the address of the client that sends data to the server.
//============================================================================
struct sockaddr_in from; // set "from" (client)
//==> store the addr of the client that sends data to the server

memset(&from,0,sizeof(from));
//==> fills the struct with 0s which initialize all the struct fields to 0

socklen_t fromlen = sizeof(struct sockaddr);

//define a buffer with a size that does not exceed MAX_LEN_MSG
char buffer [MAX_LEN_MSG];
char *buf = buffer; //*buf stock info received
//============================================================================

//create a loop that receives data from the client and resend to the same client
//we end the loop when the msg received is "Ze end"
while (1) {
        int recv_len = recvfrom(sockfd, buf, MAX_LEN_MSG, 0, (struct sockaddr*) &from, &fromlen);
        //Listen to message and store it in a buf
        //==>we added the address of from so it can store the addr of the client that send the msg
        
        printf("Size of data successfully received : %d\n", recv_len);


        //Resent to "from" the message stored in buf
        int resent_size = sendto(sockfd, buf, recv_len, 0, (struct sockaddr*) &from, sizeof(struct sockaddr));
        printf("Resent size: %d\n", resent_size);
    }








return 0;
}