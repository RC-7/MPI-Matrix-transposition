#include <vector>
#include <stdio.h>
#include <omp.h>

typedef struct threadStruct {
   std::vector<std::vector<int>>*  matrixPtr;
   int id;
   int numberThreads;
} threadStruct;

void diagTranspose(std::vector<std::vector<int>>* matrixPtr)
{
    auto n = (*matrixPtr).size();

    for (auto i = 0; i < n; ++i)
    {
        for (auto j = i+1; j < n; ++j)
        {
            (*matrixPtr)[i][j] = (*matrixPtr)[i][j] + (*matrixPtr)[j][i];
            (*matrixPtr)[j][i] = (*matrixPtr)[i][j] - (*matrixPtr)[j][i];
            (*matrixPtr)[i][j] = (*matrixPtr)[i][j] - (*matrixPtr)[j][i];
        }
    }
}

void* pthreadDiagTranspose(void* threadStructReceived)
{
    threadStruct* ts = (threadStruct*)threadStructReceived;
    auto n = (*(ts->matrixPtr)).size();

    for (auto i = ts->id; i < n; i += ts->numberThreads)
    {
        for (auto j = i+1; j < n; ++j)
        {
            (*(ts->matrixPtr))[i][j] = (*(ts->matrixPtr))[i][j] + (*(ts->matrixPtr))[j][i];
            (*(ts->matrixPtr))[j][i] = (*(ts->matrixPtr))[i][j] - (*(ts->matrixPtr))[j][i];
            (*(ts->matrixPtr))[i][j] = (*(ts->matrixPtr))[i][j] - (*(ts->matrixPtr))[j][i];
        }
    }

    pthread_exit(NULL);
}

void ompDiagTranspose(std::vector<std::vector<int>>* matrixPtr)
{
    auto n = (*matrixPtr).size();

    #pragma omp parallel for
    for (auto i = 0; i < n; ++i)
    {
        #pragma omp parallel for
        for (auto j = i+1; j < n; ++j)
        {
            (*matrixPtr)[i][j] = (*matrixPtr)[i][j] + (*matrixPtr)[j][i];
            (*matrixPtr)[j][i] = (*matrixPtr)[i][j] - (*matrixPtr)[j][i];
            (*matrixPtr)[i][j] = (*matrixPtr)[i][j] - (*matrixPtr)[j][i];
        }
    }
}