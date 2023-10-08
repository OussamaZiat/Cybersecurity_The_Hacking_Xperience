#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
int parse(int sockfd)
{
  char buf[256] = {0};
  char command[] = "uptime | sed 's/.*up \[^,]*\, .*/\\1/'";
  char greeting_text[128];

    /* 
    The dup2() function duplicates a file descriptor, replacing another file descriptor with the duplicate. 
    In this case, dup2(sockfd, STDOUT_FILENO) duplicates the sockfd file descriptor onto the standard output 
    file descriptor (STDOUT_FILENO), replacing the standard output file descriptor with the socket file descriptor.

    After this duplication, any output that would normally be sent to the console via printf() will instead be sent to 
    the socket connection via the sockfd file descriptor. This allows the server to send output directly to the client 
    over the network connection.
    */
  if (dup2(sockfd, STDOUT_FILENO) < 0) {
    perror("dup2");
  }
  printf("What is your name?\n");


  /*
  fflush(stdout) is a function call that flushes (i.e., writes out) 
  any data that is currently buffered in the standard output stream (stdout).
  
  buffered means that the data is stored in temporary memory buffer 
  */
  fflush(stdout);

  if (recv(sockfd, buf, sizeof(buf), 0) < 0) {
    perror("recv");
    return -1;
  }

  strcpy(greeting_text, "Hello, dear ");
  strcat(greeting_text, buf);
  printf("%s\n", greeting_text);
  printf("This computer has been running for ");
  fflush(stdout);
  system(command);
  fflush(stdout);
  close(sockfd);
  return 0;
}
int main(int argc, char** argv)
{


  if (argc<2) {
  	printf("Syntax: %s <port>\n", argv[0]);
  	exit(1);
  }
//============================================================================
  int listenfd = 0,connfd = 0;

  struct sockaddr_in serv_addr;
  char sendBuff[1024];

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("listen");
    return -1;
  }
  memset(&serv_addr, 0, sizeof(serv_addr));
  memset(sendBuff, 0, sizeof(sendBuff));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(atoi(argv[1]));
  if (bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0) {
    perror("bind");
    return -1;
  }
//============================================================================
//the maximum number of pending connections that can be queued up waiting to be accepted.
  if (listen(listenfd, 10) < 0) {
    perror("listen");
    return -1;
  }

  signal(SIGCHLD, SIG_IGN);
  while(1) {
    if ((connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) < 0) {
      perror("accept");
    }
    /*
    The program then forks a child process to handle the client request,
     while the parent process continues to listen for new connections.
    */
    if (fork() == 0) {
 	parse(connfd);
        return 0;
    } else {
        close(connfd);
    }
  }
  return 0;
}