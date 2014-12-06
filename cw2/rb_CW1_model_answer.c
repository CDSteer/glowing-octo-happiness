/****************************************************************************
* FILE: serial_temp2D.c
* DESCRIPTION:  
*   simplified two-dimensional temperature equation domain decomposition.  
* MODIFIED BY: Rita Borgo
* Last Revised: 25/10/14 Rita Borgo
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#define XDIM 10
#define YDIM 10
#define MASTER 0

struct Params
{ 
	double cx; /* sampling size along X */
	double cy; /* sampling size along Y */
	int nts; /* timesteps */
}params = {0.1,0.1,1}; 


void initdata(int nx, int ny, double *u1);
void update(int nx, int ny, double *u1, double *u2);
void prtdata(int nx, int ny, int ts, double *u1, char* fname);


int main(int argc, char *argv[])
{
	int numtasks, taskid, num_rows_per_process, chunksize, chunksize_returned, offset, source, tag1, tag2, dest, i, j;
	int nodeoffset;
	int my_id, root_process, ierr, num_rows, num_procs,
         num_rows_to_receive, avg_rows_per_process, 
         sender, num_rows_received, start_row, end_row, num_rows_to_send, num_rows_to_return, partial_sum;

	MPI_Status status;
	
	MPI_Init(&argc, &argv);
	double old_u[XDIM][YDIM];
	double old_u2[XDIM][YDIM];
	double new_u[XDIM][YDIM];
	int ix, iy, iz, it, ts; /* iterators */
	char buf[256], *str_end=".dat";
	
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	printf("number of tasks: %d\n", &numtasks);
	printf ("MPI task %d has started...\n", taskid);
	chunksize = (XDIM / numtasks);
	tag2 = 1;
	tag1 = 2;
	if (taskid == MASTER){
		printf("Starting serial version of 2D temperature distribution...\n");
	
		/* Read in grid dimension dim and allocate old_u and new_u */
		printf("Using [%d][%d] grid.\n", XDIM, YDIM);

		/* Read in grid sampling parameters and timesteps and initialize struct Params */


		/* Initialize grid from input file */
		printf("Initializing grid from input file:\n");

		// avg_rows_per_process = XDIM / numtasks-1;

		initdata(XDIM, YDIM, &old_u[0][0]);
		
		offset = 0;
		for (dest=1; dest<numtasks; dest++) {
			for ( ix = offset; ix < offset+chunksize; ix++ ){
				ierr = MPI_Send(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
      			ierr = MPI_Send(&old_u, chunksize*YDIM, MPI_DOUBLE, dest, tag2, MPI_COMM_WORLD);	
      		}
      		offset = offset + chunksize;
      		

			printf("Sent %d rows to node %d offset= %d\n", chunksize, dest, offset);
			
		}
		
		offset = YDIM-(YDIM%(numtasks-1));
		for(i = offset; i < chunksize; i++) {
        	for(j = 0; j < YDIM-2; j++) {
            	//partial_sum += old_u2[i][j];
            	printf("element master: %1.1f\n", old_u[i][j]);
        	}
        }

		for (dest=1; dest<numtasks; dest++) {
			source = i;
			MPI_Recv(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD, &status);
			MPI_Recv(&old_u, chunksize*YDIM, MPI_DOUBLE, dest, tag2, MPI_COMM_WORLD, &status);
			nodeoffset = offset;
			num_rows_received = chunksize;  
			for(i = nodeoffset; i < num_rows_received; i++) {
	        	for(j = 0; j < YDIM; j++) {
	            	//partial_sum += old_u2[i][j];
	            	printf("element from %d: %1.1f\n", i, old_u[i][j]);
	        	}
        	}
		}
				

		sprintf(buf,"initial_new_%d%s",0,str_end);
		prtdata(XDIM, YDIM, 0, &old_u[0][0], buf);

		for (ix = 0; ix <= XDIM-1; ix++) {
			new_u[ix][0] = old_u[ix][0];
			new_u[ix][YDIM-1] = old_u[ix][YDIM-1];
		}
		for (iy = 0; iy <= YDIM-1; iy++) {
			new_u[0][iy] = old_u[0][iy];
			new_u[XDIM-1][iy] = old_u[XDIM-1][iy];
		}

		/* Iterate over all timesteps and create final output file (e.g. timestep = nts) */
		for (it = 1; it <= params.nts; it++) {
			if(it&1)
	       		/* it is odd */
				update(XDIM, YDIM, &old_u[0][0], &new_u[0][0]);
			else 
			/* it is even */
				update(XDIM, YDIM, &new_u[0][0], &old_u[0][0]);

			sprintf(buf,"final_new_%d%s",it,str_end);
			printf("Done. Created output file: %d\n", it);
			prtdata(XDIM, YDIM, it, &old_u[0][0], buf);
		}
		
	}  
	if (taskid > MASTER){

		printf("Process %d said: Hello!\n", taskid);
		//printf("%d\n",&old_u[0][0]);
		/* Receive my portion of array from the master task */
		// source = MASTER;
		//MPI_Recv(&offset, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
		// temp_u2 = MPI_Recv(temp_u2, chunksize, MPI_DOUBLE, source, tag2, MPI_COMM_WORLD, &status);
	 	
		// update(offset, YDIM, &new_u[0][0], *temp_u);
		
		/* Send my results back to the master task */
		// dest = MASTER;
		//MPI_Send(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
		// MPI_Send(temp_u, chunksize, MPI_DOUBLE, MASTER, tag2, MPI_COMM_WORLD);

		// 

		ierr = MPI_Recv( &offset, 1 , MPI_INT, MASTER, tag1, MPI_COMM_WORLD, &status);

   		ierr = MPI_Recv( &old_u, chunksize*YDIM, MPI_DOUBLE, MASTER, tag2, MPI_COMM_WORLD, &status);
   		
   		num_rows_received = chunksize;  
   		nodeoffset = offset;
   		/* Do something with array2 here, placing the result in array3,
    	* and send array3 to the root process. */
   		// 
   		// partial_sum = 0;
        
        // printf("Process %d sum: %d\n", taskid, partial_sum);
        //update(num_rows_received, YDIM, &new_u[0][0], *old_u);

   		MPI_Send(&offset, 1, MPI_INT, MASTER, tag1, MPI_COMM_WORLD);
   		MPI_Send(&old_u, chunksize*YDIM, MPI_DOUBLE, MASTER, tag2, MPI_COMM_WORLD);

   		// MPI_Reduce(&mysum, &sum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}

/***
*  update: computes new values for timestep t+delta_t
***/
void update(int nx, int ny, double *u1, double *u2)
{
	int ix, iy;

	for (ix = 1; ix <= nx-2; ix++) {
		for (iy = 1; iy <= ny-2; iy++) {
			*(u2+ix*ny+iy) = *(u1+ix*ny+iy)  + 
				params.cx * (*(u1+(ix+1)*ny+iy) + *(u1+(ix-1)*ny+iy) - 
				2.0 * *(u1+ix*ny+iy)) +
				params.cy * (*(u1+ix*ny+iy+1) + *(u1+ix*ny+iy-1) - 
				2.0 * *(u1+ix*ny+iy));
		}
	}

}

/***
*  initdata: initializes old_u, timestep t=0
***/
void initdata(int nx, int ny, double *u1)
{
	int ix, iy;
	for (ix = 0; ix <= nx-1; ix++) 
		for (iy = 0; iy <= ny-1; iy++) 
			*(u1+ix*ny+iy) = (float)(ix * (nx - ix - 1) * iy * (ny - iy - 1));

}
/***
*  printdata: generates a .csv file with data contained in parameter double* u1 
***/

void prtdata(int nx, int ny, int ts, double *u1, char* fnam)
{
	int ix, iy;
	FILE *fp;

	fp = fopen(fnam, "w");
	for (iy = ny-1; iy >= 0; iy--) {
		for (ix = 0; ix <= nx-1; ix++) {
			fprintf(fp, "%8.3f,", *(u1+ix*ny+iy));
			if (ix != nx-1) {
				fprintf(fp, " ");
			}
			else {
				fprintf(fp, "\n");
			}
		}
	}
	fclose(fp);
	printf(" %s\n",fnam);
}

