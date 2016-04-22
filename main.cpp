//Ally Warner - u0680103
//Assignment 4
//High Performance Computing - CS 6230
//Bitonic Sort

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include "mpi.h"

#define MASTER 0
#define OUTPUT_NUM 10

void SequentialSort(void);
void CompareLow(int bit);
void CompareHigh(int bit);
int ComparisonFunc(const void * a, const void * b);

double timer_start;
double timer_end;
int process_rank;
int num_processes;
int * array;
int array_size;

//Main Function
int main(int argc, char * argv[]) {
    int i, j;
    
    //Initialize
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
    
    //Initialize Array for Storing Random Numbers
    array_size = atoi(argv[1]) / num_processes;
    array = new int[array_size];
    
    // Generate Random Numbers for Sorting (within each process)
    srand(time(NULL));  // Needed for rand()
    for (i = 0; i < array_size; i++) {
        array[i] = rand() % (atoi(argv[1]));
    }
    
    // Blocks until all processes have finished generating
    MPI_Barrier(MPI_COMM_WORLD);
    
    int dimensions = (int)(log2(num_processes));
    
    // Start Timer
    if (process_rank == MASTER) {
        printf("Number of Processes spawned: %d\n", num_processes);
        timer_start = MPI_Wtime();
    }
    
    // Sequential Sort
    qsort(array, array_size, sizeof(int), ComparisonFunc);
    
    // Bitonic Sort
    for (i = 0; i < dimensions; i++) {
        for (j = i; j >= 0; j--) {
            if (((process_rank >> (i + 1)) % 2 == 0 && (process_rank >> j) % 2 == 0) || ((process_rank >> (i + 1)) % 2 != 0 && (process_rank >> j) % 2 != 0)) {
                CompareLow(j);
            } else {
                CompareHigh(j);
            }
        }
    }
    
    // Blocks until all processes have finished sorting
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (process_rank == MASTER) {
        timer_end = MPI_Wtime();
        
//        printf("Displaying sorted array (only 10 elements for quick verification)\n");
//        
//        // Print Sorting Results
//        for (i = 0; i < array_size; i++) {
//            if ((i % (array_size / OUTPUT_NUM)) == 0) {
//                printf("%d ",array[i]);
//            }
//        }
//        printf("\n\n");
        
        printf("Sorting Time (Sec): %f\n", timer_end - timer_start);
    }
    
    free(array);
    
    MPI_Finalize();
    return 0;
}

// Comparison Function
int ComparisonFunc(const void * a, const void * b) {
    return ( * (int *)a - * (int *)b );
}

// Compare Low
void CompareLow(int j) {
    int i, min;
    
    /* Sends the biggest of the list and receive the smallest of the list */
    
    // Send entire array to paired H Process
    // Exchange with a neighbor whose (d-bit binary) processor number differs only at the jth bit.
    int send_counter = 0;
    int * buffer_send = new int[array_size+1];
    MPI_Send(&array[array_size - 1],1,MPI_INT,process_rank ^ (1 << j),0,MPI_COMM_WORLD);
    
    // Receive new min of sorted numbers
    int recv_counter;
    int * buffer_recieve = new int[array_size+1];
    MPI_Recv(&min,1,MPI_INT,process_rank ^ (1 << j),0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    
    // Buffers all values which are greater than min send from H Process.
    for (i = 0; i < array_size; i++) {
        if (array[i] > min) {
            buffer_send[send_counter + 1] = array[i];
            send_counter++;
        } else {
            break;
        }
    }
    
    buffer_send[0] = send_counter;
    
    // send partition to paired H process
    MPI_Send(buffer_send,send_counter,MPI_INT,process_rank ^ (1 << j),0,MPI_COMM_WORLD);
    
    // receive info from paired H process
    MPI_Recv(buffer_recieve,array_size,MPI_INT,process_rank ^ (1 << j),0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    
    // Take received buffer of values from H Process which are smaller than current max
    for (i = 1; i < buffer_recieve[0] + 1; i++) {
        if (array[array_size - 1] < buffer_recieve[i]) {
            // Store value from message
            array[array_size - 1] = buffer_recieve[i];
        } else {
            break;
        }
    }
    
    // Sequential Sort
    qsort(array, array_size, sizeof(int), ComparisonFunc);
    
    // Reset the state of the heap from Malloc
    free(buffer_send);
    free(buffer_recieve);
    
    return;
}

// Compare High
void CompareHigh(int j) {
    int i, max;
    
    // Receive max from L Process's entire array
    int recv_counter;
    int * buffer_recieve = new int[array_size + 1];
    MPI_Recv(&max,1,MPI_INT,process_rank ^ (1 << j),0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    
    // Send min to L Process of current process's array
    int send_counter = 0;
    int * buffer_send = new int[array_size+1];
    MPI_Send(&array[0],1,MPI_INT,process_rank ^ (1 << j),0,MPI_COMM_WORLD);
    
    // Buffer a list of values which are smaller than max value
    for (i = 0; i < array_size; i++) {
        if (array[i] < max) {
            buffer_send[send_counter + 1] = array[i];
            send_counter++;
        } else {
            break;
        }
    }
    
    // Receive blocks greater than min from paired slave
    MPI_Recv(buffer_recieve,array_size,MPI_INT,process_rank ^ (1 << j),0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    
    recv_counter = buffer_recieve[0];
    
    // send partition to paired slave
    buffer_send[0] = send_counter;
    MPI_Send(buffer_send,send_counter,MPI_INT,process_rank ^ (1 << j),0,MPI_COMM_WORLD);
    
    // Take received buffer of values from L Process which are greater than current min
    for (i = 1; i < recv_counter + 1; i++) {
        if (buffer_recieve[i] > array[0]) {
            array[0] = buffer_recieve[i];
        } else {
            break;
        }
    }
    
    // Sequential Sort
    qsort(array, array_size, sizeof(int), ComparisonFunc);
    
    free(buffer_send);
    free(buffer_recieve);
    
    return;
}