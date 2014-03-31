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
#include <stdio.h>		// needed for IO
#include <pthread.h>	// needed for pthreads
#include <string.h>		// needed for strcpy()

#define MAX_THREADS 10	// maximum number of threads
#define NUM_DAYS 30		// 30 day period for proof of concept
#define NUM_HOURS 15	// 15 hours per day when reservations are valid
#define NUM_ROOMS 26	// 26 rooms that can be reserved

/* represents a library room - holds info about the availability of a room */
struct room{
	int roomNum;				// the room's number
	int size;					// 0 = small; 1 = standard; 2 = large
	int status;					// 0 = available; 1 = unavailable
	int requesterID;			// default = -1
	char requesterEmail[256];	// default = ""
};

// Global variables
pthread_mutex_t mutex1;
int pth_count = 0;				// used to identify a thread

// This is the thread function
void *reserveFunc(void *arg){
	pthread_mutex_lock(&mutex1);
	printf("I am thread %d\n", pth_count);
	pth_count++;
	pthread_mutex_unlock(&mutex1);
	pthread_exit(NULL);
}

int main(void){
	int returnCode, i, j, k;								// loop iterator ints
	int numTh = 0;											// initializer for number of threads
	int inID, inRmNum;
	char inEmail[256];
	char term;												// terminating char for input
	struct room calender[NUM_DAYS][NUM_HOURS][NUM_ROOMS];	// 3D array of days, hours and rooms
	pthread_mutex_init(&mutex1, NULL);						// initialize mutex
	pthread_t *pth;											// initialize dynamic array of threads
	pth = malloc(MAX_THREADS * sizeof(int *));				// allocate memory for thread array
	if(pth == NULL){										// error check for memory allocation
		printf("ERROR: memory could not be allocated!\n");
		return 1;
	}

	/* represents a requesting person - gets passed to a thread */
	struct request{
		int id;
		char email[256];
		int rmRequested;
	} *requests;
	requests = malloc(MAX_THREADS * sizeof(*requests));		// check that this is correct!!!

	/* initializes rooms array with structs for each room in the library */
	struct room rooms[NUM_ROOMS] = {{109, 0, 0, -1, ""}, {110, 1, 0, -1, ""}, {111, 0, 0, -1, ""},
									{202, 1, 0, -1, ""}, {205, 1, 0, -1, ""}, {220, 2, 0, -1, ""},
									{224, 2, 0, -1, ""}, {225, 1, 0, -1, ""}, {226, 1, 0, -1, ""},
									{228, 1, 0, -1, ""}, {301, 1, 0, -1, ""}, {308, 1, 0, -1, ""},
									{309, 1, 0, -1, ""}, {310, 1, 0, -1, ""}, {311, 1, 0, -1, ""},
									{315, 1, 0, -1, ""}, {316, 1, 0, -1, ""}, {317, 1, 0, -1, ""},
									{319, 1, 0, -1, ""}, {404, 0, 0, -1, ""}, {406, 1, 0, -1, ""},
									{411, 1, 0, -1, ""}, {412, 1, 0, -1, ""}, {413, 2, 0, -1, ""},
									{414, 1, 0, -1, ""}, {415, 1, 0, -1, ""}};


	/* initializes each cell in calender[i][j] with rooms array - result is a 3D array of structs */
	for(i = 0; i < NUM_DAYS; i++){
		for(j = 0; j < NUM_HOURS; j++){
			memcpy(&calender, rooms, sizeof(rooms));
		}
	}
	
	/* prints each cell in calender
	for(i = 0; i < NUM_DAYS; i++){
		for(j = 0; j < NUM_HOURS; j++){
			for(k = 0; k < NUM_ROOMS; k++){
				printf("\nThis is the contents of rooms[%d]\n\n", i);
				printf("Room: %d\nSize: %d\nStatus: %d\nRequesterID: %d\nEmail: %s\n\n", rooms[k].roomNum, rooms[k].size, rooms[k].status, rooms[k].requesterID, rooms[k].requesterEmail);
			}
		}
	}
	*/

	/* user inputs int for number of threads to create - this simulates concurrent user requests */
	printf("\nHow many threads do you want?\n");
	printf("Enter an int between 1 and 10\n");
	printf("Choice: ");
	fflush(stdout);

	/* Error checking for user input - ensures that input is an int between 0 and MAX_THREADS */
	if(scanf("%d%c", &numTh, &term) != 2 || term != '\n' || numTh < 0 || numTh > MAX_THREADS){
		printf("\nInvalid input\n\n");
		return 0;
	}

	/*
	printf("Enter your 6-digit user ID\n");
	if(scanf("%d%c", &inID, &term) != 2 || term != '\n' || sizeof(inID) < 0 || numTh > MAX_THREADS){
		printf("\nInvalid input\n\n");
		return 0;
	}
	*/


	/* fills request array with requests - values come from user input */
	for(i = 0; i < numTh; i++){
	}
	
	printf("\n");

	/* creates numTh threads, where numTh is determined by the user */
	for(i = 0; i < numTh; i++){
		printf("In main: creating thread %d\n", i);
		returnCode = pthread_create(&pth[i], NULL, reserveFunc, NULL);
		if(returnCode){
			printf("ERROR: return code from pthread_create is %d\n", returnCode);
			return -1;
		}
	}

	/* all threads join with main thread - makes main wait for all threads to finish */
	for(i = 0; i < numTh; i++){
		pthread_join(pth[i], NULL);
	}

	printf("This is main saying goodbye!\n\n");

	free(pth);						// free dynamic memory
	pthread_mutex_destroy(&mutex1);	// destroys mutex object
	pthread_exit(NULL);				// exits main thread
}
