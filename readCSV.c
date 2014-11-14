#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main()
{
    char buffer[4096] ;
    char *record,*line;
	double item;
    int i=0,j=0;
    // int count=0;
    FILE *fstream = fopen("initial_data.csv","r");
    	
	if(fstream == NULL)
    {
        printf("\n file opening failed ");
        return -1 ;
    }
    
   	while((line=fgets(buffer,sizeof(buffer),fstream))!= NULL)
    {
        record = strtok(line,",");
		while(record = strtok(NULL,",") )
        {
			printf("item: %8.3f \n",atof(record));
			// count++;

		}
    }
   	// printf("Entries counted, %d",count);
    return 0 ;
}


