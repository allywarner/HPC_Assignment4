//Ally Warner - u0680103
//Assignment 4
//High Performance Computing - CS 6230
//Bitonic Sort

#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <mpi.h>
#include <string.h>

using namespace std;
void SequentialSort(void);
void CompareLow(int bit);
void CompareHigh(int bit);
int ComparisonFunc(const void * a, const void * b);

//Main Function
//Inputs:
int main(int argc, char* argv[]){
    
    //Initialize
    MPI_Init(&argc,&argv);
    
    //Throws an error
    if (argc < 2){
        cerr << "Error. Please input the size of the array to be sorted.";
        return 1;
    }
    
    //Get the array's length to be sorted.
    int arrayLength = atoi(argv[1]);
    
    //Getting the rank and the size
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    //Generate random array of the given length
    int *array = new int[arrayLength];
    for(int i = 0,i < arrayLength,i++){
        array[i] = rand()*100+1;
        cout << array[i] << endl;
    }
    
    MPI_Finalize();
}