
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <stdio.h>
#include <memory>
#include <pthread.h>
#include <mpi.h>

typedef struct BlockStruct {
   // std::vector<std::vector<int> >  blockVec;
   int blockVec[16];
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
	int n=64;
	srand(time(NULL));
	int rank, size;
    MPI_Init(&argc, &argv); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    int test[16];
    for (int i=0;i<16;i++){
    	test[i]=1;
    }
     std::vector<BlockStruct> vectorOfBlocks;
            for (int i = 0; i < 4; i++)
            {
                vectorOfBlocks.push_back(BlockStruct());
                // vectorOfBlocks[i].blockVec = generateRandom2D(n/4);
                // vectorOfBlocks[i].blockVec = test;
                for (int i=0;i<16;i++){
    				vectorOfBlocks[i].blockVec[i]=1;
    				}
                vectorOfBlocks[i].block = i;
            }

// int          rank;
    struct { int a; double b;} value;
    MPI_Datatype mystruct;
    int          blocklens[2];
    MPI_Aint     indices[2];
    MPI_Datatype old_types[2];

    // MPI_Init( &argc, &argv );

    // MPI_Comm_rank( MPI_COMM_WORLD, &rank );

    /* One value of each type */
    blocklens[0] = 16;
    blocklens[1] = 1;
    /* The base types */
    old_types[0] = MPI_INT;
    old_types[1] = MPI_INT;
    /* The locations of each element */
    MPI_Address( &vectorOfBlocks[0].blockVec, &indices[0] );
    MPI_Address( &vectorOfBlocks[0].block, &indices[1] );
    /* Make relative */
    indices[1] = indices[1] - indices[0];
    indices[0] = 0;
    MPI_Type_struct( 2, blocklens, indices, old_types, &mystruct );
    MPI_Type_commit( &mystruct );

if (rank==0){


	
	




    
    MPI_Send( &vectorOfBlocks[0], 1, mystruct, 1,13, MPI_COMM_WORLD );


 // for (int i =0;i<4;i++){
 // 	for (int j=0;j<4;j++){
 // 		std::cout<<vectorOfBlocks[0].blockVec[i][j]<<" ";

 // 	}
 // 	std::cout<<std::endl;


 // }
 // MPI_Send(&matrixres[i][j],1,MPI_FLOAT,0,5,MPI_COMM_WORLD);



}
else {

	std::cout<<"Yes"<<std::endl;
	BlockStruct recv;
	MPI_Status status;

    MPI_Recv(&recv,   1, mystruct, 0, 13, MPI_COMM_WORLD, &status);
    std::cout<<recv.blockVec[0]<<" ";

    for (int i =0;i<16;i++){
 // 	// for (int j=0;j<4;j++){
 		std::cout<<recv.blockVec[i]<<" ";

 // 	// }
 // 	std::cout<<std::endl;


 }

}
 MPI_Finalize();
    return 0;

}