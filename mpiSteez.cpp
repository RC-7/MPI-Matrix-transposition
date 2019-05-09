
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <stdio.h>
#include <memory>
#include <pthread.h>
#include <mpi.h>

typedef struct BlockStruct {
   std::vector<std::vector<int> >  blockVec;
   int block;
} BlockStruct;


std::vector<std::vector<int> > generateRandom2D(int n)
{

    std::vector<std::vector<int> > randomMatrix(n, std::vector<int>(n, 0));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            randomMatrix[i][j] = rand() % 21;
        }
    }

    return randomMatrix;
}





int main(int argc, char *argv[]){
	int rank, size;
    MPI_Init(&argc, &argv); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 




if (rank==0){


	int n=64;
	srand(time(NULL));

     std::vector<BlockStruct> vectorOfBlocks;
            for (int i = 0; i < 4; i++)
            {
                vectorOfBlocks.push_back(BlockStruct());
                vectorOfBlocks[i].blockVec = generateRandom2D(n/4);
                vectorOfBlocks[i].block = i;
            }


 for (int i =0;i<4;i++){
 	for (int j=0;j<4;j++){
 		std::cout<<vectorOfBlocks[0].blockVec[i][j]<<" ";

 	}
 	std::cout<<std::endl;


 }
}
else {

	std::cout<<"Yes"<<std::endl;

}
 MPI_Finalize();
    return 0;

}