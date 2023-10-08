#include <stdio.h>   
#include <sys/socket.h> 
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>


const int MAX_LEN_MSG=1000;

int main(int argc,char* argv[]){

if (argc<3){
        fprintf(stderr, "Missing argument, Please enter the IP address and the port number\n");
        return 1;
    }

//========================
char* ip_addr=argv[1];
int port=atoi(argv[2]); // atoi ASCII to integer
//========================


char message[MAX_LEN_MSG];
char *msg = message;

//============================================================================
int sockfd = socket(AF_INET, SOCK_DGRAM, 0);


struct sockaddr_in dest;

memset(&dest,0,sizeof(dest));

dest.sin_family = AF_INET;
dest.sin_port = htons(port);

inet_pton(AF_INET, ip_addr, &(dest.sin_addr)); // Ip adress adding
//==>inet_pton is a function that is used to convert a human-readable IP address in string format to its binary representation. 
//==>The name inet_pton stands for "Internet address presentation to network address conversion". 
//============================================================================


struct sockaddr_in from; 
memset(&from,0,sizeof(from));
socklen_t fromlen = sizeof(struct sockaddr);
char buffer[500];
char *buf = buffer; 

while (1) {
        printf("Enter your message: (To end the process press ENTER)");
        fgets(msg, MAX_LEN_MSG, stdin);
        int len_msg = strlen(msg);

        //to end the process just press enter
        if (len_msg == 1) { //case where there is only '/n'
            printf("PROCESS HAS BEEN ENDED UP.\n");
            return 0;
        };

        int sent_size = sendto(sockfd, msg, len_msg, 0, (struct sockaddr*) &dest, sizeof(struct sockaddr));
        printf("Size of data successfully sent: %d\n", sent_size);


        int received_size = recvfrom(sockfd, buf, 500, 0, (struct sockaddr*) &dest, &fromlen);
        printf("Response size: \n%d\n", received_size);
        printf("Response from the server: \n%s\n", buf);

    };

return 0;
}