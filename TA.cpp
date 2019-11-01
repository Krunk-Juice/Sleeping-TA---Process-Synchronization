

#include <pthread.h>   //Create POSIX threads.
#include <time.h>	  //Wait for a random time.
#include <unistd.h>	//Thread calls sleep for specified number of seconds.
#include <semaphore.h> //To create semaphores
#include <stdlib.h>
#include <stdio.h> //Input Output

pthread_t *Students; //N threads running as Students.
pthread_t TA;		 //Separate Thread for TA.

int ChairsCount = 0;
int CurrentSeat = 0;
int TeachSeat = 0;

/*COMPLETE
 
//Declaration of Semaphores and Mutex Lock.
//Semaphores used:
//A semaphore to signal and wait TA's sleep. */
sem_t sem_TAsleep;

//An array of 3 semaphores to signal and wait chair to wait for the TA.
int sem_chairs[3];

//A semaphore to signal and wait for TA's next student.
sem_t sem_nextStudent;

//Mutex Lock used:
//To lock and unlock variable ChairsCount to increment and decrement its value.
pthread_mutex_t mutex;

/* //hint: use sem_t and pthread_mutex_t
 
 
 */

//Declared Functions
void *TA_Activity(void*);
void *Student_Activity(void *threadID);

int main(int argc, char *argv[])
{
	int number_of_students; //a variable taken from the user to create student threads.	Default is 5 student threads.
	int id;
	srand(time(NULL));

	/*COMPLETE
	//Initializing Mutex Lock */
	pthread_mutex_init(&mutex, NULL);

	//and Semaphores.
	sem_init(&sem_TAsleep, 0, 1);
	sem_init(&sem_nextStudent, 0, 0);
	for (id = 0; id < 3; id++)
		sem_chairs[id] = 0;

	/* //hint: use sem_init() and pthread_mutex_init()
     */

	if (argc < 2)
	{
		printf("Number of Students not specified. Using default (5) students.\n");
		number_of_students = 5;
	}
	else
	{
		printf("Number of Students specified. Creating %d threads.\n", number_of_students);
		number_of_students = atoi(argv[1]);
	}

	//Allocate memory for Students
	Students = (pthread_t *)malloc(sizeof(pthread_t) * number_of_students);

	/*COMPLETE
	//Creating one TA thread and N Student threads.
     //hint: use pthread_create */
	pthread_create(&TA, NULL, TA_Activity, NULL);

	for (id = 0; id < number_of_students; id++)
		pthread_create(&Students[id], NULL, Student_Activity, (void *) (long)(id + 1));

	//Waiting for TA thread and N Student threads.
	//hint: use pthread_join
	pthread_join(TA, NULL);

	for (id = 0; id < number_of_students; id++)
		pthread_join(Students[id], NULL);

	/* */

	//Free allocated memory
	free(Students);
	return 0;
}

void *TA_Activity(void* threadID)
{
	/*TODO*/
	
	while (1)
	{
		if (ChairsCount > 0) {
		//TA is currently sleeping.
		sem_wait(&sem_nextStudent);

		// lock
		pthread_mutex_lock(&mutex);

		/** CRITICAL SECTION: Modifying ChairsCount & CurrentIndex, until unlock **/
		//if chairs are empty, break the loop.
		// if (ChairsCount == 0)
		// {
		// 	printf("No students need help. TA sleeping.\n");
		// 	pthread_mutex_unlock(&mutex);
		// 	break;
		// }

		//TA gets next student on chair.
		sem_chairs[TeachSeat] = 0;

		// printf("[TA] TA is teaching student %d,\n"
		// 	"\tfrom Chair %d.\n", 
		// 	sem_chairs[CurrentIndex], CurrentIndex);
		ChairsCount--;

		TeachSeat = (TeachSeat + 1) % 3;

		int help = rand() % 10 + 1;
		sleep(help);

		//printf("%d chairs remain, because student left their chair for the TA room.\n", 3 - ChairsCount);
		printf("[TA] Students waiting:\n"
			"\t[Chair 0] = %d"
			"\t[Chair 1] = %d"
			"\t[Chair 2] = %d\n",
			sem_chairs[0], sem_chairs[1], sem_chairs[2]);

		if (ChairsCount == 0)
			printf("[TA] No students need help. TA sleeping.\n");

		//unlock
		pthread_mutex_unlock(&mutex);

		//TA is currently helping the student

		//printf("TA is currently helping the student.\n");

		sem_post(&sem_TAsleep);
		}

		// printf("ChairsCount = %d\n", ChairsCount);

		//hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()
	}
	/**/
}

void *Student_Activity(void *threadID)
{
	/*TODO
    
	//Student needs help from the TA */
	printf("[Student] Student %ld will eventually need help from the TA.\n", (long)threadID);
	printf("TA is sleeping.\n");

	while (1)
	{
		int arrive = rand() % 5 + 1;
		sleep(arrive);

		pthread_mutex_lock(&mutex);

		//Student tried to sit on a chair.
		if (ChairsCount < 3)
		{

			//wake up the TA.
			// sem_wait(&sem_TAsleep);

			// lock
			// pthread_mutex_lock(&mutex);

			sem_chairs[CurrentSeat] = (long)threadID;
			ChairsCount++;

			printf("[Student] Student %ld sat down in waiting chair, "
				"%d chairs remaining.\n",
				(long)threadID, 3 - ChairsCount);

			printf("[Student] Students waiting:\n"
				"\t[Chair 0] = %d"
				"\t[Chair 1] = %d"
				"\t[Chair 2] = %d\n",
				sem_chairs[0], sem_chairs[1], sem_chairs[2]);

			CurrentSeat = (CurrentSeat + 1) % 3;

			// unlock
			pthread_mutex_unlock(&mutex);

			//Student leaves his/her chair.
			

			//Student  is getting help from the TA
			printf("[Student] Student %ld is getting help from the TA.\n", (long)threadID);

			//Student waits to go next.
			sem_post(&sem_nextStudent);

			sem_wait(&sem_TAsleep);

			//Student left TA room
			printf("[Student] Student %ld left TA room.\n", (long)threadID);
		}
		else
		{
			//If student didn't find any chair to sit on.
			//Student will return at another time
			pthread_mutex_unlock(&mutex);

			printf("[Student] Student %ld didn't find any chair to sit on, and will return at another time.\n", (long)threadID);
		}
		//hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()
	}
	/**/
}
