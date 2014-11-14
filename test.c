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
	params.cx = atof(argv[1]);
	params.cy = atof(argv[2]);
	params.nts = atoi(argv[3]);
	fname = argv[4];
	/* Check values are correct */
	printf("%1.1f \n",params.cx);
	printf("%1.1f \n",params.cy);
	printf("%i \n",params.nts);

	/* Initialize grid from input file */
	printf("Initializing grid from input file:\n");

	initdata(*old_u, fname);
	
	/* copy boundary conditions from old_u to new_u */
        for (ix = 0; ix <= XDIM-1; ix++)
        {
         	for (iy = 0; iy <= YDIM-1; iy++)
        	{
			if ((ix == 0)||(ix==XDIM-1)||(iy == 0)||(iy == YDIM-1))
			{
				new_u[ix][iy]=0;
			}
			else
			{
				new_u[ix][iy]=old_u[ix][iy]+params.cx*(old_u[ix+1][iy]+old_u[ix-1][iy]-2*(old_u[ix][iy]))+params.cy*(old_u[ix][iy+1]+old_u[ix][iy-1]-2*(old_u[ix][iy]));
			}

        	}
       
        }

	printf("After initdata \n");
	for ( ix = 0; ix < XDIM; ix++ )
	{
		for ( iy = 0; iy < YDIM; iy++ )
		{
			printf("%8.3f \n", old_u[ix][iy]);
		}
		printf("\n");
	}

	printf("Create data from old_u");
	prtdata(XDIM, YDIM, it, &old_u[0][0], "final_data_.csv");
}

void initdata(double *u1, char* fname)
{
	int ix, iy;
	FILE* fp;
    
    char buffer[4096] ;
    char *record,*line;
    double item;

	fp = fopen(fname,"r");

	if ( !fp )
	{
		printf("File %s could not be found", fname);
	} 
	else
	{
        printf("File %s open\n", fname);
        for ( ix = 0; ix < XDIM ; ix++ )
        {
            for ( iy = 0; iy < YDIM; iy++ )
            {
                if ((line = fgets(buffer, sizeof(buffer), fp)) != NULL)
                {
                    // adding random values for testing
                    // *((u1+YDIM*ix)+iy) = (double)1.1*ix*iy;
                    record = strtok(line,",");
                    record = strtok(NULL,",");
                    *((u1+YDIM*ix)+iy) = atof(record);
                }
            }
		
        }
        
	}


	fclose(fp);
	
	printf("File closed\n");


}

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

