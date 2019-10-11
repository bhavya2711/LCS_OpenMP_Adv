#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <omp.h>
#include<chrono>
#include <algorithm>
using namespace std;
#ifdef __cplusplus
extern "C" {
#endif

  void generateLCS(char* X, int m, char* Y, int n);
  void checkLCS(char* X, int m, char* Y, int n, int result);


#ifdef __cplusplus
}
#endif

int main (int argc, char* argv[]) {

  //forces openmp to create the threads beforehand
#pragma omp parallel
  {
    int fd = open (argv[0], O_RDONLY);
 if (fd != -1) {
      close (fd);
    }
    else {
      std::cerr<<"something is amiss"<<std::endl;
    }
  }

  if (argc < 4) { std::cerr<<"usage: "<<argv[0]<<" <m> <n> <nbthreads>"<<std::endl;
    return -1;
  }
  //command line inputs
  int m = atoi(argv[1]);  
  int n = atoi(argv[2]);
  int nofThreads = atoi(argv[3]);
  int maximumLength = max(n,m);
  char *X = new char[maximumLength];
  char *Y = new char[maximumLength];

  generateLCS(X, m, Y, n);
  int result=0;

int C[maximumLength+1][maximumLength+1];
 omp_set_num_threads(nofThreads);//setting number of threads based on command line input

//staring clock
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

#pragma omp parallel
{
#pragma omp for
for(int i=0;i<=maximumLength;i++){
C[0][i]=0;

}

#pragma omp for
for(int i=0;i<=maximumLength;i++){
C[i][0]=0;

}
}
#pragma omp parallel
{
#pragma omp single
{
for(int k=1; k<=maximumLength;k++)
{
    if (X[k-1] == Y[k-1]){
            C[k][k] = C[k-1][k-1] + 1;
    }else{
        C[k][k] = max(C[k][k-1],C[k-1][k]);
    }
    #pragma omp task shared(X , Y, C, k, maximumLength)
    for(int j = k; j<=maximumLength;j++)
    {
        if (X[k-1] == Y[j]){
                C[k][j] = C[k-1][j-1] + 1;
        }else{
            C[k][j] = max(C[k][j-1],C[k-1][j]);
        }
    }

   #pragma omp task shared(X, Y, C, k, maximumLength)
    for(int i = k;i<=maximumLength;i++)
    {
        if (X[i] == Y[k-1]){
                C[i][k] = C[i-1][k-1] + 1;
        }else{
            C[i][k] = max(C[i][k-1],C[i-1][k]);
        }
    }

    #pragma omp taskwait

}

result = C[m][n];
}
}

checkLCS(X, m, Y, n, result);


  //stopping clock
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
  
  std::cerr<<elapsed_seconds.count()<<std::endl; //printing time taken in error stream
  return 0;
}
