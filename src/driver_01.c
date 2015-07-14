#include "prototypes.h"

int main(int argc, char *argv[]) {
/*
DRIVER_01 is a FLOW-MOTION LIMITED flow scheme! The flow will end when all vents
          have no more lava to effuse AND when the flow is no longer innundating
          new grid cells.
*/
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
	char     **Filenames;
	char     pulseoutputfilename[15];
	int      pulse_ct = 0;
	/*FILE     *demfile;*/
	double   *DEMmetadata;
	
	/*Arrays*/
	Location **dataGrid;
	Cell **WorkingCells;
	unsigned *ActiveCounts; /*list of active counts for each worker*/
	
	/*End Times Calculators*/
	double TotalMotion = 0; /*Bookkeeper to quantify flow change*/
	double MinTotalMotion = 1e-11; /*Threshold to detect if flow is changing or not*/
	time_t LastInundationTime;
	double maxLastInundationTime = 10.0;
	unsigned lastActiveCounter = 0;
	unsigned lastmotionCounter = 0; /*Bookkeeper to count loops since last motion*/
	unsigned maxLastMotionCount = 10; /*Threshold to stop flow*/
	
	/*Model Parameters*/
	int      i,j;
	/*workerC* variables are not currently defined here. workerCount is a built-in
	  variable for future attempts at parallelization*/
	unsigned workerCount = 0; /*declare lists for first and second worker*/
	unsigned workerCapacity = 100000; /*declare length of worker lists*/
	unsigned ventCount = 0;
	int      ret; /*return integer*/
	
	/*Physical Parameters*/
	VentArr   *Vents;
	double    residual;
	double    elev_uncert;
	double    volume_remaining;
	int       volume_remaining_counter = 0; /*0 if volume remaining, 1 if no volume left*/
	double    tot_vol_in;
	double    tot_vol_out = 0; /*bookkeeper to check for total volume in cells*/
	
	starttime = time(NULL);
	srand(time(NULL)); /*Seed random number generator*/
	printf("\n\n                  Driver for a lava flow code.\n\n");
	if(argc<2) {
		printf("Usage: %s config-filename\n",argv[0]);
		return(-1);
	}
	
	printf("Beginning flow simulation...\n");
	
	ret = INITIALIZE(CFGfilename, &Filenames,
	                        &residual, &elev_uncert, &Vents, &ventCount);
	/*char *CFGfilename, char ***Filenames, double *param_modalthickness, 
	       *param_elevunc, VentArr *Vents, unsigned *ventct
	  Outputs Raster Filenames:
	   [0] - DEM
	   [1] - Residual
	   [2] - Elevation Uncertainty
	   [3] - Output file: ASCII X,Y,Thickness
	   [4] - Output file: Hit Map
	   [5] - Output file: Raster Thickness
	   [6] - Output file: Raster Elevation
	   [7] - Output file: Raster Elevation + Flow Thickness
	*/
	if(ret<0){
		printf("\nError from [INITIALIZE]. Exiting.\n");
		return(-1);
	}
	
	/*Load DEM into Global Data Grid*/
	/*     DEM_LOADER Codes: RESID,TOPOG,T_UNC,AOI_C*/
	DEMmetadata = DEM_LOADER(Filenames[0],&dataGrid,"TOPOG");
	/*DEMmetadata format:
		[0] lower left x
		[1] w-e pixel resolution
		[2] number of cols, assigned manually
		[3] lower left y
		[4] number of lines, assigned manually
		[5] n-s pixel resolution (negative value)
	*/
	if(DEMmetadata==NULL){
		printf("\nError [MAIN] from DEM_LOADER[TOPOG]. Exiting.\n");
		return(-1);
	}
	
	/*Assign Residual Thickness to Data Grid Locations*/
	if(residual==-1) {
		DEMmetadata = DEM_LOADER(Filenames[2],&dataGrid,"RESID");
		if(DEMmetadata==NULL){
			printf("\nError [MAIN] from DEM_LOADER[RESID]. Exiting.\n");
			return(-1);
		}
	}
	else {
		/*Write residual data column by column into 2D array using DEMgrid variable*/
		for(i=0;i<DEMmetadata[4];i++) {
			for(j=0;j<DEMmetadata[2];j++) {
				dataGrid[i][j].residual = residual;
			}
		}
	}
	
	/*Assign Elevation Uncertainty to Data Grid Locations*/
	if(elev_uncert==-1) {
		DEMmetadata = DEM_LOADER(Filenames[3],&dataGrid,"T_UNC");
		if(DEMmetadata==NULL){
			printf("\nError [MAIN] from DEM_LOADER[T_UNC]. Exiting.\n");
			return(-1);
		}
	}
	else {
		/*Write elev_uncert data*/
		for(i=0;i<DEMmetadata[4];i++) {
			for(j=0;j<DEMmetadata[2];j++) {
				dataGrid[i][j].elev_uncert = elev_uncert;
			}
		}
	}
	
	/*Initialize Flow by activating vents and creating active lists*/
	ret = INIT_FLOW(dataGrid, &WorkingCells, Vents, &workerCount, 
	                       &workerCapacity, ventCount, &ActiveCounts, DEMmetadata, 
	                       &tot_vol_in);
	/*args: data array, *active list, vent array, *worker count, worker capacity,
	  vent count, *active counts, DEM transform data, *total volume in*/
	
	if(ret<0) {
		printf("\nError [MAIN] from [INIT_FLOW]. Exiting.\n");
		return(-1);
	}
	
	volume_remaining = tot_vol_in;
	
	printf("\n                         Running Flow\n");
	
	
	
	
	
	
	
	/*****LOOP to Pulse and Distribute Lava*************************************/
	while((volume_remaining > 0)||(lastmotionCounter<maxLastMotionCount)) {
		
		if(volume_remaining > 0) {
			/*PULSE MODULE*/
			ret = PULSE(WorkingCells[1],&Vents,ActiveCounts[1],
				                 &volume_remaining,ventCount,DEMmetadata);
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
		printf("\rInundated Cells: %-7d; Volume Remaining: %10.2f",ActiveCounts[1],
		       volume_remaining);
		}
		else { /*if no more volume but flow still moving*/
			printf("\rInundated Cells: %-7d; total dZ in flow: %10.2e",ActiveCounts[1],
		       TotalMotion);
		}
		
		
		/*DISTRIBUTE MODULE*/
		ret = DISTRIBUTE(dataGrid,WorkingCells[1],
		                &ActiveCounts[1],DEMmetadata);
		
		if(ret<0) {
			printf("\nError [MAIN] from [DISTRIBUTE]. Exiting.\n");
			return(-1);
		}
		
		/*OUTPUT TO FILE EVERY PULSE*/
		snprintf(pulseoutputfilename,15,"pulse_%04d.xyz",(++pulse_ct));
		/*ret = OUTPUT(dataGrid,WorkingCells,workerCount,ActiveCounts,
	                    pulseoutputfilename,DEMmetadata);
		if(ret){
			printf("\nError [MAIN] from [OUTPUT]. Exiting.\n");
			return(-1);
		}*/
		
		
		
		
		
		
		/*TEST TO SEE IF FLOW IS STILL PROGRESSING, IF VOLUME HAS RUN OUT*/
		
		
		
		if(volume_remaining<=0) {
			/*If this is the first time volume is 0, start new output line*/
			if((volume_remaining_counter++)==0) printf("\n");
			
			/*MOTION TEST - If movement has stopped, end the flow*/
			TotalMotion=0.0; /*Reset TotalMotion to 0*/
			for(i=1;i<=ActiveCounts[1];i++) {
				/*Add dv/dloop of all active cells*/
				TotalMotion += fabs(WorkingCells[1][i].elev - WorkingCells[2][i].elev);
			}
			
			/*Increment lastmotionCounter if flow has not changed, or reset to 0.*/
			if(TotalMotion <= MinTotalMotion) ++lastmotionCounter;
			else{ /*reset counter to zero and reset spare working cell values*/
				lastmotionCounter=0;
				for(i=1;i<=ActiveCounts[1];i++) {
					/*redefine spare working cells elevation list as "elevations of last time"*/
					WorkingCells[2][i].elev = WorkingCells[1][i].elev;
				}
			}
			
			/*POPCORN RULE - If a new cell hasn't been created in X seconds, end flow*/
			if(lastActiveCounter!=ActiveCounts[1]) {/*If there's a new cell*/
				LastInundationTime = time(NULL);   /*Reset Last inundation time*/
				lastActiveCounter=ActiveCounts[1]; /*Reset lastActiveCounter*/
			}
			else { /*If there's no new cell*/
				if(((unsigned) (time(NULL)-LastInundationTime))>=maxLastInundationTime) {
					/*If max time to wait for a new cell has been reached, end flow*/
					printf("\n Ending Flow because time elapsed since last cell was inundated (%u seconds)",
					       (unsigned) (time(NULL)-LastInundationTime));
					lastmotionCounter=maxLastMotionCount;
				}
			
			}
		}
		
		
	} /*End while more magma to erupt (while(volume_remaining>0)and Flow Motion)*/
	

	/**********POST FLOW WRAP UP************************************************/
	
	printf("\n\n                     Single Flow Complete!\n");
	printf("Final Count: %d cells inundated.\n\n",
	       ActiveCounts[1]);
	
	
	/*CONSERVATION OF MASS CHECK*************************************************/
	
	/*Integrate to find total volume in all inundated cells.*/
	/*i =  Worker to integrate. Now there is only one worker, so no loop needed.*/
	i=1;
	tot_vol_out = 0;
	for(j=1;j<=ActiveCounts[i];j++)
		tot_vol_out += (WorkingCells[i][j].thickness + 
		               dataGrid[WorkingCells[i][j].row][WorkingCells[i][j].col].residual) *
		               DEMmetadata[1] * DEMmetadata[5];

	printf("Conservation of mass check\n");
	printf(" Total (IN) volume pulsed from vents:   %0.3f\n",tot_vol_in);
	printf(" Total (OUT) volume found in cells:     %0.3f\n",tot_vol_out);
	if(abs(tot_vol_out-tot_vol_in)<=1e-8) printf(" SUCCESS: MASS CONSERVED\n");
	else 
		printf(" ERROR: MASS NOT CONSERVED! Excess: %0.2e m",
		       tot_vol_out-tot_vol_in);
	
	/*OUTPUT*********************************************************************/
	/*Filenames from configuration file
	   Filenames[3] - Output file: ASCII X,Y,Thickness
	   Filenames[4] - Output file: Hit Map
	   Filenames[5] - Output file: Raster Thickness
	   Filenames[6] - Output file: Raster Elevation
	   Filenames[7] - Output file: Raster Elevation + Flow Thickness
	
	  File types to output:
	    0: X,Y,Thickness ascii flow list
	    1: Hit Raster (1 = Hit, 0 = Not Hit)
	    2: Thickness Raster
	    3: Elevation Raster
	    4: Elevation + Lava Raster (2+3)
	*/
	
	for(i=0;i<=4;i++){
		if(strcmp(Filenames[i+3], "")) {
		/*If there's a file set for an output type, output that information here*/
			ret = OUTPUT(dataGrid, WorkingCells[1], ActiveCounts[1],
					                Filenames[i+3], i, DEMmetadata,"");
			if(ret){
				printf("\nError [MAIN] from [OUTPUT]. Exiting.\n");
				return(-1);
			}
		}
	}
	if(ret){
		printf("\nError [MAIN] from [OUTPUT]. Exiting.\n");
		return(-1);
	}
	
	/*Calculate simulation time elapsed*/
	endtime = time(NULL);
	printf("\nElapsed Time of simulation approximately %u seconds.\n\n",
	       (unsigned)(endtime - starttime));
	
	return 0;
}
