#include <pthread.h> 
#include <stdio.h> 
#include <unistd.h> 

// gcc conditional_variable.c -lpthread -o test

//  thread condition variable 
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER; 
// declaring mutex 
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 

int change = 1; 

void* action() 
{ 

	// acquire a lock 
	pthread_mutex_lock(&lock); 
	if (change == 1) { 
		change = 2; 
		printf("Se espera la condición de variable: cond1\n"); 
		pthread_cond_wait(&cond1, &lock); 
	} 
	else { 
		printf("Señal de la condicion de variable cond1\n"); 
		pthread_cond_signal(&cond1); 
	} 

	// release lock 
	pthread_mutex_unlock(&lock); 

	printf("Returning thread\n"); 

	return NULL; 
} 

int main() 
{ 
	pthread_t t1, t2; 
	pthread_create(&t1, NULL, action, NULL); 
	sleep(1);  // give a chance the t1 run first 
	pthread_create(&t2, NULL, action, NULL); 
	pthread_join(t2, NULL); //t2 complete

	return 0; 
} 
