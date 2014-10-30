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
#define XDIM 100
#define YDIM 100

struct Params
{ 
	double cx; /* sampling size along X */
	double cy; /* sampling size along Y */
	int nts; /* timesteps */
}params; 


void initdata(double *u1, char* fname);
void update(int nx, int ny, double *u1, double *u2);
void prtdata(int nx, int ny, int ts, double *u1, char* fname);


int main(int argc, char *argv[])
{
	double old_u[XDIM][YDIM];
	double new_u[XDIM][YDIM];
	int ix, iy, iz, it, ts; /* iterators */
	char*	fname; /* input filename */

	printf("Starting serial version of 2D temperature distribution example...\n");

	/* Read in grid dimension dim and allocate old_u and new_u */

	printf("Using [%d][%d] grid.\n", XDIM, YDIM);

	/* Read in grid sampling parameters and timesteps and initialize struct Params */


	/* Initialize grid from input file */
	printf("Initializing grid from input file:");

	initdata(old_u, fname);


	/* copy boundary conditions from old_u to new_u */
	for (ix = 0; ix <= XDIM-1; ix++) 
	{
		/* TO BE COMPLETED */
	}
	for (iy = 0; iy <= YDIM-1; iy++) 
	{
		/* TO BE COMPLETED */
	}

	/* Iterate over all timesteps and create final output file (e.g. timestep = nts) */
	printf("Iterating over %d time steps...\n", nts);
	{
		/* if timestep is even */
			update(dim, dim, &old_u[0][0], &new_u[0][0]);
		/* if timestep is odd */
			update(dim, dim, &new_u[0][0], &old_u[0][0]);
	}
	printf("Done. Created output file: %d\n", it);
	prtdata(XDIM, YDIM, it, &old_u[0][0], "final_data.csv");

}


/***
*  update: computes new values for timestep t+delta_t
***/
void update(int nx, int ny, double *u1, double *u2)
{
	int ix, iy;

		/* TO BE COMPLETED */

}

/***
*  initdata: initializes old_u with data read from file specified at command line
***/
void initdata(double *u1, char* fname)
{
	int ix, iy;
	FILE* fp;

	/* TO BE COMPLETED */

}
/***
*  printdata: generates a .csv file with data contained in parameter double* u1 
***/

void prtdata(int nx, int ny, int ts, double *u1, char* fname)
{
	int ix, iy;
	FILE *fp;

	fp = fopen(fname, "w");
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
	printf(" %s\n",fname);
}
