// #include <mpi.h>
// #include <stdio.h>

// int main(int argc, char** argv) {
//     // Initialize the MPI environment
//     MPI_Init(NULL, NULL);

//     // Get the number of processes
//     int world_size;
//     MPI_Comm_size(MPI_COMM_WORLD, &world_size);

//     // Get the rank of the process
//     int world_rank;
//     MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

//     // Get the name of the processor
//     char processor_name[MPI_MAX_PROCESSOR_NAME];
//     int name_len;
//     MPI_Get_processor_name(processor_name, &name_len);
//     // printf("%i",world_rank);
//     // Print off a hello world message
//     printf("Hello world from processor %s, rank %d out of %d processors\n",
//            processor_name, world_rank, world_size);

//     printf("%i       \n",world_rank);

//     // Finalize the MPI environment.
//     MPI_Finalize();
// }
#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv); //to initial MPI with argument | ex. argument = 1, argc = 1, argv(vector) = "1"
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //ID of processor
    MPI_Comm_size(MPI_COMM_WORLD, &size); //amount of processor

    if(rank==0){
        double startTime,endTime;
        int colA,rowA,colB,rowB;
        int partcol,fraccol;
        int countrank;
        float **matrixA,**matrixB,**matrixres;
        int i,j;
        FILE *fileA;
        FILE *fileB;
        FILE *resfile;
        char *fileAname= "matAlarge.txt",*fileBname= "matBlarge.txt",*resfilename= "Resultlarge-block.txt";

        fileA = fopen(fileAname,"r");
        fileB = fopen(fileBname,"r");
        if(!fileA || !fileB)
            printf("File Not Found!\n");
        else{
            printf("Hooray!, File Founded.\n");

            fscanf(fileA, "%d %d",&rowA,&colA);
            fscanf(fileB, "%d %d",&rowB,&colB);

            partcol=(int)colA/size;
            fraccol=colA-(partcol*size);

            printf("%d %d %d %d\n",rowA,colA,partcol,fraccol);

            //allocate matrixA
            matrixA = (float**)malloc(sizeof(float*)*rowA);
            for (i = 0; i < rowA; i++)
                matrixA[i]=(float*)malloc(sizeof(float)*colA);

            //allocate matrixB
            matrixB = (float**)malloc(sizeof(float*)*rowB);
            for (i = 0; i < rowB; i++)
                matrixB[i]=(float*)malloc(sizeof(float)*colB);

            //allocate matrixResult & define 0 value
            matrixres = (float**)malloc(sizeof(float*)*rowA);
            for (i = 0; i < rowA; i++)
                matrixres[i]=(float*)malloc(sizeof(float)*colA);
            for(i = 0 ; i < rowA ; i++)
                for(j = 0; j < colA; j++)
                    matrixres[i][j]=0;

            //read matrixA
            while (!feof(fileA))
                for (i = 0; i < rowA; i++)
                    for (j = 0; j < colA; j++)
                        fscanf(fileA,"%f",&matrixA[i][j]);

            //read matrixB
            while (!feof(fileB))
            {
                for (i = 0; i < rowB; i++)
                    for (j = 0; j < colB; j++)
                        fscanf(fileB,"%f",&matrixB[i][j]);
            }

            //close file
            fclose(fileA);
            fclose(fileB);

            startTime = MPI_Wtime();

            //Addition in rank 0
            for(i = 0 ; i < rowA ; i++)
                for(j = 0; j < partcol; j++)
                    matrixres[i][j]=matrixA[i][j]+matrixB[i][j];
            printf("Rank 0 addition completed\n");

            if (size!=1)
            {
                //send detail to all rank
                for (countrank = 1; countrank < size; countrank++)
                {
                    MPI_Send(&rowA,1,MPI_INT,countrank,1,MPI_COMM_WORLD);
                    MPI_Send(&partcol,1,MPI_INT,countrank,2,MPI_COMM_WORLD);
                }

                //send part LOOP
                for (countrank = 1; countrank < size; countrank++)
                    for(i = 0 ; i < rowA ; i++)
                        for(j = partcol*countrank; j < partcol*(countrank+1); j++){
                            MPI_Send(&matrixA[i][j],1,MPI_FLOAT,countrank,3,MPI_COMM_WORLD);
                            MPI_Send(&matrixB[i][j],1,MPI_FLOAT,countrank,4,MPI_COMM_WORLD);
                        }
                printf("Rank 0 send matrix completed\n");

                //Addition fraction column in rank 0
                if(fraccol!=0){
                    for (i = 0; i < rowA; i++)
                        for (j = partcol*3; j < (partcol*3)+fraccol ; j++)
                            matrixres[i][j]=matrixA[i][j]+matrixB[i][j];
                }

                //receive matrix Res after other rank calcutate
                for (countrank = 1; countrank < size; countrank++)
                    for(i = 0 ; i < rowA ; i++)
                        for(j = partcol*countrank; j < partcol*(countrank+1); j++){
                            MPI_Recv(&matrixres[i][j],1,MPI_FLOAT,countrank,5,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                        }
            }

            endTime = MPI_Wtime();

            //write file
            resfile = fopen(resfilename,"w+");
            fprintf(resfile,"%d %d",rowA,colA);
            fprintf(resfile, "\n");
            for(i=0;i<rowA;i++){
                for(j=0;j<colA;j++)
                    fprintf(resfile, "%.1f ",matrixres[i][j]);
                fprintf(resfile, "\n");
            }
            fclose(resfile);
            printf("Successful! , Let's see result file.\n");
            printf("Timings : %f Sec\n", endTime - startTime);
        }
    }

    //OTHER RANK
    else{
        int i,j;
        int row,partcol;

        MPI_Recv(&row,1,MPI_INT,0,1,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&partcol,1,MPI_INT,0,2,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("recv %d %d via rank %d\n",row,partcol,rank);

        //alloc A B Res in other rank
        float **matrixA = (float**)malloc(sizeof(float*)*row);
        for (i = 0; i < row; i++)
            matrixA[i]=(float*)malloc(sizeof(float)*partcol);

        float **matrixB = (float**)malloc(sizeof(float*)*row);
        for (i = 0; i < row; i++)
            matrixB[i]=(float*)malloc(sizeof(float)*partcol);

        float **matrixres = (float**)malloc(sizeof(float*)*row);
        for (i = 0; i < row; i++)
            matrixres[i]=(float*)malloc(sizeof(float)*partcol);
        printf("alloc matrix from rank %d completed\n",rank);

        //Receive data in matrix array
        for(i = 0 ; i < row ; i++){
            for(j = 0; j < partcol; j++){
                MPI_Recv(&matrixA[i][j],1,MPI_FLOAT,0,3,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                MPI_Recv(&matrixB[i][j],1,MPI_FLOAT,0,4,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                matrixres[i][j]=matrixA[i][j]+matrixB[i][j];
            }
        }
        printf("recv matrix from rank %d completed\n",rank);

        //send matrix Res back to rank 0
        for(i = 0 ; i < row ; i++){
            for(j = 0; j < partcol; j++){
                MPI_Send(&matrixres[i][j],1,MPI_FLOAT,0,5,MPI_COMM_WORLD);
            }
        }
        printf("Rank %d send back completed\n",rank);
    }

    MPI_Finalize();
    return 0;
}