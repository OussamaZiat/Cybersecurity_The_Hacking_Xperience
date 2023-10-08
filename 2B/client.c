#include <stdio.h>   
#include <sys/socket.h> 
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

int MAX_LEN_MSG=1000;

int main(int argc,char* argv[]){

    if (argc<3){
        fprintf(stderr, "Missing argument, Please enter the IP address and the port number\n");
        return 1;
    }

    char* ip_addr=argv[1];
    int port = atoi(argv[2]);

    char message[MAX_LEN_MSG];
    char* msg=message;

    int msg_len;
    int sent_len;

    int mysocket = socket(AF_INET, SOCK_DGRAM, 0); 

    struct sockaddr_in dest;
    memset(&dest,0,sizeof(dest));
    dest.sin_port=htons(port);
    dest.sin_family=AF_INET;
    //If the conversion is successful, the function returns a value of 1. 
    //If the conversion fails, the function returns 0 and sets the value of the errno variable to indicate the specific error that occurred.
    if (inet_pton(AF_INET, ip_addr, &(dest.sin_addr))!=1){
        fprintf(stderr, "inet_pton error!!!");        
    }


    while (1){
        printf("please input you message (should not exceed %d character) \n", MAX_LEN_MSG);
        fgets(message, MAX_LEN_MSG, stdin);
        msg_len=strlen(msg);
        if (msg_len == 1) { //'/n'
            printf("PROCESS HAS BEEN ENDED UP.\n");
            return 0;
        };
        
        sent_len = sendto(mysocket, msg, msg_len, 0, (struct sockaddr *) &dest,sizeof(struct sockaddr)); 
        printf("Size of data successfully sent: %d\n", sent_len);        
    }

    //


}