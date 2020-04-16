#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <chrono>
#include <iostream>


//  g++ six_bucles.cpp -o ms
using namespace std; 
void Multiply_matrices(int n, double** a, double** b, double** c)
{
    int bi=0, bj=0, bk=0;
    int i=0, j=0, k=0;

    int blockSize=20; 
    for(bi=0; bi<n; bi+=blockSize)
        for(bj=0; bj<n; bj+=blockSize)
            for(bk=0; bk<n; bk+=blockSize)
                for(i=0; i<blockSize; i++)
                    for(j=0; j<blockSize; j++)
                        for(k=0; k<blockSize; k++)
                            c[bi+i][bj+j] += a[bi+i][bk+k]*b[bk+k][bj+j];
}

// OPCIONAL
void Print_matrix(double ** mult, int row_first, int column_second)
{
  	int i, j;
   	cout << "print Matrix:\n";
   	for(i = 0; i < row_first; ++i){
   		for(j = 0; j < column_second; ++j){
   			cout << mult[i][j] << " ";
   			if(j == column_second - 1)
   				cout << "\n" << "\n";
   		}
   	}
}

int main(void)
{
    int n;
    double** first_matrix;
    double** second_matrix;
    double** result_matrix;
    int numreps = 5;
    int i=0;
    int j=0;
    struct timeval tv1, tv2;
    struct timezone tz;
    double elapsed;
    int t;
    printf ("Please enter matrix dimension n : ");
    scanf("%d", &n);
    // allocate memory for the matrices
     
    //********************** First Matrix ************************
     
    first_matrix =(double **)malloc(n*sizeof(double *));
    first_matrix[0] = (double *)malloc(n*n*sizeof(double));
    if(!first_matrix) {
        printf("memory failed- row \n");
        exit(1);
    }
    for(i=1; i<n; i++) {
        first_matrix[i] = first_matrix[0]+i*n;
        if (!first_matrix[i]) {
            printf("memory failed- col \n");
            exit(1);
        }
    }
 
    //********************** Second Matrix ************************

    second_matrix =(double **)malloc(n*sizeof(double *));
    second_matrix[0] = (double *)malloc(n*n*sizeof(double));
    if(!second_matrix) {
        printf("memory failed- row2 \n");
        exit(1);
    }
    for(i=1; i<n; i++) {
        second_matrix[i] = second_matrix[0]+i*n;
        if (!second_matrix[i]) {
            printf("memory failed- col2 \n");
            exit(1);
        }
    }
 
    //********************** Results Matrix ************************

    result_matrix =(double **)malloc(n*sizeof(double *));
    result_matrix[0] = (double *)malloc(n*n*sizeof(double));
    if(!result_matrix) {
        printf("memory failed \n");
        exit(1);
    }
    for(i=1; i<n; i++) {
        result_matrix[i] = result_matrix[0]+i*n;
        if (!result_matrix[i]) {
            printf("memory failed \n");
            exit(1);
        }
    }
 
    // initialize the matrices
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            first_matrix[i][j] = 1;
            second_matrix[i][j] = 2;
        }
    }

    cout<<"First matrix \n";
    // Print_matrix(first_matrix, i, j);
    
    cout<<"Second matrix \n";
    // Print_matrix(second_matrix, i, j);

    //multiply matrices
    printf("Multiply matrices %d times...\n", numreps);
    for (i=0; i<numreps; i++)
    {
        auto timer= std::chrono::high_resolution_clock::now();
        Multiply_matrices(n, first_matrix,second_matrix, result_matrix);
        auto timer2= std::chrono::high_resolution_clock::now();
        t = std::chrono::duration_cast<std::chrono::microseconds>( timer2 - timer ).count();
    }

    std::cout<<"TIEMPO: "<<  t <<"\n";     

    //deallocate memory
    free(first_matrix[0]);
    free(first_matrix);
    free(second_matrix[0]);
    free(second_matrix);
    free(result_matrix[0]);
    free(result_matrix);
    return 0;
}