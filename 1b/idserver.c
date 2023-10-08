/*
 * idserver.c
 *
 *  Created on: Feb 15, 2016
 *      Author: jiaziyi
 *  Updated: JACF, 2020
 */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "idserver.h"

/**
 * print the server's information
 */
void print_idserver(idserver s)
{
	printf("Id: %s\n", s.id);
	printf("Latency (usec): %d\n", s.latency);
	printf("Region: %s\n", s.region);
	printf("Status: %s\n", s.status);
	printf("Nbr of threads: %d\n", *s.nthreads);
}

/**
 * try to modify the server information
 */
void modify(idserver s, char *id, int latency, char* status)
{
	s.id=id;
	s.latency=latency;
	
	strncpy(s.status, status, strlen(status));
}

/**
 * try to modify the student information using pointer
 */
void modify_by_pointer(idserver *s, char *id, int latency, char status[])
{
	s->id=id;
	s->latency=latency;
	
	strncpy(s->status, status, strlen(status));
}

//=========================================================
// old function
// idserver* create_idserver(char *id, char *region, int latency,
// 		char *status, int *nthreads)
// {

// 	idserver s;
// 	s.id = id;
// 	s.region = region;
// 	s.latency = latency;
// 	strncpy(s.status, status, strlen(status)+1);
// 	s.nthreads = nthreads;
// 	puts("---print inside create_idserver function---");
// 	print_idserver(s);
// 	puts("---end of print inside");
// 	return &s;
// }
// it returns the address of a local variable which will be destructed the moment wwe quit the function
//hence, the solution is to allcate dynamically a memory space in the heap and then create th id sever 
// the function malloc will do the job
//=====================================================

idserver* create_idserver(char *id, char *region, int latency,
		char *status, int *nthreads)
{

	idserver* s=malloc(sizeof(idserver));

	s->id=malloc(10*sizeof(char));
	s->region=malloc(10*sizeof(char));
	s->nthreads=malloc(sizeof(int));
	strncpy(s->status, malloc(10*sizeof(char)), strlen(status)+1);


	s->id = id;
	s->region = region;
	strncpy(s->status, status, strlen(status)+1);
	s->latency=latency;
	s->nthreads = nthreads;

	puts("---print inside create_idserver function---");
	print_idserver(*s);
	puts("---end of print inside");
	return s;
}