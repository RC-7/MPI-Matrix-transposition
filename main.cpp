#include <stdlib.h>
#include <time.h>
#include <vector>
#include <stdio.h>
#include <memory>
#include <omp.h>
#include <pthread.h>
#include "diag.cpp"
#include "block.cpp"
#include "naive.cpp"

#define THREAD_NUM 4

std::vector<std::vector<int>> generateRandom2D(int n)
{

    std::vector<std::vector<int>> randomMatrix(n, std::vector<int>(n, 0));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            randomMatrix[i][j] = rand() % 21;
        }
    }

    return randomMatrix;
}

void print2D(std::vector<std::vector<int>> A)
{
    printf("\n");
    for (auto &value : A)
    {

        for (auto &entry : value)
        {

            printf("%d  \t", entry);
        }

        printf("\n");
    }
}

bool checkDimension(int aLength, int aWidth)
{
    if (aLength == aWidth)
    {
        return true;
    }

    return false;
}

int main()
{
    srand(time(NULL));

    auto N = {128, 1024, 2048, 4096};

    for (auto n : N)
    {
        auto matrix = generateRandom2D(n);
        auto matrixPtr = &matrix;
        printf("Initial Matrix \n");
        // print2D(matrix);

        if (checkDimension(matrix.size(), matrix[0].size()))
        {
            auto outputMatrix = matrix;

            double begin, end, timeDiff;
            printf("Transposition times for matrix of size %d \n", n);
            printf("Naive Transpose without Threading \n");
            begin = omp_get_wtime();
            naiveTranspose(matrixPtr, &outputMatrix);
            end = omp_get_wtime();
            double naiveNoTime = end-begin;

            printf("Diagonal Transpose without Threading \n");
            begin = omp_get_wtime();
            diagTranspose(matrixPtr);
            end = omp_get_wtime();
            double diagNoTime = end-begin;

            printf("Block Transpose without Threading \n");
            begin = omp_get_wtime();
            int startX=0;
            int startY=0;
            elementBlockTranspose(matrixPtr, n, startX, startY);
            allBlocksTranspose(matrixPtr, n);
            end = omp_get_wtime();
            double blockNoTime = end-begin;


            printf("Naive Transpose with PThreads \n");
            pthread_t threads[THREAD_NUM];
            begin = omp_get_wtime();

            // setup threads
            std::vector<threadStructInput> threadStructVec;
            for (int i = 0; i < THREAD_NUM; i++)
            {
                threadStructVec.push_back(threadStructInput());
                threadStructVec[i].matrixPtr = matrixPtr;
                threadStructVec[i].outputPtr = &outputMatrix;
                threadStructVec[i].numberThreads = THREAD_NUM;
                threadStructVec[i].id = i;
            }

            // create threads and do diagonal transpose
            for (int i = 0; i < THREAD_NUM; ++i)
            {
                pthread_create(&threads[i], NULL, pthreadDiagTranspose, &threadStructVec[i]);
            }

            // join all threads
            for (int j = 0; j < THREAD_NUM; ++j)
                pthread_join(threads[j], NULL);

            end = omp_get_wtime();
            double naivePTime = end-begin;

            printf("Diagonal Transpose with PThreads \n");
            begin = omp_get_wtime();
            
             // setup threads
            std::vector<threadStruct> structVec;
            for (int i = 0; i < THREAD_NUM; i++)
            {
                structVec.push_back(threadStruct());
                structVec[i].matrixPtr = matrixPtr;
                structVec[i].numberThreads = THREAD_NUM;
                structVec[i].id = i;
            }

            // create threads and do diagonal transpose
            for (int i = 0; i < THREAD_NUM; ++i)
            {
                pthread_create(&threads[i], NULL, pthreadDiagTranspose, &structVec[i]);
            }

            // join all threads
            for (int j = 0; j < THREAD_NUM; ++j)
                pthread_join(threads[j], NULL);

            end = omp_get_wtime();
            double diagPTime = end-begin;

            printf("Block Transpose with PThreads \n");

            // setup threads
            std::vector<threadStructBlock> structVecBlock;
            for (int i = 0; i < THREAD_NUM; i++)
            {
                structVecBlock.push_back(threadStructBlock());
                structVecBlock[i].matrixPtr = matrixPtr;
                structVecBlock[i].numberThreads = THREAD_NUM;
                structVecBlock[i].id = i;
                structVecBlock[i].subBlockSize=n/2;
                structVecBlock[i].startX=0;
                structVecBlock[i].startY=0;
            }


            for (int i=0;i<2;i++)//thread 0: 0 0; thread 1: 0 n/2
            {
                structVecBlock[i].startX=0;
                structVecBlock[i].startY=i*n/2;
            }

            for (int i=2;i<4;i++)
            {
                structVecBlock[i].startX=n/2;
                structVecBlock[i].startY=(i-2)*n/2;
            }
          
            begin = omp_get_wtime();
            //First Transposition
            for (int i = 0; i < THREAD_NUM; ++i)
            {
                pthread_create(&threads[i], NULL, elementBlockTransposeThread, &structVecBlock[i]);
            }
           
            for (int j = 0; j < THREAD_NUM; ++j)
                pthread_join(threads[j], NULL);

            for (int i = 0; i < THREAD_NUM; ++i)
            {
                pthread_create(&threads[i], NULL,lastTranspose, &structVecBlock[i]);
            }

            for (int j = 0; j < THREAD_NUM; ++j)
                pthread_join(threads[j], NULL);
            
            end = omp_get_wtime();
            double blockPTime = end-begin;
            
    
            printf("Naive Transpose with OpenMP \n");
            begin = omp_get_wtime();
            ompNaiveTranspose(matrixPtr, &outputMatrix);
            end = omp_get_wtime();
            double naiveOTime = end-begin;

            printf("Diagonal Transpose with OpenMP \n");
            begin = omp_get_wtime();
            ompDiagTranspose(matrixPtr);
            end = omp_get_wtime();
            double diagOTime = end-begin;

            printf("Block Transpose with OpenMP \n");
            startX=0;
            startY=0;
            begin = omp_get_wtime();
            elementBlockTransposeOMP(matrixPtr,n, startX, startY);
            allBlocksTransposeOMP(matrixPtr,n);
            end = omp_get_wtime();
            double blockOTime = end-begin;


            
            printf("\nNaive with no threading %f s\n", naiveNoTime);
            printf("Diagonal with no threading %f s\n", diagNoTime);
            printf("Block with no threading %f s\n", blockNoTime);

            printf("\nNaive with pthreads %f s\n", naivePTime);
            printf("Diagonal with pthreads %f s\n", diagPTime);
            printf("Block with pthreads %f s\n", blockPTime);

            printf("\nNaive with openMP %f s\n", naiveOTime);
            printf("Diagonal with openMP %f s\n", diagOTime);
            printf("Block with openMP %f s\n\n\n", blockOTime);
        }
        else
        {
            printf("Error: matrix needs to be square");
        }
    }

    return 0;
}