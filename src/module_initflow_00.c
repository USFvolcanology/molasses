#include "prototypes.h"

int INIT_FLOW (Location **dataGrid, Cell ***activeList, VentArr *ventList,
               unsigned *workerct, unsigned *workersize, unsigned ventct,
               unsigned **activects, double *gridinfo, double *totalvolume) {
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
	int i,j,k;
	double min_modalthickness = 0;
	double rand_elev;
	double max_possible_cells;
	
	*totalvolume = 0;
	
	/*DATA GRID PREPARATION*/
	/* 1. Find minimum residual (to safely calculate maximum number of cells)
	   2. Calculate DEM elevations based on elevation and uncertainty*/
	k=0;
	for(i=0;i<gridinfo[4];i++) {
		for(j=0;j<gridinfo[2];j++) {
			/*if first cell, assign modal thickness as min and iterate k*/
			if ((k++)==0)	min_modalthickness = dataGrid[i][j].residual;
			/*if cell modal thickness is less than min, rewrite min.*/
			else if (min_modalthickness > dataGrid[i][j].residual)
				min_modalthickness = dataGrid[i][j].residual;
			
			/*RANDOMIZE ELEVATIONS IF NEEDED*/
			if(!dataGrid[i][j].elev_uncert) /*if no uncertainty*/
				dataGrid[i][j].elev = dataGrid[i][j].dem_elev; /*copy DEM elev to flow elev*/
			else { /*if uncertainty, assign random elevation*/
				/*This produces a pseudorandom number w/ box-muller method
				  for a pdf with mean 0, STD 1.0 (M_PI comes from the math library)*/
				rand_elev = sqrt(-2 * log((rand()+1.0)/(RAND_MAX+1.0)))*
				              cos(2*M_PI*((rand()+1.0)/(RAND_MAX+1.0)));
				/*scale random number by uncertainty (scale the STD)*/
				rand_elev *= dataGrid[i][j].elev_uncert;
				/*apply to the DEM value (translate the mean)*/
				rand_elev += dataGrid[i][j].dem_elev;
				dataGrid[i][j].elev = rand_elev;
			}
		}
	}
	if (min_modalthickness<=0) {
		printf("\nError: [INIT_FLOW]");
		printf(" Minimum Modal thickness (Residual Thickness) is <= 0!\n");
		return(-1);
	}
	
	/*ACTIVE LIST PREPARATION*/
	/*While no parellelization is in effect, scale worker capacity to the model*/
	/*The MOST cells a flow can inundate is 3*(total volume/residual volume)+2*/
	max_possible_cells = 0.0;
	for(i=0;i<ventct;i++) max_possible_cells += ventList[i].totalvolume;
	max_possible_cells *= (3/(gridinfo[1] * gridinfo[5] * min_modalthickness));
	max_possible_cells += 2;
	*workersize = (unsigned) max_possible_cells;
	
	/*Do a preliminary check to make sure there are fewer vents than the worker size.
	  If there are more vents than the first worker, it's doubtful this simulation
	  can work, so we'll nip that possibility in the bud right here.*/
	if(ventct >= *workersize) {
		printf("\nError: [INIT_FLOW]");
		printf(" Too many vents (%u)! Max # of vents is %u\n",ventct,*workersize);
		return(-1);
	}
	
	/*ARRAY DECLARATION*/
	/*Declare an array of active lists for all defined workers
	  (initially 2) at their predefined capacities*/
	*workerct = 2;
	printf("Allocating Memory for Active Cell Lists... ");
	*activeList = ACTIVELIST_INIT(*workerct,*workersize);
	IFactiveList = *activeList;
	
	/*Declare an array of Active Counts for each worker, initialize at 0*/
	printf("and counters...");
	if((*activects=
       (unsigned*)malloc((unsigned)(*workerct + 1)*sizeof(unsigned)))==NULL){
		printf("\nError: [INIT_FLOW]");
		printf(" No more memory! Tried to create Active Count List\n");
		return(-1);
	}
	printf(" Done.\n");
	IFactivects = *activects;
	
	/*Declare Active List Counts to 0 (No Active Cells yet)*/
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
		IFactivects[1] = ACTIVATE(dataGrid, IFactiveList[1], vent_row, 
			                vent_col, IFactivects[1], 0, 1);
		/*Check that activect of first worker has been updated (ct should be i+1)*/
		if(IFactivects[1]<=i){
			printf("\nError from [ACTIVATE] return value to [INIT_FLOW]!\n");
			return(-1);
		}
		/*Print vent location to standard output*/
		if (i==0) { /*Print first time a vent is loaded*/
			printf("\nVents Loaded into Lava Flow Active List:\n");
		}
		printf(" #%u [%u][%u] %15.3f cu. m.\n",
		       (i+1),vent_row,vent_col,ventList[i].totalvolume);
		
		/*Add current vent's volume to total volume in.*/
		*totalvolume += ventList[i].totalvolume;
	}
	
	/*Print total volume.*/
	printf("----------------------------------------\n");
	printf("Total Volume: %15.3f cu. m.\n", *totalvolume);
	
	return(0);
}
