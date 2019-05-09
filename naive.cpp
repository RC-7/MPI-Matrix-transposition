#include <vector>
#include <stdio.h>
#include <omp.h>
#include <pthread.h>

typedef struct threadStructInput {
   std::vector<std::vector<int>>*  matrixPtr;
   std::vector<std::vector<int>>*  outputPtr;
   int id;
   int numberThreads;
} threadStructInput;

void naiveTranspose(std::vector<std::vector<int>>* matrixPtr, std::vector<std::vector<int>>* outputPtr){
    auto n = (*matrixPtr).size();

    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            (*outputPtr)[j][i] = (*matrixPtr)[i][j];
        }
    }
}

void *pthreadNaiveTranspose(void *threadStructReceived){
    auto *ts = (threadStructInput*)threadStructReceived;
    auto n = (*(ts->matrixPtr)).size();

    for (int i = ts->id; i < n; i += ts->numberThreads){
        for (int j = 0; j < n; j++){
            (*(ts->outputPtr))[i][j] = (*(ts->matrixPtr))[j][i];
        }
    }

    pthread_exit(NULL);
}

void ompNaiveTranspose(std::vector<std::vector<int>>* matrixPtr, std::vector<std::vector<int>>* outputPtr){
    auto n = (*matrixPtr).size();

    #pragma omp parallel for
    for (int i = 0; i < n; i++){

        #pragma omp parallel for
        for (int j = 0; j < n; j++){
            (*outputPtr)[j][i] = (*matrixPtr)[i][j];
        }
    }
}