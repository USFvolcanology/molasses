#include "module_output_00.h"

int OUTPUT(Location **dataGrid, Cell **actList,unsigned workercount,
           unsigned *flowcount, char *flowfilename,double *geotransform,
           double residual) {
/*Module OUTPUT
	Prints grid cells locations inundated by lava and thicknesses to a tab-del
	file. For flexibility, more parameters are needed as arguments for this module
	in case of future needs to output additional information
	
	geotransform list description (gdal):
		geotransform[0], dem lower left easting
		geotransform[1], dem column width
		geotransform[2], # rows in DEM (max row + 1)
		geotransform[3], dem lower left northing
		geotransform[4], # columns in DEM (max column + 1)
		geotransform[5], dem row height
*/
	FILE     *flowfile;
	unsigned i,j;
	
	printf("\nFlow Output:\n");
	/*Open flow file (x,y,thickness)*/
	flowfile  = fopen(flowfilename, "w");
	if (flowfile == NULL) {
		printf("Cannot open FLOW file=[%s]:[%s]! Exiting.\n",
		flowfilename, strerror(errno));
		return(-1);
	} else printf(" opened Flow file: %s.\n",flowfilename);
	
	for(i=1;i<=workercount;i++)  { /*for each worker list*/
		for(j=1;j<=flowcount[i];j++) { /*for each cell in the current worker list*/
			/*Retrieve Active Cell values and print to flowfile*/
			/*x,y,z: easting, northing, thickness*/
			fprintf(flowfile, "%0.3f\t%0.3f\t%0.6f\n", 
			       (float) geotransform[0] + (geotransform[1] * actList[i][j].col),
			       (float) geotransform[3] + (geotransform[5] * actList[i][j].row),
			       (float) (actList[i][j].thickness + residual));
		}
	}
	printf(" Flow (x,y,thickness) data written to tab-delimited file.\n");
	fclose(flowfile);
	printf(" closed Flow file: %s successfully.\n",flowfilename);
	
	return(0);
}
