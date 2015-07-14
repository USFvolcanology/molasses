#include "prototypes.h"

int DISTRIBUTE(Location **dataGrid, Cell *actList, unsigned *actCt,
               double *gridmetadata){
/*Module: DISTRIBUTE_04
	args: Global Grid, Active List, Active Count, DEM Metadata
	
	DISTRIBUTE_04 is a BLIND-SHARE Distribution scheme! All lower neighbors are
	              given an amount of lava proportional to their distance from the
	              active cell, NOT based on their relative elevations heights.
	              
	              It is advised that this is used with a DRIVER module that ends
	              when the flow is no longer deforming, as opposed to a DRIVER
	              module that ends when the volume at the vent has run out.
	              
	              This is also a NEW PARENTS Distribution scheme! All Parent-child
	              relationships are reset when this module is called. All cells
	              that give lava to other cells become the parent of those cells.
*/

	unsigned counter = 1;
	/*Counter starts at 1 because actList[0] would look inactive on Global Grid
	  active property*/

	/*neighbor handling variables*/
	Cell *neigh_list; /*Neighbor List containing neighbor information*/
	int neigh_ct; double neigh_diff;
	double neigh_distance[8]; /*This is a factor in how to spread to diag. cells*/
	int n; unsigned ncol,nrow;
	double total_elev_diff;
	double distribute_volume;
	unsigned total_distribute = 0;
	
	/*printf("Entered [DISTRIBUTE]\n");*/
	
	/*RESET PARENTS*/
	for(counter=1;counter<=*actCt;counter++) 
		actList[counter].parents = 0;
	
	counter = 1;
	
	do { /*for all entries in active list (counter<=actCt)*/
		
		/*printf("\rDistributing from cell # %-4u of %-4u",counter,*actCt);*/
		
		if(actList[counter].thickness > 0) {
		/*if current cell's thickness > than residual, spread lava*/
			
			/*Find valid, lower neighbors*/
			neigh_list = NEIGHBOR_ID(actList[counter],dataGrid,gridmetadata,
			                                actList,&neigh_ct);
			if(neigh_ct<0) {
			/*If there is an error from the neighbor identification module*/
				printf(" [DISTRIBUTE] received error code from neighbor identifying module!\n");
				return(-1);
			}
			
			else if(neigh_ct){
			/*If the neighbor list is not empty*/
				total_distribute++; /*This is a counter of cells that have shared lava*/
				
				/*define amount of lava to be shared*/
				distribute_volume = actList[counter].thickness;
				
				/*define sum of reliefs between neighbor cells and current cell*/
				total_elev_diff = 0;
				for(n=0;n<neigh_ct;n++) {
					/*Define Distance Factor (diagonal cells should get less)*/
					neigh_distance[n]=(double) 1.0/sqrt(
					                     (abs(neigh_list[n].row-actList[counter].row)+
					                     abs(neigh_list[n].col-actList[counter].col)));
					
					/*add each neighbor bookkeeper, times its distance factor*/
					total_elev_diff += neigh_distance[n];
				}
				
				for(n=0;n<neigh_ct;n++) {
					
					/*define neighbor's row, column*/
					nrow = neigh_list[n].row;
					ncol = neigh_list[n].col;
					
					/*define neighbor's effective elevation difference*/
					neigh_diff = neigh_distance[n];
					
					if(!dataGrid[nrow][ncol].active){
					/*if neighbor not in active list, Activate neighbor cell*/
						/*ACTIVATE*/
						*actCt = ACTIVATE(dataGrid, actList, nrow, ncol,
						                           *actCt, 0, 0);
						/*args: global grid, active list, row, column, count,
						  parent, vent (0=no)*/
					}
					
					/*SPREAD LAVA TO NEIGHBOR*/
					/*add lava based on amount to share and neigh_diff*/
					actList[dataGrid[nrow][ncol].active].thickness +=
					       distribute_volume * neigh_diff / total_elev_diff;
					actList[dataGrid[nrow][ncol].active].elev      +=
					       distribute_volume * neigh_diff / total_elev_diff;
					
					/*ADD THIS CELL AS A PARENT OF THE NEIGHBOR*/
					/*give parent bit-code here*/
						/*swap bits: (0th bit = parent is right;1st= p. is down;
						              2nd bit = parent is left; 3rd=p. is up;
						              4th bit = parent is right-down; 5rd=p. is left-down;
						              6th bit = parent is left-up;    7rd=p. is right-up;)
						 if neighbor to activate is DOWN, its parent (current cell) is UP.*/
					
					if     ((ncol<actList[counter].col)&&(nrow>actList[counter].row))
						actList[dataGrid[nrow][ncol].active].parents |= 1 << 4;
					else if((ncol>actList[counter].col)&&(nrow>actList[counter].row))
						actList[dataGrid[nrow][ncol].active].parents |= 1 << 5;
					else if((ncol>actList[counter].col)&&(nrow<actList[counter].row))
						actList[dataGrid[nrow][ncol].active].parents |= 1 << 6;
					else if((ncol<actList[counter].col)&&(nrow<actList[counter].row))
						actList[dataGrid[nrow][ncol].active].parents |= 1 << 7;
					else if(ncol<actList[counter].col)
						actList[dataGrid[nrow][ncol].active].parents |= 1 << 0;
					else if(nrow>actList[counter].row) 
						actList[dataGrid[nrow][ncol].active].parents |= 1 << 1;
					else if(ncol>actList[counter].col) 
						actList[dataGrid[nrow][ncol].active].parents |= 1 << 2;
					else if(nrow<actList[counter].row) 
						actList[dataGrid[nrow][ncol].active].parents |= 1 << 3;
					
				} /*End for each neighbor*/
				
				actList[counter].elev -= distribute_volume;
				actList[counter].thickness -= distribute_volume;
			} /*End "if there are neighbors"*/
			
			/*free neighbor list from memory*/
			free(neigh_list);
		} /*End if(actList[counter].thickness > 0)*/
		
		counter++; /*Move to next cell*/
	} while(counter <= *actCt);
	
	/*printf("\n       [DISTRIBUTE] complete!\n");
	printf("         Inundated %u cells.\n",*actCt);*/
	
	/*return 0 for a successful round of distribution.*/
	return(total_distribute);
}
