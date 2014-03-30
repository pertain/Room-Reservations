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
#define NUM_DAYS 30
#define NUM_HOURS 15
#define NUM_ROOMS 26

struct room{
	int roomNum;
	int size;					// 0 = small; 1 = standard; 2 = large
	int status;					// 0 = available; 1 = unavailable
	int requesterID;			// default = -1
	char requesterEmail[256];	// default = ""
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
	int returnCode, i, j, k;
	int numTh = 0;
	char term;
	//struct room rooms[NUM_ROOMS];
	struct room calender[NUM_DAYS][NUM_HOURS][NUM_ROOMS];
	pthread_mutex_init(&mutex1, NULL);
	pthread_t *pth;
	pth = malloc(MAX_THREADS * sizeof(int *));
	if(pth == NULL){
		printf("ERROR: memory could not be allocated!\n");
		return 1;
	}

	/*
	for(i = 0; i < NUM_DAYS; i++){
		for(j = 0; j < NUM_HOURS; j++){
			calender[i][j][NUM_ROOMS] = {{109, 0, 0, -1, ""}, {110, 1, 0, -1, ""}, {111, 0, 0, -1, ""},
							  {202, 1, 0, -1, ""}, {205, 1, 0, -1, ""}, {220, 2, 0, -1, ""},
							  {224, 2, 0, -1, ""}, {225, 1, 0, -1, ""}, {226, 1, 0, -1, ""},
							  {228, 1, 0, -1, ""}, {301, 1, 0, -1, ""}, {308, 1, 0, -1, ""},
							  {309, 1, 0, -1, ""}, {310, 1, 0, -1, ""}, {311, 1, 0, -1, ""},
							  {315, 1, 0, -1, ""}, {316, 1, 0, -1, ""}, {317, 1, 0, -1, ""},
							  {319, 1, 0, -1, ""}, {404, 0, 0, -1, ""}, {406, 1, 0, -1, ""},
							  {411, 1, 0, -1, ""}, {412, 1, 0, -1, ""}, {413, 2, 0, -1, ""},
							  {414, 1, 0, -1, ""}, {415, 1, 0, -1, ""}};
		}
	}
	*/

	struct room rooms[NUM_ROOMS] = {{109, 0, 0, -1, ""}, {110, 1, 0, -1, ""}, {111, 0, 0, -1, ""},
									{202, 1, 0, -1, ""}, {205, 1, 0, -1, ""}, {220, 2, 0, -1, ""},
									{224, 2, 0, -1, ""}, {225, 1, 0, -1, ""}, {226, 1, 0, -1, ""},
									{228, 1, 0, -1, ""}, {301, 1, 0, -1, ""}, {308, 1, 0, -1, ""},
									{309, 1, 0, -1, ""}, {310, 1, 0, -1, ""}, {311, 1, 0, -1, ""},
									{315, 1, 0, -1, ""}, {316, 1, 0, -1, ""}, {317, 1, 0, -1, ""},
									{319, 1, 0, -1, ""}, {404, 0, 0, -1, ""}, {406, 1, 0, -1, ""},
									{411, 1, 0, -1, ""}, {412, 1, 0, -1, ""}, {413, 2, 0, -1, ""},
									{414, 1, 0, -1, ""}, {415, 1, 0, -1, ""}};

	for(i = 0; i < NUM_DAYS; i++){
		for(j = 0; j < NUM_HOURS; j++){
			memcpy(&calender, rooms, sizeof(rooms));
		}
	}


	
	for(i = 0; i < NUM_DAYS; i++){
		for(j = 0; j < NUM_HOURS; j++){
			for(k = 0; k < NUM_ROOMS; k++){
				printf("\nThis is the contents of rooms[%d]\n\n", i);
				printf("Room: %d\nSize: %d\nStatus: %d\nRequesterID: %d\nEmail: %s\n\n", rooms[k].roomNum, rooms[k].size, rooms[k].status, rooms[k].requesterID, rooms[k].requesterEmail);
			}
		}
	}

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
