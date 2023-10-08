#pragma once




#ifndef FILE1_H
#define FILE1_H

#include <pthread.h>

struct pseudo_header{
    unsigned int src_addr;
    unsigned int dest_addr;
    unsigned char reserved;
    unsigned char protocol_id;
    unsigned short tcp_len;
    struct tcphdr tcp_struct;
};

#endif