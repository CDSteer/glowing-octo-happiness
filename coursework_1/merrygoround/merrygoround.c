#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <malloc.h>
#include <mpi.h>
#define buffsize 1024*1024

int main(int argc, char **argv )
{
 int myid,numprocs;
 int i;
 int mbytes = 25;
 int indmax;
 double t1, t2;
 char *buffer;
 MPI_Status status;

 MPI_Init(&argc, &argv);
 MPI_Barrier(MPI_COMM_WORLD); 
 MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
 MPI_Comm_rank(MPI_COMM_WORLD, &myid); 
 MPI_Barrier(MPI_COMM_WORLD); 

 buffer = malloc(buffsize*mbytes);
 if (!buffer) { printf(" Worker %d failed to malloc %d mbytes\n", myid, mbytes); }
 indmax = buffsize*mbytes;
 MPI_Barrier(MPI_COMM_WORLD); 

 if (myid == 0) 
 {
  for (i = 0; i < indmax; i++) { buffer[i] = myid; }
  t1 = MPI_Wtime();
  MPI_Send(buffer, indmax, MPI_CHAR, 1, 1, MPI_COMM_WORLD);
  MPI_Recv(buffer, indmax, MPI_CHAR, numprocs - 1, 1, MPI_COMM_WORLD, &status);
  t2 = MPI_Wtime();
  printf("Round Trip time for %d megabytes to %d processes is %.3lf seconds\n", mbytes, numprocs, t2-t1);
  for (i = 0; i < numprocs; i++) 
   printf("Worker %d signed %d\n", i, buffer[i]);
 }
 else  
 {
  MPI_Recv(buffer, indmax, MPI_CHAR, myid - 1, 1, MPI_COMM_WORLD, &status); 
  buffer[myid] = toascii(myid);
  MPI_Send(buffer, indmax, MPI_CHAR, ((myid + 1) % numprocs), 1, MPI_COMM_WORLD);
 }

 MPI_Barrier(MPI_COMM_WORLD); 
 MPI_Finalize();
 fflush(stdout);
 return 0;
}

