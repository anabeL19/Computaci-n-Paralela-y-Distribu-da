#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

// Compile:  mpicc -g -Wall -o mpi odd_even.c
// Run: mpiexec -n <number of processes> ./mpi <number of elements in global list> 
 
const int MAX = 10;

// Local functions 
void Print_list(int local_keys[], int local_n, int rank);
void Merge_low(int local_keys[], int temp_B[], int temp_C[], int local_n);
void Merge_high(int local_keys[], int temp_B[], int temp_C[], int local_n);
void Generate_list(int local_keys[], int local_n, int rank);
int  Compare(const void* _a, const void* _b);

// Functions involving communication 
void Sort(int local_keys[], int local_n, int rank, int comm_sz, MPI_Comm comm);
int Compute_partner(int phase, int rank, int comm_sz);
void Odd_even_iter(int local_keys[], int temp_B[], int temp_C[], int local_n, 
         int phase, int partner, int rank, int comm_sz, MPI_Comm comm);
void Print_local_lists(int local_keys[], int local_n, int rank, int comm_sz, MPI_Comm comm);
void Print_global_list(int local_keys[], int local_n, int rank, int comm_sz, MPI_Comm comm);


int main(int argc, char* argv[]) {
   int rank, comm_sz;
   char g_i;
   int *local_keys;
   int global_n;
   int local_n;
   MPI_Comm comm;
   double start, finish, loc_elapsed, elapsed;

   MPI_Init(&argc, &argv);
   comm = MPI_COMM_WORLD;
   MPI_Comm_size(comm, &comm_sz);
   MPI_Comm_rank(comm, &rank);

//-------------------------------------------------------------------------------------
   if (rank == 0) {
      if (argc != 2) 
         global_n = -1;  // Bad args, quit 
      else {
         global_n = atoi(argv[1]);
         if (global_n % comm_sz != 0)
            global_n = -1;
      }
   }

   MPI_Bcast(&g_i, 1, MPI_CHAR, 0, comm);
   MPI_Bcast(&global_n, 1, MPI_INT, 0, comm);

   if (global_n <= 0) {
      MPI_Finalize();
      exit(-1);
   }

   local_n = global_n/comm_sz;
   // printf("Proc %d, global_n = %d, local_n = %d\n",
   //    rank, global_n, local_n);   
 
   local_keys = (int*) malloc(local_n*sizeof(int));
   Generate_list(local_keys, local_n, rank);

   start = MPI_Wtime();
   Sort(local_keys, local_n, rank, comm_sz, comm);
   finish = MPI_Wtime();
   loc_elapsed = finish-start;
   MPI_Reduce(&loc_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

   // Print_local_lists(local_keys, local_n, rank, comm_sz, comm);

   // Print_global_list(local_keys, local_n, rank, comm_sz, comm);
   if (rank == 0)
      printf("Elapsed time = %e\n", elapsed);

   free(local_keys);

   MPI_Finalize();

   return 0;
} 


 // Fill list with random ints
void Generate_list(int local_keys[], int local_n, int rank) {
   int i;
   srandom(rank+1);
   for (i = 0; i < local_n; i++)
      local_keys[i] = random() % MAX;

}

void Print_global_list(int local_keys[], int local_n, int rank, int comm_sz, 
      MPI_Comm comm) {
   int* A;
   int i, n;

   if (rank == 0) {
      n = comm_sz*local_n;
      A = (int*) malloc(n*sizeof(int));
      MPI_Gather(local_keys, local_n, MPI_INT, A, local_n, MPI_INT, 0,
            comm);
      printf("Global list:\n");
      for (i = 0; i < n; i++)
         printf("%d ", A[i]);
      printf("\n\n");
      free(A);
   } else {
      MPI_Gather(local_keys, local_n, MPI_INT, A, local_n, MPI_INT, 0,
            comm);
   }

} 

int Compare(const void* _a, const void* _b) 
{
   int a = *((int*)_a);
   int b = *((int*)_b);

   if (a == b) return 0;
   else if (a < b)   return -1;
   else return 1;
} 

// Purpose:Sort local list, use odd-even sort to sort global list
void Sort(int local_keys[], int local_n, int rank, int comm_sz, MPI_Comm comm) 
{
   int phase;
   int *temp_B, *temp_C;
   int partner = 0;

   // Temporary storage used in merge-split 
   temp_B = (int*) malloc(local_n*sizeof(int));
   temp_C = (int*) malloc(local_n*sizeof(int));

   // Sort local list using built-in quick sort 
   qsort(local_keys, local_n, sizeof(int), Compare);

   for (phase = 0; phase < comm_sz; phase++) {
      Odd_even_iter(local_keys, temp_B, temp_C, local_n, phase, 
            partner, rank, comm_sz, comm);
   }
   free(temp_B);
   free(temp_C);
}  

// Find partners:  negative rank; do nothing during phase 
int Compute_partner(int phase, int rank, int comm_sz)
{
   int partner;
   if (phase % 2 == 0){
      if (rank % 2 != 0) {
         partner = rank - 1;
      } else {
         partner = rank + 1; 
      }
   } else{
      if (rank % 2 != 0) {
         partner = rank + 1;
      } else {
         partner = rank-1;  
      }
   }
   if (partner == -1 || partner == comm_sz) 
      partner = MPI_PROC_NULL;  // Idle during odd phase
      
   return partner;
}

// Purpose: One iteration of Odd-even transposition sort
void Odd_even_iter(int local_keys[], int temp_B[], int temp_C[],
        int local_n, int phase, int partner, 
        int rank, int comm_sz, MPI_Comm comm) 
{
   partner = Compute_partner(phase, rank, comm_sz);
   MPI_Status status;
   MPI_Sendrecv(local_keys, local_n, MPI_INT, partner, 0, 
            temp_B, local_n, MPI_INT, partner, 0, comm,
            &status);
   if (partner != MPI_PROC_NULL){
      if (rank < partner) {
         Merge_low(local_keys, temp_B, temp_C, local_n);
      } else {
         Merge_high(local_keys, temp_B, temp_C, local_n);  
      }
   }
} 

 // Merge the smallest local_n elements in local_keys and recv_keys into temp_keys.
 // Then copy temp_keys back into local_keys.
void Merge_low(
      int  local_keys[],  // in/out    
      int  recv_keys[],   // in        
      int  temp_keys[],   // scratch   
      int  local_n)        // = n/p, in  
{
   int m_i, r_i, t_i;
   
   m_i = r_i = t_i = 0;
   while (t_i < local_n) {
      if (local_keys[m_i] <= recv_keys[r_i]) {
         temp_keys[t_i] = local_keys[m_i];
         t_i++; m_i++;
      } else {
         temp_keys[t_i] = recv_keys[r_i];
         t_i++; r_i++;
      }
   }
   //copy temp_keys to local_keys
   memcpy(local_keys, temp_keys, local_n*sizeof(int));
} 

 // Merge the largest local_n elements in local_keys and temp_B into temp_C.
 // Then copy temp_C back into local_keys.
void Merge_high(int local_keys[], int temp_B[], int temp_C[], 
        int local_n) 
{
   int m_i, r_i, t_i;
   
   m_i = r_i = t_i = local_n-1;
   while (t_i >= 0) {
      if (local_keys[m_i] >= temp_B[r_i]) {
         temp_C[t_i] = local_keys[m_i];
         t_i--; m_i--;
      } else {
         temp_C[t_i] = temp_B[r_i];
         t_i--; m_i--;
      }
   }

   memcpy(local_keys, temp_C, local_n*sizeof(int));
}  

// Only called by process 0
void Print_list(int local_keys[], int local_n, int rank) {
   int i;
   printf("%d: ", rank);
   for (i = 0; i < local_n; i++)
      printf("%d ", local_keys[i]);
   printf("\n");
} 

// Print each process' current list contents
void Print_local_lists(int local_keys[], int local_n, 
         int rank, int comm_sz, MPI_Comm comm) 
{
   int*       A;
   int        q;
   MPI_Status status;

   if (rank == 0) {
      A = (int*) malloc(local_n*sizeof(int));
      Print_list(local_keys, local_n, rank);
      for (q = 1; q < comm_sz; q++) {
         MPI_Recv(A, local_n, MPI_INT, q, 0, comm, &status);
         Print_list(A, local_n, q);
      }
      free(A);
   } else {
      MPI_Send(local_keys, local_n, MPI_INT, 0, 0, comm);
   }
}  
