

#include <pthread.h>   //Create POSIX threads.
#include <time.h>	  //Wait for a random time.
#include <unistd.h>	//Thread calls sleep for specified number of seconds.
#include <semaphore.h> //To create semaphores
#include <stdlib.h>
#include <stdio.h> //Input Output

pthread_t *Students; //N threads running as Students.
pthread_t TA;		 //Separate Thread for TA.

int ChairsCount = 0;
int CurrentIndex = 0;

/*COMPLETE
 
//Declaration of Semaphores and Mutex Lock.
//Semaphores used:
//A semaphore to signal and wait TA's sleep. */
sem_t sem_TAsleep;

//An array of 3 semaphores to signal and wait chair to wait for the TA.
sem_t sem_chairs[3];

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
	sem_init(&sem_TAsleep, 0, 0);
	sem_init(&sem_nextStudent, 0, 0);
	for (id = 0; id < 3; id++)
		sem_init(&sem_chairs[id], 0, 0);

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
		pthread_create(&Students[id], NULL, Student_Activity, (void *) (long)id);

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
	
	//TA is currently sleeping.
	sem_wait(&sem_TAsleep);

	while (1)
	{

		// lock
		pthread_mutex_lock(&mutex);

		/** CRITICAL SECTION: Modifying ChairsCount & CurrentIndex, until unlock **/
		//if chairs are empty, break the loop.
		if (ChairsCount == 0)
		{
			pthread_mutex_unlock(&mutex);
			break;
		}

		//TA gets next student on chair.
		sem_post(&sem_chairs[CurrentIndex]);
		ChairsCount--;
		CurrentIndex = (CurrentIndex + 1) % 3;
		printf("%d chairs remain, because student left their chair for the TA room.\n", 3 - ChairsCount);

		//unlock
		pthread_mutex_unlock(&mutex);

		//TA is currently helping the student
		printf("TA is currently helping the student.\n");
		sem_post(&sem_nextStudent);

		//hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()
	}
	/**/
}

void *Student_Activity(void *threadID)
{
	/*TODO
    
	//Student needs help from the TA */
	printf("Student %ld needs help from the TA.\n", (long)threadID);

	while (1)
	{

		//Student tried to sit on a chair.
		if (ChairsCount < 3)
		{

			//wake up the TA.
			if (ChairsCount == 0)
				sem_post(&sem_TAsleep);
			else
				printf("Student %ld waiting for TA.\n", (long)threadID);

			// lock
			pthread_mutex_lock(&mutex);

			/** CRITICAL SECTION: Modifying ChairsCount, until unlock **/
			int prev_index = (CurrentIndex + ChairsCount) % 3;
			ChairsCount++;
			printf("Student sat down, %d chairs remaining.\n", 3 - ChairsCount);

			// unlock
			pthread_mutex_unlock(&mutex);

			//Student leaves his/her chair.
			sem_wait(&sem_chairs[prev_index]);

			//Student  is getting help from the TA
			printf("Student %ld is getting help from the TA.\n", (long)threadID);

			//Student waits to go next.
			sem_wait(&sem_nextStudent);

			//Student left TA room
			printf("Student %ld left TA room.\n", (long)threadID);
		}
		else
		{
			//If student didn't find any chair to sit on.
			//Student will return at another time
			printf("Student %ld didn't find any chair to sit on, and will return at another time.\n", (long)threadID);
		}
		//hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()
	}
	/**/
}
