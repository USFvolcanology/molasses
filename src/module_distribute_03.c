#include "prototypes.h"

int DISTRIBUTE(DataCell **dataGrid, Automata *CAList, unsigned *activeCount,
               double *gridMetadata){
/*DISTRIBUTE_03 Updates Cellular Automata ALL AT ONCE and shares lava EVENLY to
                all lower neighbors NOT-PROPORTIONAL TO SLOPE.
                The Center Cell will SHARE ALL VOLUME of lava it contains ABOVE
                the RESIDUAL VOLUME.
*/
/*Module: DISTRIBUTE
          Distributes lava from central cells which have a residual amount of
          lava to lower cells in a neighborhood.
          Arguments:
            dataCell dataGrid     - a 2D Global Data Grid
            Automata CAList       - a cellular automata list of active cells
            unsigned activeCount  - the number of elements within CAList
            double   gridMetadata - geometry of the Global Data Grid
          
          Algorithm:
            Until counter = the cell count in CAList when module was called:
              IF CAList[counter] (center cell) has enough lava to spread:
                Identify cells in local neighborhood which can receive lava
                IF there are cells:
                  Define volume to advect away from center cell
                  Calculate Cell-Cell elevation reliefs and sum them
                  FOR each neighbor:
                    Determine Effective center cell - neighbor Cell relief
                    If neighbor cell is not in CAList, append with ACTIVATE
                    Determine volume to add to neighbor cell
                    Add thickness of lava to neighbor's  IN  transition property
                  Add total vol. distributed to center's OUT transition property
              Increment counter and test against number of cells in CAList
            
            For all cells in CAList:
              update lava thicknesses with transition properties
              reset transition properties
          
          Return total number of active cells and updated CAList with pointers
*/
	
	/*VARIABLES******************************************************************/
	/*Center Cell counter, used to cycle through all active Cells (1-N). Does not
	  start at 0, because CAList[0] would look inactive in dataGrid.active.     */
	unsigned c = 1;
	/*This module only distributes lava away from already active cells, uses init.
	  active counter as c max in main do-while loop.                            */
	unsigned initialActiveCount = *activeCount;

	/*Neighborhood Variables*/
	Automata *neighborCAList;   /*CA List containing neighbor information       */
	int      neighborCount;     /*Number of lava-accepting cells in neighborhood*/
	double   neighborDist[8];   /*Weight: NESW cells = 1, corner cells = 2^-0.5 */
	double   neighborRelief[8]; /*Cell-Cell elevation difference*/
	double   neighborEffRelief; /*Cell-Cell elev difference * neigh. dist weight*/
	double   totalRelief;       /*summed Cell-Cell elevation differences        */
	unsigned n, NCol,NRow;      /*neighbor counter, grid location               */
	
	double   volumeDistribute;  /*lava volume to advect away from center cell   */
	char     parentCode;        /*bitwise parent-child relationship code        */
	
	
	/****************************************************************************/
	/*MAIN CELL DISTRIBUTE LOOP**************************************************/
	/****************************************************************************/
	/*for all entries in active Cellular Automata list (c <= initialActiveCount)*/
	do { 
		/*If center cell has more lava than the residual thickness, advect lava*/
		if(CAList[c].thickness > 0) {
			
			/*MODULE: NEIGHBOR_ID****************************************************/
			/*        Identifies lower, valid-to-spread-to cells in the center cell
			          neighborhood.*/
			neighborCAList = NEIGHBOR_ID(CAList[c],     /*Automata Center Cell      */
			                             dataGrid,      /*DataCell Global Data Grid */
			                             gridMetadata,  /*double   grid data        */
			                             CAList,        /*Automata Active Cell List */
			                             &neighborCount /*Number of ID'd Neighbors  */
			                            );
			
			/*Check for Error flag (NEIGHBOR_ID returns <0 value)*/
			if(neighborCount<0) {
				printf("ERROR [DISTRIBUTE]: Error flag returned from [NEIGHBOR_ID].\n");
				return(-1);
			}
			
			
			/*If there are found neighbors (neighborCount>0)*/
			else if(neighborCount){
				/*define amount of lava to be spread away from center cell*/
				volumeDistribute = CAList[c].thickness;
				
				/*Loop through neighbors once to find Cell-Cell reliefs, total Relief*/
				totalRelief = 0;
				for(n=0;n<neighborCount;n++) {
					/*Distance factor: NESW cells are weighted at 1, 
					  Corner cells at 2^-0.5. This makes corner cells receive less lava*/
					neighborDist[n] = (double) 1.0/sqrt(
					                  (abs(neighborCAList[n].row-CAList[c].row)+
					                   abs(neighborCAList[n].col-CAList[c].col)));
					
					/*Add EFFECTIVE relief of cell to totalRelief*/
					totalRelief += neighborDist[n];
				}
				
				/*LOOP THROUGH NEIGHBORS AGAIN to activate new cells, distribute lava*/
				for(n=0;n<neighborCount;n++) {
					
					NRow = neighborCAList[n].row; /*Current neighbor's grid location*/
					NCol = neighborCAList[n].col;
					
					/*Neighbor's Effective elevation difference from center cell*/
					neighborEffRelief = neighborDist[n]; /*Slope-blind just use distance*/
					
					
					/*if neighbor not in active list, Activate neighbor cell*/
					if(!dataGrid[NRow][NCol].active){
						/*Parent bit-code is 8-bits, denotes which cell(s) is the "parent"*/
						/*swap bits: (0th bit = parent is right;1st= p. is down;
						              2nd bit = parent is left; 3rd=p. is up;
						              4th bit = parent is right-down; 5rd=p. is left-down;
						              6th bit = parent is left-up;    7rd=p. is right-up;)
						 If neighbor to activate is DOWN, its parent (current cell) is UP.*/
						
						parentCode = 0; /*reset parent code*/
						if     ((NCol<CAList[c].col)&&(NRow>CAList[c].row))
							parentCode |= 1 << 4; /*Switch Right-Down bit (Bit 4) to ON*/
						else if((NCol>CAList[c].col)&&(NRow>CAList[c].row))
							parentCode |= 1 << 5;
						else if((NCol>CAList[c].col)&&(NRow<CAList[c].row))
							parentCode |= 1 << 6;
						else if((NCol<CAList[c].col)&&(NRow<CAList[c].row))
							parentCode |= 1 << 7;
						else if(NCol<CAList[c].col) parentCode |= 1 << 0;
						else if(NRow>CAList[c].row) parentCode |= 1 << 1;
						else if(NCol>CAList[c].col) parentCode |= 1 << 2;
						else if(NRow<CAList[c].row) parentCode |= 1 << 3;
						
						/*MODULE: ACTIVATE*************************************************/
						/*        Appends a cell to the CA List with Global Data Grid info*/
						
						*activeCount = ACTIVATE(dataGrid,     /*DataCell Global Data Grid */
						                        CAList,       /*Automata CA List          */
						                        NRow,         /*unsigned Row Location     */
						                        NCol,         /*unsigned Column Location  */
						                        *activeCount, /*unsigned No. Cells in List*/
						                        parentCode,   /*char     Parent Code      */
						                        0             /*char     Vent Code: 0=not */
						                       );
						
						/*Check for Error Flag (Active Count should never be 0 or less)*/
						if(*activeCount<=0){
							printf("\nError [DISTRIBUTE]: Error from [ACTIVATE]\n");
							return(-1);
						}
					} /*End IF neighbor cell is not yet active*/
					
					
					/*DETERMINE AMOUNT OF LAVA TO SPREAD TO NEIGHBOR FOR LATER***********/
					/*add lava based on amount to share and neighborEffRelief*/
					CAList[dataGrid[NRow][NCol].active].lava_in +=
					       volumeDistribute * neighborEffRelief / totalRelief;
					
				} /*End FOR all neighbors*/
				
				/*DETERMINE AMOUNT OF LAVA TO REMOVE FROM CENTER CELL FOR LATER********/
				CAList[c].lava_out += volumeDistribute;
				
			} /*End IF there are lava-accepting neighbors*/
			
			/*free neighbor list from memory*/
			free(neighborCAList);
			
		} /*End IF lava is thick enough to advect (CAList[c].thickness > 0)*/
		
		c++; /*Move to next cell*/
	} while(c <= initialActiveCount); /*Keep looping until cells which were     */
	                                  /*already active at start have been tested*/
	
	/*UPDATE ALL CELL LAVA LEVELS*/
	for(c=1;c<=*activeCount;c++) {
		/*Change both flow thickness and effective elevation*/
		CAList[c].elev      += CAList[c].lava_in;
		CAList[c].thickness += CAList[c].lava_in;
		
		CAList[c].elev      -= CAList[c].lava_out;
		CAList[c].thickness -= CAList[c].lava_out;
		
		/*Reset transition properties*/
		CAList[c].lava_in    = CAList[c].lava_out = 0.0;
	}
	
	/*return 0 for a successful round of distribution.*/
	return(0);
}
