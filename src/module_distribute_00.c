#include "module_distribute_00.h"

Cell *DISTRIBUTE(Location **dataGrid, Cell *actList, unsigned *actCt,
               unsigned capacity, unsigned workerCt, double residual){
/*Module: DISTRIBUTE
	args: Global Grid, Active List, Active Count, Worker Capacity, # Workers,
	residual thickness
*/

	unsigned counter = 1;
	/*Counter starts at 1 because actList[0] would look inactive on Global Grid
	  active property*/

	/*neighbor handling variables*/
	Cell *neigh_list; /*Neighbor List containing neighbor information*/
	int neigh_ct; double neigh_diff;
	int n; unsigned ncol,nrow;
	double total_elev_diff;
	double distribute_volume;
	char parentcode; char outofbounds = 0;
	
	/*List of outer cells for P-B declaring in other lists.
	  Remain null until needed, so that a null list will be 
	  returned if not needed.*/
	Cell *ExteriorCells = 0;
	
	/*printf("Entered [DISTRIBUTE]\n");*/
	
	do { /*for all entries in active list (counter<=actCt)*/
		
		/*printf("\rDistributing from cell # %-4u of %-4u",counter,*actCt);*/
		
		if(actList[counter].thickness > 0) {
		/*if current cell's thickness > than residual, spread lava*/
			/*Find valid, lower neighbors*/
			neigh_list = module_neighbor_id(actList[counter],dataGrid,actList,&neigh_ct);
			
			if(neigh_ct<0) {
			/*If there is an error from the neighbor identification module*/
				printf(" [DISTRIBUTE] received error code from neighbor identifying module!\n");
				if((ExteriorCells=(Cell*)malloc(sizeof(Cell)))==NULL){
					printf("[DISTRIBUTE]\n   No more memory while creating Error Marker!!");
					printf(" How ironic. Exiting.\n");
					exit(0);
				}
				ExteriorCells[0].row = UINT_MAX;
				return(ExteriorCells);
			}
			
			
			else if(neigh_ct){
			/*If the neighbor list is not empty*/
				/*define amount of lava to be shared*/
				distribute_volume = actList[counter].thickness;
				
				/*define sum of reliefs between neighbor cells and current cell*/
				total_elev_diff = 0;
				for(n=0;n<neigh_ct;n++) {
				/*for each neighbor add relief between it and current cell to bookkeeper*/
					total_elev_diff += actList[counter].elev - neigh_list[n].elev;
				}
				
				for(n=0;n<neigh_ct;n++) {
					
					/*define neighbor's row, column*/
					nrow = neigh_list[n].row;
					ncol = neigh_list[n].col;
					
					if(dataGrid[nrow][ncol].active){
					/*if neighbor already active spread normally.*/
						/*elevation difference between active and neighbor cells:*/
						neigh_diff = actList[counter].elev - neigh_list[n].elev;
						
						/*add lava based on amount to share and neigh_diff*/
						actList[dataGrid[nrow][ncol].active].thickness +=
						       distribute_volume * neigh_diff / total_elev_diff;
						actList[dataGrid[nrow][ncol].active].elev      +=
						       distribute_volume * neigh_diff / total_elev_diff;
						
					} else if(*actCt < capacity) {
					/*if neighbor not in active list, check act count for over-capacity
						if under-capacity: Activate neighbor cell*/
						
						/*give parent bit-code here*/
						/*swap bits: (0th bit = right;1st=down;2nd=left;3rd=up)
						 if neighbor to activate is DOWN, its parent (current cell) is UP.*/
						
						parentcode = 0; /*reset parent code*/
						if     (ncol<actList[counter].col) parentcode |= 1 << 0;
						else if(nrow>actList[counter].row) parentcode |= 1 << 1;
						else if(ncol>actList[counter].col) parentcode |= 1 << 2;
						else if(nrow<actList[counter].row) parentcode |= 1 << 3;
						
						/*ACTIVATE*/
						*actCt = module_activation(dataGrid, actList, nrow, ncol,
						                  *actCt, parentcode, 0, residual);
						/*global grid, active list, row, column, count,
						  parent, vent (0=no), residual*/
						  
						/*Spread normally*/
						/*elevation difference between active and neighbor cells:*/
						neigh_diff = actList[counter].elev - neigh_list[n].elev;
						
						/*add lava based on amount to share and neigh_diff*/
						actList[dataGrid[nrow][ncol].active].thickness +=
						       distribute_volume * neigh_diff / total_elev_diff;
						actList[dataGrid[nrow][ncol].active].elev      +=
						       distribute_volume * neigh_diff / total_elev_diff;

					} else {
					/*if neighbor not in active list and if at capacity:
					  do not spread & FLAG so:
					  *active cell does not return to residual, active cell
					  *is checked for being on a PB list.*/
						outofbounds |= 1 << n;
					} /*End active at/not at capacity decision*/
				} /*End for each neighbor*/
				
				/*Return lava in active cell to the residual, only if not retaining 
				  it for other workers*/
				if(outofbounds == 0) {
					actList[counter].elev -= distribute_volume;
					actList[counter].thickness -= distribute_volume;
				} else outofbounds = 0;
				/*reset out of bounds for next cell if a neighbor was out of bounds*/
				
			} /*End "if there are neighbors"*/
			
			/*free neighbor list*/
			free(neigh_list);
		} /*End if(actList[counter].thickness > 0)*/
		
		counter++; /*Move to next cell*/
	} while(counter <= *actCt);
	
	/*printf("\n       [DISTRIBUTE] complete!\n");
	printf("         Inundated %u cells.\n",*actCt);*/
	
	/*return a list of Exterior Cells if there are any. 
	  If not, ExteriorCells should be a null pointer.*/
	return(ExteriorCells);
}
