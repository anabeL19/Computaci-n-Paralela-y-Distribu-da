#include <stdio.h> 
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 

// gcc semaphores.c -lpthread -o test

sem_t smutex; 
int count;

void* action(void* arg) 
{ 
	sem_wait(&smutex); 
	unsigned long i = 0; 
    count += 1; 
    printf("\n Task %d has started\n", count); 
  
    //critical section 
	// sleep(4);
    for (i = 0; i < (0xFFFFFFFF); i++) 
        ; 
 
    printf("\n Task %d has finished\n", count); 
	sem_post(&smutex); 
} 


int main() 
{ 
	sem_init(&smutex, 0, 1); 
	pthread_t t1,t2; 
	pthread_create(&t1,NULL,action,NULL); 
	sleep(2); 
	pthread_create(&t2,NULL,action,NULL); 
	pthread_join(t1,NULL); 
	pthread_join(t2,NULL); 
	sem_destroy(&smutex); 

	return 0; 
} 
