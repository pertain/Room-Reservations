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

#include <stdlib.h>		// needed for: malloc(), realloc(), free(), exit()
#include <stdio.h>
#include <pthread.h>	// needed for pthreads
#include <string.h>		// needed for strcpy()

#define MAX_THREADS 10	// maximum number of threads

struct room{
	int roomNum;
	int size;
	int status;
	int requesterID;
	char requesterEmail[256];
};

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
	/* this approach implements a dynamic array of threads */
	struct room rooms[26];
	int returnCode, j, k;
	int numTh = 0;
	char term;
	pthread_mutex_init(&mutex1, NULL);
	pthread_t *pth;
	pth = malloc(MAX_THREADS * sizeof(int *));
	if(pth == NULL){
		printf("ERROR: memory could not be allocated!\n");
		return 1;
	}

	/* this section is for testing the room struct */
	rooms[0].roomNum = 404;
	rooms[0].size = 0;
	rooms[0].status = 0;
	rooms[0].requesterID = 123456;
	strcpy(rooms[0].requesterEmail,  "something@somewhere.com");
	printf("\nThis is the contents of rooms[0]\n\n");
	printf("Room: %d\nSize: %d\nStatus: %d\nRequesterID: %d\nEmail: %s\n\n", rooms[0].roomNum, rooms[0].size, rooms[0].status, rooms[0].requesterID, rooms[0].requesterEmail);

	printf("\nThis is the contents of rooms[1]\n\n");
	printf("Room: %d\nSize: %d\nStatus: %d\nRequesterID: %d\nEmail: %s\n\n", rooms[1].roomNum, rooms[1].size, rooms[1].status, rooms[1].requesterID, rooms[1].requesterEmail);
	/* end of testing of room struct - build on this */

	/* user inputs int for number of threads to create */
	printf("\nHow many threads do you want?\n");
	printf("Enter an int between 1 and 10\n");
	printf("Choice: ");
	fflush(stdout);

	if(scanf("%d%c", &numTh, &term) != 2 || term != '\n' || numTh < 0 || numTh > MAX_THREADS){
		printf("\nInvalid input\n\n");
		return 0;
	}
	
	printf("\n");
	for(j = 0; j < numTh; j++){
		printf("In main: creating thread %d\n", j);
		returnCode = pthread_create(&pth[j], NULL, reserveFunc, NULL);
		if(returnCode){
			printf("ERROR: return code from pthread_creat is %d\n", returnCode);
			return -1;
		}
	}

	for(k = 0; k < numTh; k++){
		pthread_join(pth[k], NULL);
	}

	printf("This is main saying goodbye!\n\n");

	free(pth);
	pthread_mutex_destroy(&mutex1);
	pthread_exit(NULL);
}
