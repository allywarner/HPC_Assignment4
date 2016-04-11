//Ally Warner - u0680103
//Assignment 4
//High Performance Computing - CS 6230
//Bitonic Sort

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <cstring>
#include <mpi.h>

using namespace std;

//MAIN FUNCTION
//Inputs:
int main(int argc, char* argv[]){
    
    //Initialize
    MPI_Init(&argc,&argv);
    
    //Getting the rank and the size
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    MPI_Finalize();
}