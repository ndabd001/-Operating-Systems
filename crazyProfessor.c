
/*
* Nicolas Dabdoub
* PID: 4531579
*/

#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> 
#include <unistd.h>
#include <signal.h>

// -------------------------- Structs ------------------------------------
//Creates a Queue Structure

struct Student
{
	int id;						//each student knows their id
	int numQuestions;			//number of Questions
	int count;					//number of times student asks question
	struct Student1 *Student2;
};

// -------------------------- Globals ------------------------------------

pthread_mutex_t lock0, lock1, lock2, lock3, lock4;
pthread_cond_t questionAsk, endQuestion;
pthread_t professor_t;
int qSignal; // a question has been asked. For professors
int aSignal; // a question has been answered. For students

int studentsInOffice; // Number of students currently in office.
int officeCapacity; // User defined capacity of the Office
int studentsProcessed; // How many students have been answered.
int totalStudents; // User defined; Number of student to ask questions

// Queue iterators
struct Student *head = NULL;
struct Student *currStudent = NULL;

// -------------------------- Constructors/Destructors ----------------------

void studentInit(struct Student1 *s, int idNum)
{
	s->count = 0;
	s->id = idNum;
	s->numQuestions = (idNum % 4) + 1;
}

void threadsStart()
{	
	pthread_cond_init(&(questionAsk),NULL);
	pthread_cond_init(&(endQuestion),NULL);
	pthread_mutex_init(&(lock0),NULL);
	pthread_mutex_init(&(lock1),NULL);
	pthread_mutex_init(&(lock2),NULL);
	pthread_mutex_init(&(lock3),NULL);
	pthread_mutex_init(&(lock4),NULL);
}

void threadsEnd()
{
	pthread_cond_destroy(&(questionAsk));
	pthread_cond_destroy(&(endQuestion));
	pthread_mutex_destroy(&(lock0));
	pthread_mutex_destroy(&(lock1));
	pthread_mutex_destroy(&(lock2));
	pthread_mutex_destroy(&(lock3));
	pthread_mutex_destroy(&(lock4));
}

// -------------------------- Professor functions -------------------------- 

void answerDone()
{	
	printf("Professor is done with answer for student %d \n",head->id);
}

void answerStart()
{	
	printf("Professor starts to answer question for student %d.\n", head->id);
}

/*
* Listen for qsignal, block when the professor answers
*/
void* professorStart(void* thread)
{	
	printf("Prof thread started.\n");	
	qSignal = 1; 

	while(1)
	{
		// Close the office once all of the student's questions have been answered.
		if(studentsProcessed == totalStudents)
		{
			printf("\nAll questions have been answered; this office is now closed\n");
			break;
		}	

		// Wait for a question
		pthread_cond_wait(&questionAsk,&lock1);
		qSignal = 0; // Prof thread not ready

		if(head != NULL)
		{
			answerStart();
			answerDone();

			// answer signal given
			while(aSignal)
				pthread_cond_signal(&enduestion);

			aSignal = 1;
		}
	
	}	
}

/*
* This function begins a professor thread,
* which answers a student threads questions.
*/
void professor(int cap, int sNum)
{	
	// Initialize variables 
	studentsInOffice = 0;
	qSignal = 0;
	aSignal = 1;
	officeCapacity = cap;
	studentsProcessed = 0;
	totalStudents = sNum;
	threadsStart();

	// create thread
	int returnVal = 0;
	returnVal = pthread_create(&professor_t,NULL,professorStart,NULL);
}

// -------------------------- Student functions -------------------------- 

/*
* Returns 1 if there is room avialable
* -1 if the office is at capacity.
*/
int enterOffice()
{	
	if(studentsInOffice < officeCapacity)
		return 0;
	else
		return -1;
}

// This function removes a student from the queue
void leaveOffice()
{	
	--studentsInOffice;
	printf("Student %d leaves the office.\n", head->id);
	if(head != NULL)
	{
		free(head);
		head = head->Student2;
	}

	if(head == NULL)
		pthread_cond_signal(&questionAsk);
}

// when student is satisfied with all his questions, he should leave
void questionDone()
{	
	printf("Student %d is satisfied.\n", head->id);

	
	if(head->count == head->numQuestions)
	{
		++studentsProcessed;
		leaveOffice();
	}
}

void questionStart()
{	
	printf("Student %d asks a question.\n", head->id);
	++head->count;

	// question signal
	while(qSignal)
		pthread_cond_signal(&questionAsk);

	qSignal = 1;

	// wait for answer signal
	pthread_cond_wait(&endQuestion,&lock2);
	aSignal = 0;
}

void* studentStart(void* args)
{	
	struct Student1 *someStudent = args;

	printf("Student %d is outside the professor's office.\n", someStudent->id);

	// student doesnt start until there is capacity
	while(enterOffice(someStudent) < 0){}

	// student added to queue
	pthread_mutex_lock(&(lock3));

	++studentsInOffice;

	if(head != NULL)
	{
		currStudent->Student2 = someStudent;
		currStudent = someStudent;
	}
	//Empty queue
	else
	{	
		qSignal = 1; 
		head = someStudent;
		currStudent = head;
	}

	pthread_mutex_unlock(&(lock3)); 

	printf("Student %d shows up in the office.\n", someStudent->id);

	int i;
	// Professor is asked the pre-defined amount of questions.
	for(i = 0; i < someStudent->numQuestions; ++i)
	{
		pthread_mutex_lock(&lock);

		if((head != NULL) && studentsInOffice == 0)
		{
			head = NULL;
			pthread_cond_signal(&questionAsk);
		}

		// nexxt student
		if (head != NULL)
		{
			questionStart();
			questionDone();
		}

		pthread_mutex_unlock(&lock);
	}	
}

void student(int id)
{	
	// below creates a new student
	struct Student1 *s  = (struct Student1*)malloc(sizeof(struct Student1));
	studentInit(s,id);

	// below starts a new student thread
	pthread_t student_t;
	pthread_create(&student_t,NULL,studentStart,s);	
}	

// -------------------------- Main function -------------------------- 


/*
1 Prof thread that waits until a question is asked from a student. 
No other student can ask questions until Prof thread is completed
*/

int main(int argc, char **argv)
{
	extern char *optarg;
	extern int optind;
	int c, err = 0;
	int cflag, sflag, numStudents, officeCap = 0;
	void* status;
	static char usage[] = "usage %s [-s num] [-c num]\n";

	// Parse command line input
	while ((c = getopt(argc, argv, "s:c:")) != -1)
	{	
		switch(c) 
		{
			case 'c':
				cflag = 1;
				officeCap = atoi(optarg);
				break;
			case 's':
				sflag = 1;
				numStudents = atoi(optarg);
				break;
			default:
				err = 1;
				break;
		}
	}

	// Check for valid input
	if(err > 0 || numStudents < 0 || officeCap <= 0)
	{
		printf("This input is invalid ERROR.\n%s",usage);
		exit(0);
	}
	
	professor(officeCap,numStudents);
	int i;
	
	for (i = 0; i < numStudents; ++i)
	{
		student(i);
	}

	pthread_exit(NULL);
	threadsEnd();

	return 0;
}