#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define XDIM 100
#define YDIM 100
#define NFILESIZE 15

struct Params
{
	double cx; /* sampling size along X */
	double cy; /* sampling size along Y */
	int nts; /* timesteps */
}params;

void initdata(double *u1, char* fname);
void prtdata(int nx, int ny, int ts, double *u1, char* fname);
void update(int nx, int ny, double *u1, double *u2);
void boundaryCopy(int nx, int ny, double *u1, double *u2);

int main(int argc, char *argv[])
{
	double old_u[XDIM][YDIM];
	double new_u[XDIM][YDIM];
	int ix, iy, iz, /*it = 0*/ ts; /* iterators */
	char*	fname; /* input filename */
	

	printf("Starting serial version of 2D temperature distribution example...\n");

	/* Read in grid dimension dim and allocate old_u and new_u */

	printf("Using [%d][%d] grid.\n", XDIM, YDIM);

	/* Read in grid sampling parameters and timesteps and initialize struct Params */
	params.cx = atof(argv[1]);
	params.cy = atof(argv[2]);
	params.nts = atoi(argv[3]);
	fname = argv[4];
	/* Check values are correct */
	printf("%1.1f \n", params.cx);
	printf("%1.1f \n", params.cy);
	printf("%i \n", params.nts);


   	
   	//printf("%s\n", newfname);

	/* Initialize grid from input file */
	printf("Initializing grid from input file:\n");

	initdata(*old_u, fname);

	printf("Create data from old_u\n");

	for ( ts=1 ; ts <= params.nts ; ts++ )
	{	
		char newfname[sizeof(&ts)+NFILESIZE];
		sprintf(newfname, "final_data_%d.csv", ts);
		//printf("Doing step: %d\n ",ts);
		if ( ts%2 != 0)
		{

			boundaryCopy(XDIM, YDIM, &old_u[0][0], &new_u[0][0]);			
			update(XDIM, YDIM, &old_u[0][0], &new_u[0][0]);
			prtdata(XDIM, YDIM, ts, &new_u[0][0], newfname);			
		}	
		else
		{
			boundaryCopy(XDIM, YDIM, &new_u[0][0], &old_u[0][0]);
			update(XDIM, YDIM, &new_u[0][0], &old_u[0][0]);
			prtdata(XDIM, YDIM, ts, &old_u[0][0], newfname);
		}
	}



	//boundaryCopy(XDIM, YDIM, &old_u[0][0], &new_u[0][0]);
	//update(XDIM, YDIM, &old_u[0][0],&new_u[0][0]);

	//prtdata(XDIM, YDIM, it, &old_u[0][0], "final_data.csv");

	
	//prtdata(XDIM, YDIM, it, &new_u[0][0], newfname);

}

void initdata(double *u1, char* fname)
{
	int ix, iy;
	FILE* fp;
	char buffer[4096];
	char *record = 0, *line = 0;
	fp = fopen(fname, "r");

	if (!fp)
	{
		printf("File %s could not be found", fname);
	}
	else
	{
		printf("File %s open\n", fname);
		for (iy = 0; iy < YDIM; iy++)
		{
			line = fgets(buffer, sizeof(buffer), fp);
			if (buffer)
			{
				record = strtok(line, ", ");
				for (ix = 0; ix < XDIM; ix++)
				{
					if (record)
					{
						// u1 = array
						*(u1 + (XDIM * iy + ix)) = atof(record);
						//printf("(%d,%d) %0.3f, ", iy, ix, *(u1 + (XDIM * iy + ix)));
						// printf("%f\n", *((u1+YDIM*ix)+iy));
						record = strtok(0, ", ");
					}
				}
				//printf("\n");
			}
			else
			{
				int err;
				if ((err = ferror(fp))) printf("error: %d\n", err);
				//printf("hello\n");
				break;
			}
		}


		// adding random values for testing
		// *((u1+YDIM*ix)+iy) = (double)1.1*ix*iy;
	}

	fclose(fp);
	printf("\nFile closed\n");
}

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
	//printf(" %s\n", fname);
}


//for (iy = ny-1; iy >= 0; iy--) {
//for (ix = 0; ix <= nx-1; ix++) {
//printf("x:%d, y:%d = %8.3f,", ix, iy, *(u1+ix*ny+iy));

//printf("x:%d, y:%d\n", ix, iy);
//printf("x:%d, y:%d = %8.3f\n", ix, iy, *(u1+YDIM*ix+iy));

void update(int nx, int ny, double *u1, double *u2)
{
    int ix, iy;

        for ( iy = 1; iy < XDIM-1; iy++)
        {
            for ( ix = 1; ix < YDIM-1; ix++ )
            {
                //*(u2+iy*ny+ix) = *(u1+iy*YDIM+ix)+params.cx*(*(u1+iy*YDIM+(ix+1))+(*(u1+iy*YDIM+(ix-1)))-2*(*(u1+iy*YDIM+(ix))))+params.cy*(*(u1+(iy+1)*YDIM+(ix))+(*(u1+(iy-1)*YDIM+(ix)))-2*(*(u1+iy*YDIM+(ix))));
                *(u2+iy*ny+ix) = *(u1+iy*YDIM+ix)+params.cx*(-2*(*(u1+iy*YDIM+ix))+(*(u1+iy*YDIM+(ix+1)))+(*(u1+iy*YDIM+(ix-1))))+params.cy*(-2*(*(u1+iy*YDIM+ix))+(*(u1+(iy+1)*YDIM+(ix)))+(*(u1+(iy-1)*YDIM+(ix))));
            }
        }

}

void boundaryCopy(int nx, int ny, double *u1, double *u2)
{
    int ix, iy;
    
    for ( ix = 0; ix < nx; ix++ )
    {
        *(u2+0*ny+ix) = *(u1+0*ny+ix);
        *(u2+(ny-1)*ny+ix) = *(u1+(ny-1)*ny+ix);  
    }

    for ( iy = 1; iy < ny-1; iy++)
    {
        *(u2+iy*ny+0) = *(u1+iy*ny+0);
        *(u2+iy*ny+(nx-1)) = *(u1+iy*ny+(nx-1));
    }
}