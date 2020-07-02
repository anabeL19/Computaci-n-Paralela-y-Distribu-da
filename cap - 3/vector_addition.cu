#include <stdio.h>

__global__
void vecAddKernel(float *A, float *B, float *C, int n){
  int i = threadIdx.x + blockDim.x * blockIdx.x;
  if(i < n) C[i] = A[i] + B[i];
}

void vecAdd(float* A, float* B, float* C, int n){
  int size = n*sizeof(float);
  float *d_A, *d_B, *d_C;

  cudaMalloc((void **) &d_A, size);
  cudaMemcpy(d_A,A,size,cudaMemcpyHostToDevice);
  cudaMalloc((void **) &d_B, size);
  cudaMemcpy(d_B,B,size,cudaMemcpyHostToDevice);
  cudaMalloc((void **) &d_C, size);

  vecAddKernel<<<ceil(n/256.0), 256>>>(d_A,d_B,d_C,n);
  
  cudaMemcpy(C,d_C,size,cudaMemcpyDeviceToHost);

  cudaFree(d_A); cudaFree(d_B); cudaFree(d_C);
}


int main(){
  int n,i;
  float *h_A,*h_B,*h_C;
  scanf("%d", &n);
  h_A = (float*) malloc(n*sizeof(float));
  h_B = (float*) malloc(n*sizeof(float));
  h_C = (float*) malloc(n*sizeof(float));
  for(i = 0; i < n; i++){
    //scanf("%f", &h_A[i]);
    h_A[i] = 1;
  }
  for(i = 0; i < n; i++){
    //scanf("%f", &h_B[i]);
    h_B[i] = 1;
  }
  vecAdd(h_A,h_B,h_C,n);
  /*for(i = 0; i < n; i++){
    printf("%f ", h_C[i]);
  }*/
  printf("\n");
  return 0;
}
