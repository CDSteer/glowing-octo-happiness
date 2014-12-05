/**
Please refer to the 705784_663524.pdf in the MPIEX_A1  
folder for answers, comments and test data regarding
the temp_dist2D.c file
*/

/****************************************************************************
* FILE: temp_dist2D.c
* DESCRIPTION:  
*   simplified two-dimensional temperature equation domain decomposition.  
* MODIFIED BY: Rita Borgo, Cameron Steer, Henrik Thaudal
* Last Revised: 17/11/14 Cameron Steer, Henrik Thaudal
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define XDIM 100
#define YDIM 100

struct Params
{
	double cx; /** sampling size along X */
	double cy; /** sampling size along Y */
	int nts; /** timesteps */
}params;

void initdata(double *u1, char* fname);
void prtdata(int nx, int ny, int ts, double *u1, char* fname);
void update(int nx, int ny, double *u1, double *u2);
void boundarycopy(int nx, int ny, double *u1, double *u2);

int main(int argc, char *argv[])
{
	double old_u[XDIM][YDIM];
	double new_u[XDIM][YDIM];
	/* iterators */
	int ix, iy, iz, ts;
	/* input filename */
	char*	fname;
	/* output filename */
	char newfname[30];

	clock_t c1, c2;
	float total;

	printf("Starting serial version of 2D temperature distribution example...\n");
	/** Read in grid dimension dim and allocate old_u and new_u */
	printf("Using [%d][%d] grid.\n", XDIM, YDIM);

	/** Read in grid sampling parameters and timesteps and initialize struct Params */
	params.cx = atof(argv[1]);
	params.cy = atof(argv[2]);
	params.nts = atoi(argv[3]);
	fname = argv[4];

	/** Check values are correct */
	printf("%1.1f \n", params.cx);
	printf("%1.1f \n", params.cy);
	printf("%i \n", params.nts);

	/** Initialize grid from input file */
	printf("Initializing grid from input file:\n");
	initdata(*old_u, fname);
	boundarycopy(XDIM, YDIM, &old_u[0][0], &new_u[0][0]);

	c1 = clock();
	
	for (ts=1 ; ts <= params.nts ; ts++)
	{	
		sprintf(newfname, "final_data_%d.csv", ts);
		if (ts%2 != 0)
		{		
			update(XDIM, YDIM, &old_u[0][0], &new_u[0][0]);
			prtdata(XDIM, YDIM, ts, &new_u[0][0], newfname);			
		}	
		else
		{
			update(XDIM, YDIM, &new_u[0][0], &old_u[0][0]);
			prtdata(XDIM, YDIM, ts, &old_u[0][0], newfname);
		}
	}

	/** prtdata(XDIM, YDIM, ts, &new_u[0][0], newfname); */

	c2 = clock();

	/** calculate time taken based on clock c1 and c2 */
	total = (c2-c1)*1000.f/CLOCKS_PER_SEC;
	printf("Doing %d steps took %6.2f milliseconds\n", params.nts,total);
}


/***
*  initdata: initializes old_u with data read from file specified at command line
***/
void initdata(double *u1, char* fname)
{
	int ix, iy;
	FILE* fp;

	char buffer[4096];
	char *record = 0, *line = 0;
	fp = fopen(fname, "r");

	/** If fp returns NULL, i.e. file not found, prints an error */
	if (!fp)
	{
		printf("File %s could not be found", fname);
	}
	/** If fp is found */
	else
	{
		printf("File %s open\n", fname);
		/** Outer loop cycles through the .csv file line by line */
		for (iy = 0; iy < YDIM; iy++)
		{
			/** fgets reads in the line, and stores it in line */
			line = fgets(buffer, sizeof(buffer), fp);
			if (buffer)
			{
				/** strtok takes the string before delimiter ',' and stores it in record */
				record = strtok(line, ", ");
				for (ix = 0; ix < XDIM; ix++)
				{
					if (record)
					{
						/** converts the record string to a float and stores in u1 */
						*(u1 + (XDIM * iy + ix)) = atof(record);
						record = strtok(0, ", ");
					}
				}
			}
			/** else if the file no more lines break out of loop */
			else
			{
				int err;
				/** throw error if issue with file */
				if ((err = ferror(fp))) printf("error: %d\n", err);
				break;
			}
		}
	}

	fclose(fp);
	printf("\nFile closed\n");
}

/***
*  printdata: generates a .csv file with data contained in parameter double* u1 
***/
void prtdata(int nx, int ny, int ts, double *u1, char* fname)
{
	int ix, iy;
	FILE *fp;
	fp = fopen(fname, "w");

	for (iy = 0; iy < ny; iy++)
	{
		for (ix = 0; ix < nx; ix++){
			fprintf(fp, "%8.3f, ", *(u1 + iy*ny + ix));
			if (ix != nx - 1){
				fprintf(fp, " ");
			}
			else {
				fprintf(fp, "\n");
			}
		}
	}

	fclose(fp);
	printf("%s correspond to data generated at time step %d.\n", fname, ts);
}

/***
*  update: computes new values for timestep t+delta_t
***/
void update(int nx, int ny, double *u1, double *u2)
{
    int ix, iy;
    
    for (iy = 1; iy < XDIM-1; iy++)
    {
        for (ix = 1; ix < YDIM-1; ix++)
        {	
        	/** implemtation of equation */
        	*(u2+iy*ny+ix) = *(u1+iy*YDIM+ix)+params.cx*(*(u1+iy*YDIM+(ix+1))+(*(u1+iy*YDIM+(ix-1)))-2*(*(u1+iy*YDIM+(ix))))+params.cy*(*(u1+(iy+1)*YDIM+(ix))+(*(u1+(iy-1)*YDIM+(ix)))-2*(*(u1+iy*YDIM+(ix))));        	
        }
    }
}

/***
*  boundarycopy: copies bondaries of one array to the other
***/
void boundarycopy(int nx, int ny, double *u1, double *u2)
{
    int ix, iy;

    /** First loop copying the top and bottom rows of u1 to u2 */
    for (ix = 0; ix < nx; ix++)
    {
        *(u2+0*ny+ix) = *(u1+0*ny+ix);
        *(u2+(ny-1)*ny+ix) = *(u1+(ny-1)*ny+ix);  
    }

    /* Second loop copying the values of the first columns from u1 to u2, omitting the corners, 
       since they've already been copied by the above loop
     */
    for ( iy = 1; iy < ny-1; iy++)
    {
        *(u2+iy*ny+0) = *(u1+iy*ny+0);
        *(u2+iy*ny+(nx-1)) = *(u1+iy*ny+(nx-1));
    }
}
