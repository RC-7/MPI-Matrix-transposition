
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <stdio.h>
#include <memory>
#include <pthread.h>
#include <mpi.h>
#include <fstream>


const int n=64;
const int subBlockSize=n/4;
bool finishedTranspose=false;
typedef struct BlockStruct {
   int blockVec[subBlockSize];
   int block;
} BlockStruct;


void generateRandom1D(int original[subBlockSize])
{

    for (int i = 0; i < subBlockSize; i++)
    {

            original[i]= rand() % 21;
    }

}

void transpose(int original[subBlockSize]){

	int transposed[subBlockSize];
	int count=0;

	for(int i=0;i<subBlockSize/4;i++){
		for(int j=i;j<subBlockSize;j+=subBlockSize/4){
			transposed[count]=original[j];
			count++;

		}

	}

	for (int i =0;i<subBlockSize;i++){
		original[i]=transposed[i];
	}


}

void writeToFIle(std::vector<BlockStruct> vectorOfBlocks, std::string filename){

	std::ofstream outfile;
	outfile.open(filename);
	outfile<<n;
	outfile<<"\n";
	
	if (!finishedTranspose){
		for (int i=0;i<subBlockSize/4;i++){
			for (int k=0;k<2;k++){
			for (int j =i*subBlockSize/4;j<i*subBlockSize/4+subBlockSize/4;j++)
			{
				outfile<<vectorOfBlocks[k].blockVec[j];
				outfile<<" ";
			}
			
		}

	}
		for (int i=0;i<subBlockSize/4;i++){
			for (int k=2;k<4;k++){
			for (int j =i*subBlockSize/4;j<i*subBlockSize/4+subBlockSize/4;j++)
			{
				outfile<<vectorOfBlocks[k].blockVec[j];
				outfile<<" ";
			}
		}

	}
}
else {
		for (int i=0;i<subBlockSize/4;i++){
			for (int k=0;k<3;k+=2){
			for (int j =i*subBlockSize/4;j<i*subBlockSize/4+subBlockSize/4;j++)
			{
				outfile<<vectorOfBlocks[k].blockVec[j];
				outfile<<" ";
			}
			
		}

	}
		for (int i=0;i<subBlockSize/4;i++){
			for (int k=1;k<4;k+=2){
			for (int j =i*subBlockSize/4;j<i*subBlockSize/4+subBlockSize/4;j++)
			{
				outfile<<vectorOfBlocks[k].blockVec[j];
				outfile<<" ";
			}
		}

	}



}

outfile.close();

}




int main(int argc, char *argv[]){
	MPI_Status status;
	srand(time(NULL));
	int rank, size;
    MPI_Init(&argc, &argv); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

     std::vector<BlockStruct> vectorOfBlocks;
            for (int i = 0; i < 4; i++)
            {
                vectorOfBlocks.push_back(BlockStruct());
                generateRandom1D(vectorOfBlocks[i].blockVec);

                vectorOfBlocks[i].block = i;
            }
 for (int i =0;i<4;i++){
 	for (int j=0;j<16;j++){

 		std::cout<<vectorOfBlocks[i].blockVec[j]<<" ";

 	}

 	std::cout<<std::endl;
 }

 std::cout<<"--------------------"<<std::endl;

  writeToFIle(vectorOfBlocks,"input.txt");


    struct { int a; double b;} value;
    MPI_Datatype mystruct;
    int          blocklens[2];
    MPI_Aint     indices[2];
    MPI_Datatype old_types[2];


    blocklens[0] = 16;
    blocklens[1] = 1;

    old_types[0] = MPI_INT;
    old_types[1] = MPI_INT;

    MPI_Address( &vectorOfBlocks[0].blockVec, &indices[0] );
    MPI_Address( &vectorOfBlocks[0].block, &indices[1] );

    indices[1] = indices[1] - indices[0];
    indices[0] = 0;
    MPI_Type_struct( 2, blocklens, indices, old_types, &mystruct );
    MPI_Type_commit( &mystruct );

if (rank==0){


    
    MPI_Send( &vectorOfBlocks[1], 1, mystruct, 1,13, MPI_COMM_WORLD );
    transpose(vectorOfBlocks[0].blockVec);

    MPI_Recv(&vectorOfBlocks[1],   1, mystruct, 1, 13, MPI_COMM_WORLD, &status);
        for (int i =0;i<16;i++){

 		std::cout<<vectorOfBlocks[1].blockVec[i]<<" ";



 }

    MPI_Send( &vectorOfBlocks[2], 1, mystruct, 1,13, MPI_COMM_WORLD );
    transpose(vectorOfBlocks[3].blockVec);
    MPI_Recv(&vectorOfBlocks[2],   1, mystruct, 1, 13, MPI_COMM_WORLD, &status);
    finishedTranspose=true;
    writeToFIle(vectorOfBlocks,"output.txt");


}
else {


	BlockStruct recv;
	


    MPI_Recv(&recv,   1, mystruct, 0, 13, MPI_COMM_WORLD, &status);

 std::cout<<std::endl;
    transpose(recv.blockVec);


std::cout<<std::endl;
 MPI_Send( &recv, 1, mystruct, 0,13, MPI_COMM_WORLD );


    MPI_Recv(&recv,   1, mystruct, 0, 13, MPI_COMM_WORLD, &status);

    transpose(recv.blockVec);
    MPI_Send( &recv, 1, mystruct, 0,13, MPI_COMM_WORLD );



}
 MPI_Finalize();
    return 0;

}