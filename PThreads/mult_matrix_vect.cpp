#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <chrono>
#include <iostream>

#define m 8
#define n 8000000

// g++ -g -Wall -o test mult_matrix_vect.cpp -lpthread

typedef std::chrono::steady_clock::time_point time_pt;

int thread_count;
double A[m][n];
double x[n];
double y[m];

void* Pth_matrix_vector(void* rank);

int main(int argc, char* argv[])
{
    long thread;
    pthread_t* thread_handles;

    thread_count = strtol(argv[1], NULL, 10);

    thread_handles = (pthread_t*) malloc(thread_count * sizeof(pthread_t));

    double cont = 0;
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j) {
            A[i][j] = cont;
            cont += 2.0;
        }

    for (int i = 0; i < n; ++i) 
        x[i] = (double) i;

    time_pt begin = std::chrono::steady_clock::now();

    for (thread = 0; thread < thread_count; ++thread)
        pthread_create(&thread_handles[thread], NULL, Pth_matrix_vector, (void*) thread);

    void* ret;
    double *y2;
    int cont2 = 0;
    int local_m = m/thread_count;

    for (thread = 0; thread < thread_count; ++thread) {
        pthread_join(thread_handles[thread], &ret);
        y2 = (double*) ret;
        for (int i = 0; i < local_m; ++i) {
            y[cont2] = y2[i];
            ++cont2;
        }
        delete[] y2;
    }
    time_pt end = std::chrono::steady_clock::now();
    std::cout << "Tiempo: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " ms" <<std::endl;

    free(thread_handles);

    return 0;
}

void* Pth_matrix_vector(void* rank)
{
    long my_rank = (long) rank;
    int i, j;
    int local_m = m/thread_count;
    int my_first_row = my_rank*local_m;
    int my_last_row = (my_rank+1)*local_m - 1;
    double* y2 = new double[local_m];

    int idx = 0;
    for (i = my_first_row; i <= my_last_row; ++i) {
        y2[idx] = 0.0;
        for (j = 0; j < n; ++j) {
            y2[idx] += A[i][j] * x[j];
        }
        ++idx;
    }

    return (void*) y2;
}