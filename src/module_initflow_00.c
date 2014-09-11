#include "module_initflow_00.h"

int INIT_FLOW (Location **dataGrid, Cell ***activeList, VentArr *ventList,
               unsigned *workerct, unsigned *workersize, unsigned ventct,
               unsigned **activects, double *gridinfo, double *totalvolume,
               double modalthickness) {
/* Module INIT_FLOW
	creates initial workers' active arrays
	checks that all vents are within DEM area
	adds vent locations to first active array
	add all vent volumes to calculate total incoming volume
	
	args: data array, *active list, vent array, *worker count, worker capacity,
	      vent count, *active counts, DEM transform data, *total volume in, residual
	
	DEM transform data (gridinfo) format:
		[0] lower left x
		[1] w-e pixel resolution, column size
		[2] number of columns
		[3] lower left y
		[4] number of lines/rows
		[5] n-s pixel resolution, row size
*/
	
	unsigned vent_row, vent_col;
	Cell **IFactiveList; /*working array for activeList in Init_Flow module*/
	unsigned *IFactivects; /*working array for active counts*/
	int i;
	*totalvolume = 0;
	
	/*Do a preliminary check to make sure there are fewer vents than the worker size.
	  If there are more vents than the first worker, it's doubtful this simulation
	  can work, so we'll nip that possibility in the bud right here.*/
	
	/*While no parellelization is in effect, scale worker capacity to the model*/
	*workersize = 0;
	for(i=0;i<ventct;i++) *workersize += ventList[i].totalvolume;
	*workersize *= (unsigned) 2 / (gridinfo[1] * gridinfo[5] * modalthickness);
	
	if(ventct >= *workersize) {
		printf("\nError: [INIT_FLOW]");
		printf(" Too many vents (%u)! Max # of vents is %u\n",ventct,*workersize);
		return(-1);
	}
	
	/*ARRAY DECLARATION*/
	/*Declare an array of active lists for all defined workers
	  (initially 2) at their predefined capacities*/
	*workerct = 2;
	printf("Allocating Memory for %u Active Lists with length %u.\n",
	       *workerct,*workersize);
	*activeList = ACTIVELIST_INIT(*workerct,*workersize);
	IFactiveList = *activeList;
	
	/*Declare an array of Active Counts for each worker, initialize at 0*/
	printf("Initializing list of Filled Cell Counts for each worker.\n");
	if((*activects=
       (unsigned*)malloc((unsigned)(*workerct + 1)*sizeof(unsigned)))==NULL){
		printf("[MAIN]\n   No more memory!! Exiting\n");
		return(-1);
	}
	IFactivects = *activects;
	for(i=1;i<=*workerct;i++) IFactivects[i]=0;
	
	/*LOAD VENTS INTO FIRST ACTIVE LIST*/
	for(i=0;i<ventct;i++) {
		/*check that each vent is inside global grid*/
		if((vent_row = (unsigned) ((ventList[i].northing-gridinfo[3])/gridinfo[5])) 
		   <= 0) {
			printf("\nError: [INIT_FLOW]");
			printf(" Vent not within region covered by DEM! (SOUTH of region)\n");
			printf(" Vent #%u at cell: [%u][%u].\n",
			       (i+1),
			       vent_row,
			       (unsigned) ((ventList[i].easting-gridinfo[0])/gridinfo[1]));
			return (-1);
		}
		else if(vent_row >= gridinfo[4]) {
			printf("\nError: [INIT_FLOW]");
			printf(" Vent not within region covered by DEM! (NORTH of region)\n");
			printf(" Vent #%u at cell: [%u][%u].\n",
			       (i+1),
			       vent_row,
			       (unsigned) ((ventList[i].easting-gridinfo[0])/gridinfo[1]));
			return (-1);
		}
		else if((vent_col = (unsigned) ((ventList[i].easting-gridinfo[0])/gridinfo[1])) 
		        <= 0) {
			printf("\nERROR: [INIT_FLOW]");
			printf(" Vent not within region covered by DEM! (WEST of region)\n");
			printf(" Vent #%u at cell: [%u][%u].\n",
			       (i+1),
			       vent_row,
			       vent_col);
			return (-1);
		}
		else if(vent_col >= gridinfo[2]) {
			printf("\nERROR: [INIT_FLOW]");
			printf(" Vent not within region covered by DEM! (EAST of region)\n");
			printf(" Vent #%u at cell: [%u][%u].\n",
			       (i+1),
			       vent_row,
			       vent_col);
			return (-1);
		}
		
		/*Add vent to first active list*/
		/*Activate a vent in first Worker Active List*/
		/* args: data grid, active list, row, column, active ct, parents (0=none), 
		         vent(1=yes), residual*/
		IFactivects[1] = module_activation(dataGrid, IFactiveList[1], vent_row, 
			                vent_col, IFactivects[1], 0, 1, modalthickness);
		/*Check that activect of first worker has been updated (ct should be i+1)*/
		if(IFactivects[1]<=i){
			printf("\nError from [ACTIVATE] return value to [INIT_FLOW]!\n");
			return(-1);
		}
		/*Print vent location to standard output*/
		if (i==0) { /*Print first time a vent is loaded*/
			printf("\nVents Loaded into Lava Flow Active List:\n");
		}
		printf(" #%u [%u][%u] %15f cu. m.\n",
		       (i+1),vent_row,vent_col,ventList[i].totalvolume);
		
		/*Add current vent's volume to total volume in.*/
		*totalvolume += ventList[i].totalvolume;
	}
	
	/*Print total volume.*/
	printf("----------------------------------------\n");
	printf("Total Volume: %0.3f cu. m.\n", *totalvolume);
	
	return(0);
}
