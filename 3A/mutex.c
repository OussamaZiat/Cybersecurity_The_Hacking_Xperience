/*
 * mutex.c
 *
 *  Created on: Mar 19, 2016
 *      Author: jiaziyi
 */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


#define NTHREADS 50
void *increase_counter(void *);
pthread_mutex_t lock;

int  counter = 0;

int main()
{
	pthread_t thread_numbers[NTHREADS];
	pthread_mutex_init(&lock, NULL);
	for (int i=0; i<NTHREADS;i++){
		if(pthread_create(&thread_numbers[i], NULL, increase_counter, NULL)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    	}	
	}
	for (int i=0;i<NTHREADS;i++){
		if(pthread_join(thread_numbers[i], NULL)){
			fprintf(stderr, "Failed to terminate thread\n");
			return 1;
		}
	}
	


	printf("\nFinal counter value: %d\n", counter);
	pthread_mutex_destroy(&lock);
}

void *increase_counter(void *arg)
{
	pthread_mutex_lock(&lock);            // START critical region
	printf("Thread number %ld, working on counter. The current value is %d\n", (long)pthread_self(), counter);
	int i = counter;
	//usleep (10000); //simulate the data processing
	counter = i+1;
	pthread_mutex_unlock(&lock);   
	return NULL;       // END critical region
}
