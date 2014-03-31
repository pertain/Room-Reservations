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
struct room calender[NUM_DAYS][NUM_HOURS][NUM_ROOMS];	// 3D array of days, hours, and rooms

/* initializes rooms array with structs for each room in the library
struct room rooms[NUM_ROOMS] = {{109, 0, 0, -1, ""}, {110, 1, 0, -1, ""}, {111, 0, 0, -1, ""},
								{202, 1, 0, -1, ""}, {205, 1, 0, -1, ""}, {220, 2, 0, -1, ""},
								{224, 2, 0, -1, ""}, {225, 1, 0, -1, ""}, {226, 1, 0, -1, ""},
								{228, 1, 0, -1, ""}, {301, 1, 0, -1, ""}, {308, 1, 0, -1, ""},
								{309, 1, 0, -1, ""}, {310, 1, 0, -1, ""}, {311, 1, 0, -1, ""},
								{315, 1, 0, -1, ""}, {316, 1, 0, -1, ""}, {317, 1, 0, -1, ""},
								{319, 1, 0, -1, ""}, {404, 0, 0, -1, ""}, {406, 1, 0, -1, ""},
								{411, 1, 0, -1, ""}, {412, 1, 0, -1, ""}, {413, 2, 0, -1, ""},
								{414, 1, 0, -1, ""}, {415, 1, 0, -1, ""}};
								*/

/*
// initializes each cell in calender[i][j] with rooms array - result is a 3D array of structs
for(i = 0; i < NUM_DAYS; i++){
	for(j = 0; j < NUM_HOURS; j++){
		memcpy(&calender, rooms, sizeof(rooms));
	}
}
*/



/* represents a requesting person - gets passed to a thread */
struct request{
	int id;				// user input for user id
	char email[256];	// user input email address
	int day;			// 0-30: represents a month
	int time;			// 0-15 represents the hours 8am-11pm
	int rmRequested;	// user input for room number requested
};

// Global variables
pthread_mutex_t mutex1;	// initialize mutex lock
int pth_count = 0;		// used to identify a thread

// This is the thread function
void *reserveFunc(void *arg){
	struct request inSt = *(struct request *)arg;
	int i, alternateRoom;
	int successful = 0;
	for(i = 0; i < NUM_ROOMS; i++){
		if(calender[inSt.day][inSt.time][i].roomNum == inSt.rmRequested && calender[inSt.day][inSt.time][i].status == 0){
			successful = 1;
			pthread_mutex_lock(&mutex1);
			calender[inSt.day][inSt.time][i].requesterID = inSt.id;
			strcpy(calender[inSt.day][inSt.time][i].requesterEmail, inSt.email);
			calender[inSt.day][inSt.time][i].status = 1;
			printf("CONFIRMATION #%d:\troom (%d) reserved for user (%d) on day (%d) at time (%d)\n", pth_count, calender[inSt.day][inSt.time][i].roomNum, calender[inSt.day][inSt.time][i].requesterID, inSt.day, inSt.time);
			pthread_mutex_unlock(&mutex1);
		}
		else if(calender[inSt.day][inSt.time][i].roomNum != inSt.rmRequested && calender[inSt.day][inSt.time][i].status == 0){
			alternateRoom = i;
		}
	}
	if(successful == 0){
		successful = 1;
		pthread_mutex_lock(&mutex1);
		calender[inSt.day][inSt.time][alternateRoom].requesterID = inSt.id;
		strcpy(calender[inSt.day][inSt.time][alternateRoom].requesterEmail, inSt.email);
		calender[inSt.day][inSt.time][alternateRoom].status = 1;
		printf("CONFIRMATION #%d:\troom (%d) reserved for user (%d) on day (%d) at time (%d)\n", pth_count, calender[inSt.day][inSt.time][alternateRoom].roomNum, calender[inSt.day][inSt.time][alternateRoom].requesterID, inSt.day, inSt.time);
		pthread_mutex_unlock(&mutex1);
	}

	pthread_mutex_lock(&mutex1);
	//printf("I am thread %d\n", pth_count);
	//printf("inSt[%d].id = %d\n", pth_count, inSt.id);
	pth_count++;
	pthread_mutex_unlock(&mutex1);
	pthread_exit(NULL);
}

int main(void){
	int returnCode, i, j, k;								// loop iterator ints
	int numTh = 0;											// initializer for number of threads
	char term;												// terminating char for input
	//struct room calender[NUM_DAYS][NUM_HOURS][NUM_ROOMS];	// 3D array of days, hours and rooms
	struct request *requests;
	requests = malloc(MAX_THREADS * sizeof(*requests));		// check that this is correct!!!
	pthread_mutex_init(&mutex1, NULL);						// initialize mutex
	pthread_t *pth;											// initialize dynamic array of threads
	pth = malloc(MAX_THREADS * sizeof(int *));				// allocate memory for thread array
	if(pth == NULL){										// error check for memory allocation
		printf("ERROR: memory could not be allocated!\n");
		return 1;
	}

	//int nums[26] = {109, 110, 111, 202, 205, 220, 224, 225, 226, 228, 301, 308, 309, 310, 311, 315, 316, 317, 319, 404, 406, 411, 412, 413, 414, 415};

	/* initializes rooms array with structs for each room in the library
	struct room rooms[NUM_ROOMS] = {{nums[0], 0, 0, -1, ""}, {nums[1], 1, 0, -1, ""}, {nums[2], 0, 0, -1, ""},
									{nums[3], 1, 0, -1, ""}, {nums[4], 1, 0, -1, ""}, {nums[5], 2, 0, -1, ""},
									{nums[6], 2, 0, -1, ""}, {nums[7], 1, 0, -1, ""}, {nums[8], 1, 0, -1, ""},
									{nums[9], 1, 0, -1, ""}, {nums[10], 1, 0, -1, ""}, {nums[11], 1, 0, -1, ""},
									{nums[12], 1, 0, -1, ""}, {nums[13], 1, 0, -1, ""}, {nums[14], 1, 0, -1, ""},
									{nums[15], 1, 0, -1, ""}, {nums[16], 1, 0, -1, ""}, {nums[17], 1, 0, -1, ""},
									{nums[18], 1, 0, -1, ""}, {nums[19], 0, 0, -1, ""}, {nums[20], 1, 0, -1, ""},
									{nums[21], 1, 0, -1, ""}, {nums[22], 1, 0, -1, ""}, {nums[23], 2, 0, -1, ""},
									{nums[24], 1, 0, -1, ""}, {nums[25], 1, 0, -1, ""}};
									*/



	/* initializes rooms array with structs for each room in the library
	struct room rooms[NUM_ROOMS] = {{109, 0, 0, -1, ""}, {110, 1, 0, -1, ""}, {111, 0, 0, -1, ""},
									{202, 1, 0, -1, ""}, {205, 1, 0, -1, ""}, {220, 2, 0, -1, ""},
									{224, 2, 0, -1, ""}, {225, 1, 0, -1, ""}, {226, 1, 0, -1, ""},
									{228, 1, 0, -1, ""}, {301, 1, 0, -1, ""}, {308, 1, 0, -1, ""},
									{309, 1, 0, -1, ""}, {310, 1, 0, -1, ""}, {311, 1, 0, -1, ""},
									{315, 1, 0, -1, ""}, {316, 1, 0, -1, ""}, {317, 1, 0, -1, ""},
									{319, 1, 0, -1, ""}, {404, 0, 0, -1, ""}, {406, 1, 0, -1, ""},
									{411, 1, 0, -1, ""}, {412, 1, 0, -1, ""}, {413, 2, 0, -1, ""},
									{414, 1, 0, -1, ""}, {415, 1, 0, -1, ""}};
									*/



	for(i = 0; i < NUM_DAYS; i++){
		for(j = 0; j < NUM_HOURS; j++){
			for(k = 0; k < NUM_ROOMS; k++){
				//calender[i][j][k] =
				switch(k){
					case 0:{
							calender[i][j][k].roomNum = 109;
							calender[i][j][k].size = 0;
							break;
						   }
					case 1:{
							calender[i][j][k].roomNum = 110;
							calender[i][j][k].size = 1;
							break;
						   }
					case 2:{
							calender[i][j][k].roomNum = 111;
							calender[i][j][k].size = 0;
							break;
						   }
					case 3:{
							calender[i][j][k].roomNum = 202;
							calender[i][j][k].size = 1;
							break;
						   }
					case 4:{
							calender[i][j][k].roomNum = 205;
							calender[i][j][k].size = 1;
							break;
						   }
					case 5:{
							calender[i][j][k].roomNum = 220;
							calender[i][j][k].size = 2;
							break;
						   }
					case 6:{
							calender[i][j][k].roomNum = 224;
							calender[i][j][k].size = 2;
							break;
						   }
					case 7:{
							calender[i][j][k].roomNum = 225;
							calender[i][j][k].size = 1;
							break;
						   }
					case 8:{
							calender[i][j][k].roomNum = 226;
							calender[i][j][k].size = 1;
							break;
						   }
					case 9:{
							calender[i][j][k].roomNum = 228;
							calender[i][j][k].size = 1;
							break;
						   }
					case 10:{
							calender[i][j][k].roomNum = 301;
							calender[i][j][k].size = 1;
							break;
						   }
					case 11:{
							calender[i][j][k].roomNum = 308;
							calender[i][j][k].size = 1;
							break;
						   }
					case 12:{
							calender[i][j][k].roomNum = 309;
							calender[i][j][k].size = 1;
							break;
						   }
					case 13:{
							calender[i][j][k].roomNum = 310;
							calender[i][j][k].size = 1;
							break;
						   }
					case 14:{
							calender[i][j][k].roomNum = 311;
							calender[i][j][k].size = 1;
							break;
						   }
					case 15:{
							calender[i][j][k].roomNum = 315;
							calender[i][j][k].size = 1;
							break;
						   }
					case 16:{
							calender[i][j][k].roomNum = 316;
							calender[i][j][k].size = 1;
							break;
						   }
					case 17:{
							calender[i][j][k].roomNum = 317;
							calender[i][j][k].size = 1;
							break;
						   }
					case 18:{
							calender[i][j][k].roomNum = 319;
							calender[i][j][k].size = 1;
							break;
						   }
					case 19:{
							calender[i][j][k].roomNum = 404;
							calender[i][j][k].size = 0;
							break;
						   }
					case 20:{
							calender[i][j][k].roomNum = 406;
							calender[i][j][k].size = 1;
							break;
						   }
					case 21:{
							calender[i][j][k].roomNum = 411;
							calender[i][j][k].size = 1;
							break;
						   }
					case 22:{
							calender[i][j][k].roomNum = 412;
							calender[i][j][k].size = 1;
							break;
						   }
					case 23:{
							calender[i][j][k].roomNum = 413;
							calender[i][j][k].size = 2;
							break;
						   }
					case 24:{
							calender[i][j][k].roomNum = 414;
							calender[i][j][k].size = 1;
							break;
						   }
					case 25:{
							calender[i][j][k].roomNum = 415;
							calender[i][j][k].size = 1;
							break;
						   }
				}
			}
		}
	}

	/*
	// initializes each cell in calender[i][j] with rooms array - result is a 3D array of structs
	for(i = 0; i < NUM_DAYS; i++){
		for(j = 0; j < NUM_HOURS; j++){
			memcpy(&calender, &rooms, sizeof(rooms));
		}
	}
	*/
	
	/* prints each cell in calender */
	for(i = 0; i < NUM_DAYS; i++){
		for(j = 0; j < NUM_HOURS; j++){
			for(k = 0; k < NUM_ROOMS; k++){
				printf("\nThis is the contents of rooms[%d]\n\n", k);
				printf("Room: %d\nSize: %d\nStatus: %d\nRequesterID: %d\nEmail: %s\n\n", calender[i][j][k].roomNum, calender[i][j][k].size, calender[i][j][k].status, calender[i][j][k].requesterID, calender[i][j][k].requesterEmail);
			}
		}
	}

	/* prompts user for number of threads to create - this simulates concurrent user requests */
	printf("\nHow many threads do you want?\n");
	printf("Enter an int between 1 and 10\n");
	printf("Choice: ");
	fflush(stdout);

	/* read in user input for number of threads - ensures that input is an int between 0 and MAX_THREADS */
	if(scanf("%d%c", &numTh, &term) != 2 || term != '\n' || numTh < 0 || numTh > MAX_THREADS){
		printf("\nInvalid input\n\n");
		return 0;
	}

	
	/* fills request array with requests - values come from user input */
	for(i = 0; i < numTh; i++){
		int inID, inDay, inHour, inRmNum;
		char inEmail[256];
		printf("\nRequest #%d\n", i);

		/* read in user input for user ID - ensures that input is a 6-digit int */
		printf("\nEnter your 6-digit user ID\n");
		printf("Choice: ");
		fflush(stdout);
		if(scanf("%d%c", &inID, &term) != 2 || term != '\n' || inID < 100000 || inID > 999999){
			printf("\nInvalid input\n");
			return 0;
		}

		/* read in user input for user email */
		printf("\nEnter your email\n");
		printf("Choice: ");
		fflush(stdout);
		fgets(inEmail, 256, stdin);
		if(strlen(inEmail) < 6){
			printf("\nInvalid input\n");
			return 0;
		}


		/* read in user input for requested day */
		printf("\nEnter the requested day (0-29)\n");
		printf("Choice: ");
		fflush(stdout);
		if(scanf("%d%c", &inDay, &term) != 2 || term != '\n' || inDay < 0 || inDay > 29){
			printf("\nInvalid input\n\n");
			return 0;
		}


		/* read in user input for requested hour */
		printf("\nEnter the requested hour (0-14)\n");
		printf("Choice: ");
		fflush(stdout);
		if(scanf("%d%c", &inHour, &term) != 2 || term != '\n' || inHour < 0 || inHour > 14){
			printf("\nInvalid input\n\n");
			return 0;
		}


		/* read in user input for requested room number */
		printf("\nEnter the requested room number\n");
		printf("Choice: ");
		fflush(stdout);
		if(scanf("%d%c", &inRmNum, &term) != 2 || term != '\n' || inRmNum < 100 || inRmNum > 499){
			printf("\nInvalid input\n\n");
			return 0;
		}
		requests[i].id = inID;				// populate requests array with data
		strcpy(requests[i].email, inEmail);	//
		requests[i].day = inDay;				//
		requests[i].time = inHour;			//
		requests[i].rmRequested = inRmNum;	//

		//printf("id: %d\temail: %s\tday: %d\ttime: %d\troomNum: %d\n", requests[i].id, requests[i].email, requests[i].day, requests[i].time, requests[i].rmRequested);
	}
	
	printf("\n");

	/* creates numTh threads, where numTh is determined by the user */
	for(i = 0; i < numTh; i++){
		printf("In main: creating thread %d\n", i);
		returnCode = pthread_create(&pth[i], NULL, reserveFunc, &requests[i]);
		if(returnCode){
			printf("ERROR: return code from pthread_create is %d\n", returnCode);
			return -1;
		}
	}


	/* creates numTh threads, where numTh is determined by the user
	for(i = 0; i < numTh; i++){
		printf("In main: creating thread %d\n", i);
		returnCode = pthread_create(&pth[i], NULL, reserveFunc, NULL);
		if(returnCode){
			printf("ERROR: return code from pthread_create is %d\n", returnCode);
			return -1;
		}
	}
	*/

	/* all threads join with main thread - makes main wait for all threads to finish */
	for(i = 0; i < numTh; i++){
		pthread_join(pth[i], NULL);
	}

	printf("This is main saying goodbye!\n\n");

	free(pth);						// free dynamic memory
	pthread_mutex_destroy(&mutex1);	// destroys mutex object
	pthread_exit(NULL);				// exits main thread
}
