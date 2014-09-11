#include "driver_00.h"

int main(int argc, char *argv[]) {
/*DRIVER for a lava flow code
  Set up sample data grid (DEM, AOI)
  Define a Worker Capacity
  Create an active list, 2D, with one array for each worker:
  	2 initially, with 1 being the lowest worker
  Define a sample vent as first entry in Worker[1] list
  Create a loop to:
  	pulse lava to vent
  	run distribute
  When Distribute returns the full capacity of active cells
  	Check Active List "worker[2]"
  	if not empty anymore, reallocate Worker List to make empty 3rd worker

  	Output Lists to a file for mapping
  */
	
	/*Files*/
	char     *CFGfilename = argv[1]; /*configuration file name*/
	char     *DEMfilename;           /*raster file name*/
	char     *outputfilename;
	/*FILE     *demfile;*/
	double   *DEMmetadata;
	
	/*Arrays*/
	Location **dataGrid;
	Cell **WorkingCells;
	Cell *ExteriorCells; /*Return from distribution (Null if Worker not full)*/
	unsigned *ActiveCounts; /*list of active counts for each worker*/
	
	/*Model Parameters*/
	int      i,j;
	unsigned workerCount = 0; /*declare lists for first and second worker*/
	unsigned workerCapacity = 100000; /*declare length of worker lists*/
	unsigned ventCount = 0;
	int      ret; /*return integer*/
	
	/*Physical Parameters*/
	VentArr   *Vents;
	double    residual;
	double    volume_remaining;
	double    tot_vol_in;
	double    tot_vol_out = 0; /*bookkeeper to check for total volume in cells*/
	
	/*Assign Module Pointers*/
	module_initialize  = &INITIALIZE;
	module_dem_loader  = &DEM_LOADER;
	module_init_flow   = &INIT_FLOW;
	module_distribute  = &DISTRIBUTE;
	module_activation  = &ACTIVATE;
	module_neighbor_id = &NEIGHBOR_ID;
	module_pulse       = &PULSE;
	module_output      = &OUTPUT;
	
	starttime = time(NULL);
	printf("\n\n                  Driver for a lava flow code.\n\n");
	if(argc<2) {
		printf("Usage: %s config-filename\n",argv[0]);
		return(-1);
	}
	
	printf("Beginning flow simulation...\n");
	
	ret = module_initialize(CFGfilename, &DEMfilename, &outputfilename,
	                        &residual, &Vents, &ventCount);
	/*char *CFGfilename, char **DEMfilename, char **OUTfilename, 
    double *param_modalthickness, VentArr *Vents, unsigned *ventct*/
	if(ret<0){
		printf("\nError from [INITIALIZE]. Exiting.\n");
		return(-1);
	}
	
	/*Load DEM into Global Data Grid*/
	DEMmetadata = module_dem_loader(DEMfilename,&dataGrid);
	/*DEMmetadata format:
		[0] lower left x
		[1] w-e pixel resolution
		[2] number of cols, assigned manually
		[3] lower left y
		[4] number of lines, assigned manually
		[5] n-s pixel resolution (negative value)
	*/
	if(DEMmetadata==NULL){
		printf("\nError [MAIN] from DEM_LOADER. Exiting.\n");
		return(-1);
	}
	
	/*Assign DEM metadata to user-readable variables, potentially this can be
	 phased out.*/
	dem_row_max  = (unsigned) DEMmetadata[4]-1; /*maximum dimension of DEM (#rows-1)*/
	dem_col_max  = (unsigned) DEMmetadata[2]-1;
	dem_row_size = DEMmetadata[5];   /*height/width of row,columns*/
	dem_col_size = DEMmetadata[1];
	dem_ll_y     = DEMmetadata[3];   /*lower left coordinates*/
	dem_ll_x     = DEMmetadata[0];
	
	/*Initialize Flow by activating vents and creating active lists*/
	ret = module_init_flow(dataGrid, &WorkingCells, Vents, &workerCount, 
	                       &workerCapacity, ventCount, &ActiveCounts, DEMmetadata, 
	                       &tot_vol_in, residual);
	/*args: data array, *active list, vent array, *worker count, worker capacity,
	  vent count, *active counts, DEM transform data, *total volume in, residual*/
	
	if(ret<0) {
		printf("\nError [MAIN] from [INIT_FLOW]. Exiting.\n");
		return(-1);
	}
	
	volume_remaining = tot_vol_in;
	
	printf("\n                         Running Flow\n");
	
	/*LOOP to Pulse and Distribute Lava*/
	while(volume_remaining > 0) {	
	
		/*PULSE MODULE*/
		ret = module_pulse(WorkingCells[1],&Vents,ActiveCounts[1],
		                   &volume_remaining,ventCount,DEMmetadata);
		
		printf("\rInundated Cells: %-7d; Volume Remaining: %10.2f",ActiveCounts[1],
		       volume_remaining);

		if (ret<0) {
		/*This return indicates an error in Pulse module*/
			printf("\n  Pulse Module returned an error code to [MAIN]!  Exiting\n");
			return(-1);
		}
		else if (ret==0) {
			if (volume_remaining) {
			/*This return should not be possible*/
				printf("\n  Pulse Module had an error [MAIN]!  Exiting\n");
				return(-1);
			}
			/*If ret==0, do not call Pulse or Distribute anymore!
			  Break out of While loop.*/
			break;
		}
		/*if Pulse module successfully updated vents, ret will > 0.
		  Continue, call Distribute module.*/
		
		/*DISTRIBUTE MODULE*/
		ExteriorCells = module_distribute(dataGrid,WorkingCells[1],
		                &ActiveCounts[1],workerCapacity,workerCount,residual);
		
		if(ExteriorCells==NULL) {
			/*
			printf("Exited [DISTRIBUTE]...");
			printf("Not at capacity (%d/%d)\n\n", ActiveCounts[1], workerCapacity);
			*/
		}
		else if (ExteriorCells[0].row == UINT_MAX) {
			printf("\nError [MAIN] from [DISTRIBUTE]. Exiting.\n");
			return(-1);
		}
	} /*End while more magma to erupt (while(volume_remaining>0))*/
	
	printf("\n\nSingle Flow Complete: %d/%d cells inundated.\n\n",
	       ActiveCounts[1], workerCapacity);
	
	/*Integrate to find total volume in all inundated cells.*/
	/*i =  Worker to integrate. Now there is only one worker, so no loop needed.*/
	i=1;
	tot_vol_out = 0;
	for(j=1;j<=ActiveCounts[i];j++)
		tot_vol_out += (WorkingCells[i][j].thickness + residual) *
		               dem_row_size * dem_col_size;
	printf("Total volume pulsed from vents:   %0.3f\n",tot_vol_in);
	printf("Total volume found in cells:      %0.3f\n",tot_vol_out);
	
	ret = module_output(dataGrid,WorkingCells,workerCount,ActiveCounts,
	                    outputfilename,DEMmetadata,residual);
	if(ret){
		printf("\nError [MAIN] from [OUTPUT]. Exiting.\n");
		return(-1);
	}
	
	/*Calculate simulation time elapsed*/
	endtime = time(NULL);
	printf("\nElapsed Time of test approximately %u seconds\n\n",
	       (unsigned)(endtime - starttime));
	
	return 0;
}
