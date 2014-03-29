/* ===========================================================================
 *
 *      Filename:  Reservations.c
 *
 *       Created:  03/26/2014
 *        Author:  William Ersing
 *
 *   Description:  Reservations is a room reservation system for the TCNJ
 *  			   library. It is multithreaded (using POSIX), and threads
 *  			   are handles by a scheduling algorithm.
 *
 * ===========================================================================
 */

//#include "Reservations.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>	// needed for pthreads

#define MAX_THREADS 10	// maximum number of threads

// Global variables
pthread_mutex_t mutex1;
int pth_count = 0;

// This is the thread function
void *reserveFunc(void *arg){
	pthread_mutex_lock(&mutex1);
	printf("I am thread %d\n", pth_count);
	pth_count++;
	pthread_mutex_unlock(&mutex1);
	pthread_exit(NULL);
}

int main(void){
	pthread_mutex_init(&mutex1, NULL);
	pthread_t pth[MAX_THREADS];
	int j, k, returnCode;
	for(j = 0; j < MAX_THREADS; j++){
		printf("In main: creating thread %d\n", j);
		returnCode = pthread_create(&pth[j], NULL, reserveFunc, NULL);
		if(returnCode){
			printf("ERROR: return code from pthread_creat is %d\n", returnCode);
			exit(-1);
		}
	}

	for(k = 0; k < MAX_THREADS; k++){
		pthread_join(pth[k], NULL);
	}

	printf("This is main saying goodbye!\n");

	pthread_mutex_destroy(&mutex1);
	pthread_exit(NULL);
}
