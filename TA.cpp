

#include <pthread.h>   //Create POSIX threads.
#include <time.h>	  //Wait for a random time.
#include <unistd.h>	//Thread calls sleep for specified number of seconds.
#include <semaphore.h> //To create semaphores
#include <stdlib.h>
#include <stdio.h> //Input Output

pthread_t *Students; //N threads running as Students.
pthread_t TA;		 //Separate Thread for TA.

int ChairsCount = 0;
int CurrentSeat = 3 - 1;
int TeachSeat = 3 - 1;

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

bool sleep_ = true; // to prevent multiple prints.

//Declared Functions
void *TA_Activity(void *);
void *Student_Activity(void *threadID);

int main(int argc, char *argv[])
{
	int number_of_students; //a variable taken from the user to create student threads.	Default is 5 student threads.
	int id;
	srand(time(NULL));

	/*COMPLETE
	//Initializing Mutex Lock */
	if (pthread_mutex_init(&mutex, NULL) == -1)
	{
		perror("pthread_mutex_init - [main] mutex");
		exit(EXIT_FAILURE);
	}

	//and Semaphores.
	if (sem_init(&sem_TAsleep, 0, 1) == -1)
	{
		perror("sem_init - [main] sem_TAsleep");
		exit(EXIT_FAILURE);
	}

	if (sem_init(&sem_nextStudent, 0, 0) == -1)
	{
		perror("sem_init - [main] sem_nextStudent");
		exit(EXIT_FAILURE);
	}

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
	if (pthread_create(&TA, NULL, TA_Activity, NULL) == -1)
	{
		perror("pthread_create - [main] TA");
		exit(EXIT_FAILURE);
	}

	for (id = 0; id < number_of_students; id++)
	{
		if (pthread_create(&Students[id], NULL, Student_Activity, (void *)(long)(id + 1)) == -1)
		{
			perror("pthread_create - [main] Students");
			exit(EXIT_FAILURE);
		}
	}

	//Waiting for TA thread and N Student threads.
	//hint: use pthread_join
	if (pthread_join(TA, NULL) == -1)
	{
		perror("pthread_join - [main] TA");
		exit(EXIT_FAILURE);
	}

	for (id = 0; id < number_of_students; id++)
	{
		if (pthread_join(Students[id], NULL) == -1)
		{
			perror("pthread_join - [main] Students");
			exit(EXIT_FAILURE);
		}
	}

	/* */

	//Free allocated memory
	free(Students);
	return 0;
}

void *TA_Activity(void *threadID)
{
	/*COMPLETE*/

	while (1)
	{
		if (ChairsCount > 0)
		{
			//TA is currently sleeping.
			sleep_ = false;
			if (sem_wait(&sem_nextStudent) == -1)
			{
				perror("sem_wait - [TA] sem_nextStudent");
				exit(EXIT_FAILURE);
			}

			// lock
			if (pthread_mutex_lock(&mutex) == -1)
			{
				perror("pthread_mutex_lock - [TA] mutex");
				exit(EXIT_FAILURE);
			}

			//if chairs are empty, break the loop.

			//TA gets next student on chair.
			printf("[TA] TA is teaching student %d, from Chair %d.\n",
				   sem_chairs[TeachSeat], TeachSeat);

			int tempStudent = sem_chairs[TeachSeat];

			sem_chairs[TeachSeat] = 0;

			ChairsCount--;

			TeachSeat = (TeachSeat + 1) % 3;

			int help = rand() % 10 + 1;
			sleep(help);

			printf("[TA] Student %d left TA room.\n", tempStudent);

			printf("[TA] Students waiting:\n"
				   "\t    [Chair 0]\t    [Chair 1]\t    [Chair 2]\n"
				   "\t\t%d\t\t%d\t\t%d\n",
				   sem_chairs[0], sem_chairs[1], sem_chairs[2]);

			//printf("%d chairs remain, because student left their chair for the TA room.\n", 3 - ChairsCount);

			//unlock
			if (pthread_mutex_unlock(&mutex) == -1)
			{
				perror("pthread_mutex_unlock - [TA] mutex");
				exit(EXIT_FAILURE);
			}

			//TA is currently helping the student

			//printf("TA is currently helping the student.\n");

			if (sem_post(&sem_TAsleep) == -1)
			{
				perror("sem_post - [TA] sem_TAsleep");
				exit(EXIT_FAILURE);
			}
		}
		else if (ChairsCount == 0 && !sleep_)
		{
			printf("[TA] No student needs help.\n\tAll chairs are empty.\n\tTA is sleeping.\n");
			sleep_ = true;
		}
		// printf("ChairsCount = %d\n", ChairsCount);

		//hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()
	}
	/**/
}

void *Student_Activity(void *threadID)
{
	/*COMPLETE
    
	//Student needs help from the TA */
	printf("[Student] Student %ld will eventually need help from the TA.\n", (long)threadID);

	while (1)
	{
		int arrive = rand() % 5 + 1;
		sleep(arrive);

		if (pthread_mutex_lock(&mutex) == -1) {
			perror("pthread_mutex_lock - [Student] mutex");
			exit(EXIT_FAILURE);
		}

		//Student tried to sit on a chair.
		if (ChairsCount < 3)
		{

			//wake up the TA.
			// sem_wait(&sem_TAsleep);

			// lock
			// pthread_mutex_lock(&mutex);

			sem_chairs[CurrentSeat] = (long)threadID;
			ChairsCount++;

			printf("[Student] Student %ld sat down in Chair %d, "
				   "%d chairs remaining.\n",
				   (long)threadID, CurrentSeat, 3 - ChairsCount);

			printf("[Student] Students waiting:\n"
				   "\t    [Chair 0]\t    [Chair 1]\t    [Chair 2]\n"
				   "\t\t%d\t\t%d\t\t%d\n",
				   sem_chairs[0], sem_chairs[1], sem_chairs[2]);

			CurrentSeat = (CurrentSeat + 1) % 3;

			// unlock
			if (pthread_mutex_unlock(&mutex) == -1) {
				perror("pthread_mutex_unlock - [Student] mutex");
				exit(EXIT_FAILURE);
			}

			//Student leaves his/her chair.

			//Student  is getting help from the TA
			// printf("[Student] Student %ld is getting help from the TA.\n", (long)threadID);

			//Student waits to go next.
			if (sem_post(&sem_nextStudent) == -1) {
				perror("sem_post - [Student] sem_nextStudent");
				exit(EXIT_FAILURE);
			}

			if (sem_wait(&sem_TAsleep) == -1) {
				perror("sem_wait - [Student] sem_TAsleep");
				exit(EXIT_FAILURE);
			}

			//Student left TA room
			// printf("[Student] Student %ld left TA room.\n", (long)threadID);
		}
		else
		{
			//If student didn't find any chair to sit on.
			//Student will return at another time
			if (pthread_mutex_unlock(&mutex) == -1) {
				perror("pthread_mutex_unlock - [Student] mutex");
				exit(EXIT_FAILURE);
			}

			printf("[Student] Student %ld didn't find any chair to sit on, and will return at another time.\n", (long)threadID);
		}
		//hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()
	}
	/**/
}
