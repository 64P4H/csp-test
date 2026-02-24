#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <mpi.h>

#include "DLinked.hpp"

#define NUM 100

int main(){
  srand(time(NULL));

  DLinkedList list(NUM);
  int commSize;
  int commRank;
  DLinkedElem* cur0;
  DLinkedElem* cur1;
  int res;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &commSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &commRank);

  if(commSize != 2){
    if(commRank == 0) {
      printf("This program is ment to be run with 2 threads\n");
    }
    MPI_Abort(MPI_COMM_WORLD, -1);
    exit(-1);
  }

  res = list.threadMagic(commRank, cur0, cur1);
  printf("Counted %i: %i\n", commRank, res);

  MPI_Barrier(MPI_COMM_WORLD);

  if(commRank == 0) {
    printf("Completed\n");
  }

  MPI_Finalize();

  exit(0);
}
