/*
 * rawip_example.c
 *
 *  Created on: May 4, 2016
 *      Author: jiaziyi
 */

//highly inspired from : https://www.binarytides.com/raw-udp-sockets-c-linux/
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "header.h"

#define SRC_IP  "192.168.1.111" //set your source ip here. It can be a fake one
#define SRC_PORT 54321 //set the source port here. It can be a fake one

#define DEST_IP "129.104.89.108" //set your destination ip here
#define DEST_PORT 5555 //set the destination port here
#define TEST_STRING "test data" //a test string as packet payload

extern struct sockaddr_in source,dest;
extern FILE *logfile;

int main(int argc, char *argv[])
{
	char source_ip[] = SRC_IP;
	char dest_ip[] = DEST_IP;


	int fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if(fd < 0) 
	{
		//socket creation failed, may be because of non-root privileges
		perror("Failed to create raw socket");
		exit(1);
	}
    int hincl = 1;                  /* 1 = on, 0 = off */
    setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &hincl, sizeof(hincl));

	if(fd < 0)
	{
		perror("Error creating raw socket ");
		exit(1);
	}

	char packet[65536], *data;
	char data_string[] = TEST_STRING;
	memset(packet, 0, 65536);

	//IP header pointer
	struct iphdr *iph = (struct iphdr *)packet;

	//UDP header pointer
	struct udphdr *udph = (struct udphdr *)(packet + sizeof(struct iphdr));
	struct pseudo_udp_header psh; //pseudo header

	//data section pointer
	data = packet + sizeof(struct iphdr) + sizeof(struct udphdr);

	//fill the data section
	strncpy(data, data_string, strlen(data_string));

//====================================================================================
	printf("===========IP HEADER==========\n\n");
	//fill the IP header here
	/*
	members of struct iphdr:

	==> unsigned int ihl - This represents the length of the IP header in 32-bit words.
    ==> unsigned int version - This represents the version of the IP protocol (should be 4 for IPv4).
    ==> u_int8_t tos - This represents the type of service (TOS) field in the IP header.
    ==> unsigned short int tot_len - This represents the total length of the IP packet (header and data).
    ==> unsigned short int id - This represents the identification field of the IP header.
    ==> unsigned short int frag_off - This represents the fragment offset field of the IP header.
    ==> u_int8_t ttl - This represents the time to live (TTL) field in the IP header.
    ==> u_int8_t protocol - This represents the protocol field in the IP header (e.g. TCP, UDP, ICMP).
    unsigned short int check - This represents the header checksum field of the IP header.
    unsigned int saddr - This represents the source IP address of the packet.
    unsigned int daddr - This represents the destination IP address of the packet.
	*/

	iph->ihl=5; //moodle page
	iph->version=4;
	iph->tos=0; //type of service
	iph->tot_len = sizeof (struct iphdr) + sizeof (struct udphdr) + strlen(data); // inspired from the website evoked 
	iph->id= 0 ; //id of this packet
	iph->frag_off = 0;
	iph->ttl = 255; // time to live, the value was taken from the website
	iph->protocol = 17; // for UDP in iana.org
	iph->check = checksum((unsigned short *) packet, iph->tot_len);// inspired from the website
	// iph->saddr = inet_addr(source_ip );
	// iph->saddr = inet_addr(dest_ip );
	if (inet_pton(AF_INET, dest_ip, &(iph->daddr)) != 1) {
		printf("Failed to convert destination ip address\n\n");
	}
	if (inet_pton(AF_INET, source_ip, &(iph->saddr)) != 1) {
		printf("Failed to convert source ip address\n\n");
	}
	printf("=========IP HEADER FILLED========\n\n");

//====================================================================================

//====================================================================================
	//fill pseudo header 
	printf("===========PSEUDO HEADER==========\n\n");
	if (inet_pton(AF_INET, dest_ip, &(psh.dest_address)) != 1) {
		printf("Failed to convert destination ip address ( pseudo header ) \n\n");
	}
	if (inet_pton(AF_INET, source_ip, &(psh.source_address)) != 1) {
		printf("Failed to convert source ip address ( pseudo header )\n\n");
	}
	psh.placeholder = 0;
	psh.protocol = 17; // UDP
	psh.udp_length = htons(sizeof(struct udphdr) + strlen(data));
	printf("=========PSEUDO HEADER FILLED========\n\n");
//====================================================================================

//====================================================================================
	printf("===========UDP HEADER==========\n\n");
	//fill the UDP header

	/*
	The members of the udphdr struct are:

    ==> source: a 16-bit unsigned integer representing the source port number.
    ==> dest: a 16-bit unsigned integer representing the destination port number.
    ==> len: a 16-bit unsigned integer representing the length of the UDP header and data in bytes.
    ==> check: a 16-bit unsigned integer representing the UDP checksum (optional, set to zero if not used).
	*/

	udph->source = htons(SRC_PORT);
	udph->dest = htons(DEST_PORT);
	udph->len = htons(sizeof(struct udphdr) + strlen(data));
	udph->check = 0;


	// this part is literally a copy paste 
	// sadly could not understand 
	int psize = sizeof(struct pseudo_udp_header) + sizeof(struct udphdr) + strlen(data);
	char *pseudogram = malloc(psize);
	memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_udp_header));
	memcpy(pseudogram + sizeof(struct pseudo_udp_header) , udph , sizeof(struct udphdr) + strlen(data));
	udph->check = checksum((unsigned short*) pseudogram, psize);
	printf("=========UDP HEADER FILLED========\n\n");

//====================================================================================
//====================================================================================
	printf("===========START SENDING==========\n\n");
	//send the packet

	struct sockaddr_in dest; //where we stock the sestination info 
    memset(&dest,0,sizeof(dest));
    dest.sin_family = AF_INET; //IPv4
    dest.sin_port = htons(DEST_PORT); // destiantion port number 
	inet_pton(AF_INET, dest_ip, &(dest.sin_addr)); //destiantion ip address


	int sent_len = sendto(fd, packet, iph->tot_len, 0, (struct sockaddr *) &dest, sizeof(struct sockaddr));

	printf("===========END SENDING==========\n\n");

	return 0;

}
