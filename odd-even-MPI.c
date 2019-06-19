#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"


#define N 25
int hostCount = 5;



void show_array(int size,int *array,char* msg){
    printf("%s==>",msg);
    printf("\n{");
    for(int i = 0 ; i<  size;i++){
        printf(" %d ",array[i]);
        if(i < size-1)printf(",");
    }

    printf("}\n");

}

void generate_random_array(int size,int *array){
    
    time_t t;
    srand((unsigned) time(&t));
    for(int i  = 0; i < size ; i++){
        array[i] = (rand() % 50); 
    }

}

void sequentialSort(int *arrayToSort, int size)
{
    int sorted = 0;
    while (sorted == 0)
    {
        sorted = 1;
        int i;
        for (i = 1; i < size - 1; i += 2)
        {
            if (arrayToSort[i] > arrayToSort[i + 1])
            {
                int temp = arrayToSort[i + 1];
                arrayToSort[i + 1] = arrayToSort[i];
                arrayToSort[i] = temp;
                sorted = 0;
            }
        }
        for (i = 0; i < size - 1; i += 2)
        {
            if (arrayToSort[i] > arrayToSort[i + 1])
            {
                int temp = arrayToSort[i + 1];
                arrayToSort[i + 1] = arrayToSort[i];
                arrayToSort[i] = temp;
                sorted = 0;
            }
        }
    }
}
void lower(int *subArray,int *nextArray, int size){
    int i = 0;
    int fusionSize = size * 2 ;
    int *fusionArray = malloc(fusionSize* sizeof(int));

    for(i = 0; i  < fusionSize;i++){
        if(i < size){
        fusionArray[i] = subArray[i];

        }else{
        fusionArray[i] = nextArray[i-size];
        }
    }

    sequentialSort(fusionArray,fusionSize);
    
    //show_array(fusionSize,fusionArray,"fusion Array");

    //we keep the n/p low elements.
    for(i = 0; i  < size;i++){
        subArray[i] = fusionArray[i];
     
    }
    //show_array(size,subArray,"sub Array");

}
void higher(int *subArray,int *previousArray, int size){
    int i = 0;
    int fusionSize = size * 2 ;
    int *fusionArray = malloc(fusionSize* sizeof(int));

    for(i = 0; i  < fusionSize;i++){
        if(i < size){
        fusionArray[i] = subArray[i];

        }else{
        fusionArray[i] = previousArray[i-size];
        }
    }

    sequentialSort(fusionArray,fusionSize);
    
     int j = 0; 

    for(i = size; i  <= fusionSize;i++,j++){
        subArray[j] = fusionArray[i];
    }
    //show_array(size,subArray,"higher sub Array");

}

void exchangeWithNext(int *subArray, int size, int rank)
{
    

    MPI_Send(subArray, size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
   
    int *nextArray = malloc(size * sizeof(int));
    MPI_Status stat;
    MPI_Recv(nextArray, size, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &stat);
   
    lower(subArray, nextArray, size);
    free(nextArray);
}

void exchangeWithPrevious(int *subArray, int size, int rank){


    sequentialSort(subArray,N/hostCount);
  

  
    MPI_Send(subArray, size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);


  
    int *previousArray = malloc(size * sizeof(int));
    MPI_Status stat;
    MPI_Recv(previousArray, size, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &stat);

    higher(subArray, previousArray, size);
    free(previousArray);
}
int main(int argc, char* argv[])
{
    int rank, size;
  
    int *arrayToSort = malloc(N * sizeof(int));
    int *subArray = malloc(N / hostCount * sizeof(int));

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0){ 
        generate_random_array(N,arrayToSort);


        char *msg = "Main Array:";
        show_array(N,arrayToSort,msg);
        MPI_Scatter(arrayToSort, N / hostCount, MPI_INT, subArray, N / hostCount, MPI_INT, 0, MPI_COMM_WORLD);
    }

    int *displs = malloc(hostCount * sizeof(int));
    int i;
    for (i = 0; i < hostCount; i++)
    {
        displs[i] = i * (N / hostCount);
    }
    int *sendcnts = malloc(hostCount * sizeof(int));
    for (i = 0; i < hostCount; i++)
    {
        sendcnts[i] = N / hostCount;
    }

    MPI_Scatterv(arrayToSort, sendcnts, displs, MPI_INT,
     subArray, N / hostCount, MPI_INT, 0, MPI_COMM_WORLD);
    free(displs);  
    free(sendcnts);

    i = 0;
    for (i = 0; i < N; i++)
    {
        
        if (i % 2 == 0)
        {

    
char *msg = "pair ";
show_array(5,subArray,msg);

            if (rank % 2 == 0)
            {
                
                if (rank < hostCount - 1)
                {

                    exchangeWithNext(subArray, N / hostCount, rank);
                }
            }
            else
            {
                if (rank - 1 >= 0)
                {
         
                    exchangeWithPrevious(subArray, N / hostCount, rank);
                }
            }
        }
        else
        {


char *msg = "impair ";
show_array(5,subArray,msg);


            if (rank % 2 != 0)
            {
                if (rank < hostCount - 1)
                {
                   
                    exchangeWithNext(subArray, N / hostCount, rank);
                }
            }
            else
            {
            if (rank - 1 >= 0)
                {
                  
                    exchangeWithPrevious(subArray, N / hostCount, rank);
                }
            }
        }
    }

  
    MPI_Gather(subArray,N/hostCount,MPI_INT,arrayToSort,N/hostCount,MPI_INT,0,MPI_COMM_WORLD); 
    if(rank == 0){

    char *msg = "FINAL Sorted ARRAY===================>:"; 
    show_array(N,arrayToSort,msg);


    }


    MPI_Finalize();
    return 0;
}
