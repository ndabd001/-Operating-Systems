
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h> 
#define PTHREAD_SYNC

int sharedVariable = 0;
pthread_mutex_t lock1;
pthread_barrier_t lock2;

void* simpleThread(int which)
{
	int num,val;
	for (num = 0; num < 20; num++)
	{

		// Mutex lock before updating val
		#ifdef PTHREAD_SYNC
			pthread_mutex_lock(&lock1);
		#endif

		val = sharedVariable;
		printf("*** thread %d sees value %d\n", which, val);


		sharedVariable = val + 1;

		// after update, unlock
		#ifdef PTHREAD_SYNC
			pthread_mutex_unlock(&lock1);
		#endif
	}

	// Wait for next thread to finish
	#ifdef PTHREAD_SYNC
		int ret = pthread_barrier_wait(&lock2);
	#endif

	val = sharedVariable;
	printf("Thread %d sees final value %d\n", which, val);
}


int main(int argc, char **argv)
{
	//declare global variables
	extern char *optarg;
	extern int optind;
	int c, err = 0;
	int tflag, threadID = 0;
	void* threadStatus;	
	static char usage[] = "usage %s [-t num]\n";

	// take command line input and process it
	while ((c = getopt(argc, argv, "t:")) != -1)
	{	
		switch(c) 
		{
			case 't':
				threadID = atoi(optarg);
				tflag = 1;
				break;
			default:
				err = 1;
				break;
		}
	}

	// Check for parsing errors
	if(err > 0 || threadID < 0)
	{
		printf("Error! Invalid input\n%s",usage);
		exit(0);
	}


	// Process input
	pthread_t threads[threadID];
	long i;
	int returnVal;

	// Mutex locks on variable to synchronize the threads
	pthread_mutex_init(&lock1, NULL);
	int ret = pthread_barrier_init(&lock2,NULL,threadID);

	for (i = 0; i < threadID; ++i)
	{
		printf("MAIN: Creating thread number %ld\n", i);
		returnVal = pthread_create(&threads[i],NULL,simpleThread, (void*)i);
		// Check for errors when creating a thread
		if(returnVal != 0)
		{
			printf("ERROR: pthread_create() error value: %d\n", returnVal);
			exit(-1);
		}
	}

	// Wait for all other threads to finish 
	for (i = 0; i < threadID; ++i)
	{
		pthread_join(threads[i],&threadStatus); 

	// unlock and end process
	pthread_mutex_destroy(&lock1);
	pthread_exit(NULL);
	return 0;
}