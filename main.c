// Developer: Alex St.Aubin
// Program: The Dining Philosophers
// Description: The dining philosophers scenario is a classic which is structured as 
//                follows. Five philosophers, numbered zero to four, are sitting at 
//                a round table, thinking. As time passes, different individuals become 
//                hungry and decide to eat. There is a platter of spaghetti on the 
//                table but each philosopher only has one fork to use. To eat, they 
//                must share forks. The fork to the left of each philosopher (as they 
//                sit facing the table) has the same number as that philosopher.

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

// Global Variables:
int status;
sem_t forkArr[5];
sem_t room;

// Global Constants:
const int TOTAL_PHILOSOPHERS = 5;
const int MAX_TIMES_EATING = 5;

// Function: getAgruments(int, char*)
// Description: Reads command line arguments for deadlock/no deadlock option.
// Parameters: Argument count, argument vector.
// Returns: Pass - entered flag
//          Fail - -1
int getArguments(int argc, char* argv[]){
	// Print usage message.
	if(argc == 1){
		printf("Usage: ./dining <1(deadlock)/0(no deadlock)>\n");
		return -1;
	}
	// Error if not exactly 2 arguments.
	if(argc != 2){
		printf("Error: invalid command line arguments.\n");
		return -1;
	}
	// Convert from ASCII to integer and check in bounds.
	int tmp = atoi(argv[1]);
	if(tmp != 0 && tmp != 1){
		printf("Error: invalid flag value.\n");
		printf("Usage: ./dining <1(deadlock)/0(no deadlock)>\n");
		return -1;
	}
	return tmp;
}

// Function: dining(void*)
// Description: Sends a philosopher to grab his forks,
//                eat, then drop his forks 5 times.
// Parameters: Philosopher number.
// Returns: NULL
void* dining(void* philNum){
	int timesEaten = 0;
	long phil = (long)philNum;
	while(timesEaten < MAX_TIMES_EATING){
		// Think;
		sleep(1);
		printf("Philosopher %ld is done thinking and now ready to eat.\n", phil);
		// If avoiding deadlock, make sure there is room at the table.
		if(status == 0)
			sem_wait(&room);
		// Pick up left fork[phil]
		sem_wait(&forkArr[phil]);
		printf("Philosopher %ld: took left fork %ld\n", phil, phil);
		// Make it easier to observe the deadlock
		if(status == 1)
			sleep(1);
		// Pick up right fork[(phil+1)%n]
		sem_wait(&forkArr[(phil + 1) % TOTAL_PHILOSOPHERS]);
		printf("Philosopher %ld: took right fork %ld\n", phil, ((phil + 1) % TOTAL_PHILOSOPHERS));
		// Eat
		printf("Philosopher %ld: EATING\n", phil);
		sleep(1);
		// Set down left fork[phil]
		sem_post(&forkArr[phil]);
		printf("Philosopher %ld: put down left fork %ld\n", phil, phil);
		// Set down right fork[(phil+1)%n]
		sem_post(&forkArr[(phil + 1) % TOTAL_PHILOSOPHERS]);
		printf("Philosopher %ld: put down right fork %ld\n", phil, ((phil + 1) % TOTAL_PHILOSOPHERS));
		// If avoiding deadlock, make sure to notify you are leaving the table.
		if(status == 0)
			sem_post(&room);
		timesEaten++;
	}
	pthread_exit(NULL);
}

// Function: main(int, char*)
// Description: Tests the dining philosophers algorithm.
// Parameters: Command line arguments.
// Returns: Terminated program.
int main(int argc, char* argv[]){
	// Get command line arguments and exit if invalid.
	status = getArguments(argc, argv);
	if(status == -1){
		printf("Error: getArguments() failure.\n");
		exit(1);
	}
	// Initialize forks and semaphore room.
	int i;
	for(i = 0; i < TOTAL_PHILOSOPHERS; i++)
		sem_init(&forkArr[i], 0, 1);
	sem_init(&room, 0, (TOTAL_PHILOSOPHERS - 1));
	// Create philosopher array.
	pthread_t philosophers[TOTAL_PHILOSOPHERS];
	// Create each philosopher.
	for(i = 0; i < TOTAL_PHILOSOPHERS; i++)
		pthread_create(&philosophers[i], NULL, dining, (void*)(long)i);
	// Wait for all philosophers to come back.
	for(i = 0; i < TOTAL_PHILOSOPHERS; i++)
		pthread_join(philosophers[i], NULL);
	// Deallocate semaphores
	for(i = 0; i < TOTAL_PHILOSOPHERS; i++)
		sem_destroy(&forkArr[i]);
	sem_destroy(&room);
	exit(1);
}