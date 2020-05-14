
#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 

//gcc mutex.c -lpthread -o test

pthread_t t1[2]; 
int count; 
pthread_mutex_t lock; 
  
void* action(void* arg) 
{ 
    pthread_mutex_lock(&lock); 
  
    unsigned long i = 0; 
    count += 1; 
    printf("\n Job %d has started\n", count); 
  
    for (i = 0; i < (0xFFFFFFFF); i++) 
        ; 
  
    printf("\n Job %d has finished\n", count); 
  
    pthread_mutex_unlock(&lock); 
  
    return NULL; 
} 
  
int main(void) 
{ 
    int i = 0; 
    int error; 
  
    if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
  
    while (i < 2) { 
        error = pthread_create(&(t1[i]), NULL, &action, NULL); 
        if (error != 0) 
            printf("\nThread can't be created :[%s]", 
                   strerror(error)); 
        i++; 
    } 
  
    pthread_join(t1[0], NULL); 
    pthread_join(t1[1], NULL); 
    pthread_mutex_destroy(&lock); 
  
    return 0; 
} 
