#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdlib.h> 
#include <queue> 
#include <semaphore.h>
using namespace std;

#define NUM_THREADS 10
#define MEMORY_SIZE 150

struct node
{
	int id;
	int size;
};


queue<node> myqueue; // shared que
pthread_mutex_t sharedLock = PTHREAD_MUTEX_INITIALIZER; // mutex
pthread_t server; // server thread handle
sem_t semlist[NUM_THREADS]; // thread semaphores

int thread_message[NUM_THREADS]; // thread memory information
char  memory[MEMORY_SIZE]; // memory size

bool flag = true;
int idx = 0;
//int used_memory = 0;


void my_malloc(int thread_id, int size)
{
	//This function will add the struct to the queue
	node queue_node;
    queue_node.id = thread_id;
    queue_node.size = size;
	pthread_mutex_lock(&sharedLock);
    myqueue.push(queue_node);    //critical region
	pthread_mutex_unlock(&sharedLock);
	
}

void * server_function(void *)
{
	//This function should grant or decline a thread depending on memory size.
	while(flag)  //run in a loop until all threads are done
	{
		pthread_mutex_lock(&sharedLock);
		if(!myqueue.empty())
		{
			if(idx + myqueue.front().size > MEMORY_SIZE)
			{
				thread_message[myqueue.front().id] = -1; //no available size
			} 
			else                                          //start point of the memory location that will be allocated to the requesting thread
			{
				thread_message[myqueue.front().id] = idx;  
				idx += myqueue.front().size;
			}

			sem_post(&semlist[myqueue.front().id]); //incrementing semaphore
			myqueue.pop();
		}
		pthread_mutex_unlock(&sharedLock);
	}
	//node tmp;
	//tmp = myqueue.front();
	//used_memory += tmp.size;
	//if (used_memory > MEMORY_SIZE)
	//{
	//	thread_message[tmp.id] = -1;
	//}
	//else
	//{
	//	thread_message[tmp.id] = used_memory - tmp.size;
	//}
	//sem_post(&semlist[myqueue.front().id]);
	//myqueue.pop();

}

void * thread_function(void * id) 
{
	//This function will create a random size, and call my_malloc
	//Block
	//Then fill the memory with id's or give an error prompt
	srand(time(NULL));
	int *numID = (int*)id; //thread_id

	int randomSize = (rand()%(MEMORY_SIZE/6) + 1);

	my_malloc(*numID, randomSize); //function call


	sem_wait(&semlist[*numID]); //semaphore decrements the value, if it is positive. 
	                            //If it is negative or zero, then no operation is performed - blocked.

	if (thread_message[*numID] == -1)
	{
		pthread_mutex_lock(&sharedLock);
		cout << "Thread" <<  *numID << ": Not enough memory." << endl;
		pthread_mutex_unlock(&sharedLock);
	}
	else
	{
		char chID = '0' + *numID; //int id to char
		for (int i = 0; i < randomSize; i++) // enough memory size
		{
			//int thread_id = *numID;
			//char ch = (char) thread_id;
			memory[thread_message[*numID]+i] = chID; //ýf there is enough memory all bits assign to thread id
		}
	}
}

void init()	 
{
	pthread_mutex_lock(&sharedLock);	//lock
	for(int i = 0; i < NUM_THREADS; i++) //initialize semaphores
	{sem_init(&semlist[i],0,0);}
	for (int i = 0; i < MEMORY_SIZE; i++)	//initialize memory 
  	{char zero = '0'; memory[i] = zero;}
   	pthread_create(&server,NULL,server_function,NULL); //start server 
	pthread_mutex_unlock(&sharedLock); //unlock
}



void dump_memory() 
{
 // You need to print the whole memory array here.
	for (int i = 0; i < MEMORY_SIZE; i++)
	{
		printf("%c", memory[i]);
	}
}

int main (int argc, char *argv[])
 {

 	//You need to create a thread ID array here
	 pthread_t threadArray[NUM_THREADS];
	 int idThreads[NUM_THREADS]; //thread_id

	  
	 for (int i = 0; i < NUM_THREADS; i++)  //created NUM of threads and assign them to array
	 {
		 pthread_t th;
		 threadArray[i] = th;
		 idThreads[i] = i;
	}

 	init();	// call init

 	//You need to create threads with using thread ID array, using pthread_create()
	for(int i = 0; i < NUM_THREADS; i++)
	{
		pthread_create(&threadArray[i], NULL, thread_function ,(void *)&idThreads[i]);
	}

 	//You need to join the threads
	for(int i = 0; i < NUM_THREADS; i++)
	{
		pthread_join(threadArray[i], NULL);
	}

 	dump_memory(); // this will print out the memory
 	flag = false;
 	printf("\nMemory Indexes:\n" );
 	for (int i = 0; i < NUM_THREADS; i++)
 	{
 		printf("[%d]" ,thread_message[i]); // this will print out the memory indexes
 	}
 	printf("\nTerminating...\n");
 }