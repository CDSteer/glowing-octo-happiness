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
}params = {0.1,0.1,1}; 


void initdata(int nx, int ny, double *u1);
void update(int nx, int ny, double *u1, double *u2);
void prtdata(int nx, int ny, int ts, double *u1, char* fname);


int main(int argc, char *argv[])
{
	double old_u[XDIM][YDIM];
	double new_u[XDIM][YDIM];
	int ix, iy, iz, it, ts; /* iterators */
	char buf[256], *str_end=".dat";

	printf("Starting serial version of 2D temperature distribution...\n");

	/* Read in grid dimension dim and allocate old_u and new_u */

	printf("Using [%d][%d] grid.\n", XDIM, YDIM);

	/* Read in grid sampling parameters and timesteps and initialize struct Params */


	/* Initialize grid from input file */
	printf("Initializing grid from input file:");

	initdata(XDIM, YDIM, *old_u);

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
		if(it&1) /* it is odd */
			update(XDIM, YDIM, &old_u[0][0], &new_u[0][0]);
		else /* it is even */
			update(XDIM, YDIM, &new_u[0][0], &old_u[0][0]);

		sprintf(buf,"final_new_%d%s",it,str_end);
		printf("Done. Created output file: %d\n", it);
		prtdata(XDIM, YDIM, it, &old_u[0][0], buf);
	}
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

