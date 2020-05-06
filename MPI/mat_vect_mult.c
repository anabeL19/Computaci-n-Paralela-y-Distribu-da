#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


// Compile:  mpicc -g -Wall -o mpi mat_vect_mult.c
// Run:      mpiexec -n <number of processes> ./mpi


void Generate_matrix(double local_A[], int local_m, int n);
void Generate_vector(double local_x[], int local_n);
void Print_matrix(char title[], double local_A[], int m, int local_m, int n, int rank, MPI_Comm comm);
void Print_vector(char title[], double local_vec[], int n, int local_n, int rank, MPI_Comm comm);
void Mat_vect_mult(double local_A[], double local_x[], double local_y[], int local_m, int n, int local_n, MPI_Comm comm);


int main(void) {
   double* local_A;
   double* local_x;
   double* local_y;
   int m, local_m, n, local_n;
   int rank, comm_sz;
   MPI_Comm comm;
   double start, finish, loc_elapsed, elapsed;

   MPI_Init(NULL, NULL);
   comm = MPI_COMM_WORLD;
   MPI_Comm_size(comm, &comm_sz);
   MPI_Comm_rank(comm, &rank);

//----------------------------------------------------------------------------------------
//    Get_dims(&m, &local_m, &n, &local_n, rank, comm_sz, comm);
   if (rank == 0) {
      printf("Number of rows\n");
      scanf("%d", &m);
      printf("Number of columns\n");
      scanf("%d", &n);
   }
   MPI_Bcast(&m, 1, MPI_INT, 0, comm);
   MPI_Bcast(&n, 1, MPI_INT, 0, comm);

   local_m = m/comm_sz;
   local_n = n/comm_sz;
//--------------------------------------------------------------------------------      
//    Allocate_arrays(&local_A, &local_x, &local_y, local_m, n, local_n, comm);
   local_A = malloc(local_m*n*sizeof(double));
   local_x = malloc(local_n*sizeof(double));
   local_y = malloc(local_m*sizeof(double));
//--------------------------------------------------------------------------------

   srandom(rank);
   Generate_matrix(local_A, local_m, n);
//    Print_matrix("A", local_A, m, local_m, n, rank, comm);

   Generate_vector(local_x, local_n);
//    Print_vector("x", local_x, n, local_n, rank, comm);

   MPI_Barrier(comm);
   start = MPI_Wtime();
   Mat_vect_mult(local_A, local_x, local_y, local_m, n, local_n, comm);
   finish = MPI_Wtime();
   loc_elapsed = finish-start;
   MPI_Reduce(&loc_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

//    Print_vector("y", local_y, m, local_m, rank, comm);

   if (rank == 0)
      printf("Elapsed time = %e\n", elapsed);

   free(local_A);
   free(local_x);
   free(local_y);
   MPI_Finalize();
   return 0;
}

void Generate_matrix(double local_A[], int local_m, int n) //local_A[] is out
{
   int i, j;

   for (i = 0; i < local_m; i++)
      for (j = 0; j < n; j++) 
         local_A[i*n + j] = ((double) random())/((double) RAND_MAX);
}  

void Generate_vector(double local_x[], int local_n) //local_x[] is out
{
   int i;

   for (i = 0; i < local_n; i++)
      local_x[i] = ((double) random())/((double) RAND_MAX);
}

void Print_matrix(char title[], double local_A[], int m, int local_m, int n, int rank, MPI_Comm comm) 
{
   double* A = NULL;
   int i, j;

   if (rank == 0) {
      A = malloc(m*n*sizeof(double));
      MPI_Gather(local_A, local_m*n, MPI_DOUBLE,
            A, local_m*n, MPI_DOUBLE, 0, comm);
      printf("\nThe matrix %s\n", title);
      for (i = 0; i < m; i++) {
         for (j = 0; j < n; j++)
            printf("%f ", A[i*n+j]);
         printf("\n");
      }
      printf("\n");
      free(A);
   } else {
      MPI_Gather(local_A, local_m*n, MPI_DOUBLE,
            A, local_m*n, MPI_DOUBLE, 0, comm);
   }
} 

void Print_vector(char title[], double local_vec[], int n, int local_n, int rank, MPI_Comm comm) 
{
   double* vec = NULL;
   int i;

   if (rank == 0) {
      vec = malloc(n*sizeof(double));
      MPI_Gather(local_vec, local_n, MPI_DOUBLE,
            vec, local_n, MPI_DOUBLE, 0, comm);
      printf("\nThe vector %s\n", title);
      for (i = 0; i < n; i++)
         printf("%f ", vec[i]);
      printf("\n");
      free(vec);
   }  else {
      MPI_Gather(local_vec, local_n, MPI_DOUBLE,
            vec, local_n, MPI_DOUBLE, 0, comm);
   }
} 

void Mat_vect_mult(double local_A[], double local_x[], double local_y[], 
                  int local_m, int n, int local_n, MPI_Comm comm)//local_y[] is out 
{
   double* x;
   int local_i, j;

   x = malloc(n*sizeof(double));
   MPI_Allgather(local_x, local_n, MPI_DOUBLE,
         x, local_n, MPI_DOUBLE, comm);

   for (local_i = 0; local_i < local_m; local_i++) {
      local_y[local_i] = 0.0;
      for (j = 0; j < n; j++)
         local_y[local_i] += local_A[local_i*n+j]*x[j];
   }
   free(x);
} 
