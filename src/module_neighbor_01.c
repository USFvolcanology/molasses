#include "prototypes.h"

Cell *NEIGHBOR_ID(Cell current_cell, Location **grid, double *gridmetadata,
                  Cell *list, int *neighbor_count) {
/*Module: NEIGHBOR_ID (8 Directions)
	identifies 8-neighbors, List of Cells (8 long) updated, # of eligible-for-lava
	neighbors returned
	
	*Accepts a Cell structure (active cell location)
	*Checks length of neighbor list for validity (must be at least 4 Cells long)
	
	*If location is at global grid boundary:
		return "off the grid" error, -1 to -4
	Else:
	*finds neighbors in cardinal directions:
		that aren't parent cells (using bitswitching: 0th bit = right;1=d;2=l;3=u
		with lower elevations
	*Flags eligible neighbors in neighbor list
	*Updates neighbor count
	
	*Neighbor list is passed back as a pointer with rows, cols, elevs of 
	 eligible neighbors
	*Returns neighbor count
*/
	
	char parents;                           /*Parent bookkeper*/
	int up,down,left,right,cur_row,cur_col; /*Geographic bookkeepers*/
	Cell *neighbor_list;
	
	/*Create the neighbor list array*/
	if((neighbor_list = (Cell*)malloc(8 * sizeof(Cell)))==NULL) {
		printf("\n[NEIGHBOR_ID]   No more memory while creating Neighbor List!!");
		printf("  Exiting.\n");
		exit(0);
	}
	
	/*Test to see if neighbor list is large enough for this module/is declared*/
	/*if ((n = sizeof(neighbor_list)/sizeof(Cell)) < 8) {
		printf("Error in [NEIGHBOR_ID]: Neighbor List array received is too small\n");
		return(-9);
	}*/
	
	/*Reset neighbor_list values to 0*/
	/*for (i=0;i<n;i++) {
		neighbor_list[i].row  = 0;
		neighbor_list[i].col  = 0;
		neighbor_list[i].elev = 0;
	}*/
	
	*neighbor_count = 0; /*Reset neighbor counter*/
	
	/*define column and row numbers for current cell and neighbors*/
	cur_row = current_cell.row;
	cur_col = current_cell.col;
	up = cur_row + 1;
	down = cur_row - 1;
	right = cur_col + 1;
	left = cur_col - 1;

	/*Off grid handling*/
	/*gridmetadata format:
		[0] lower left x
		[1] w-e pixel resolution
		[2] number of cols, assigned manually      **********
		[3] lower left y
		[4] number of lines, assigned manually     **********
		[5] n-s pixel resolution (negative value)
	*/
	if (down < 0) {
		printf("\nFLOW IS OFF THE MAP! (South) [NEIGHBOR_ID]\n");
		*neighbor_count = -1;
		return(neighbor_list);
	} else if (up >= gridmetadata[4]) {
		printf("\nFLOW IS OFF THE MAP! (North) [NEIGHBOR_ID]\n");
		*neighbor_count = -2;
		return(neighbor_list);
	} else if (left < 0) {
		printf("\nFLOW IS OFF THE MAP! (West) [NEIGHBOR_ID]\n");
		*neighbor_count = -3;
		return(neighbor_list);
	} else if (right >= gridmetadata[2]) {
		printf("\nFLOW IS OFF THE MAP! (East) [NEIGHBOR_ID]\n");
		*neighbor_count = -4;
		return(neighbor_list);
	}

	/*Parent finding requires bit switching*/
	parents = current_cell.parents;
	
	/****
	**FOR EACH DIRECTION, TEST FOR PARENTAGE, ACTIVE STATUS, ELEVATION
	****/
	
	/*NORTH*/
	if(!(parents & (1 << 3))) { 
	/*if 3rd parent bit False, UPWARD cell is not parent: Continue*/
		if(!(grid[up][cur_col].active)) {
		/*if UPWARD cell is inactive in Global Grid, use grid elevation value*/
			
			if(current_cell.elev > grid[up][cur_col].elev) {
			/*if active cell's elevation is higher than UPWARD cell, flag up cell*/
				neighbor_list[*neighbor_count].row  = up;
				neighbor_list[*neighbor_count].col  = cur_col;
				neighbor_list[*neighbor_count].elev = grid[up][cur_col].elev;
				*neighbor_count+=1;
			}
		} else {
		/*if UPWARD cell is active in Global Grid, use active list elev value*/
			if(current_cell.elev > list[grid[up][cur_col].active].elev) {
			/*if active cell's elevation is higher than UPWARD cell, flag up cell*/
				neighbor_list[*neighbor_count].row  = up;
				neighbor_list[*neighbor_count].col  = cur_col;
				neighbor_list[*neighbor_count].elev = list[grid[up][cur_col].active].elev;
				*neighbor_count+=1;
			}
		}
	}
	
	/*SOUTH*/
	if(!(parents & (1 << 1))) {
	/*if 1st parent bit False, DOWNWARD cell is not parent: Continue*/
		if(!(grid[down][cur_col].active)) {
		/*if DOWNWARD cell is inactive in Global Grid, use grid elevation value*/
			if(current_cell.elev > grid[down][cur_col].elev) {
			/*if active cell's elevation is higher than DOWNWARD cell, flag down cell*/
				neighbor_list[*neighbor_count].row  = down;
				neighbor_list[*neighbor_count].col  = cur_col;
				neighbor_list[*neighbor_count].elev = grid[down][cur_col].elev;
				*neighbor_count+=1;
			}
		} else {
		/*if DOWNWARD cell is active in Global Grid, use active list elev value*/
			if(current_cell.elev > list[grid[down][cur_col].active].elev) {
			/*if active cell's elevation is higher than DOWNWARD cell, flag down cell*/
				neighbor_list[*neighbor_count].row  = down;
				neighbor_list[*neighbor_count].col  = cur_col;
				neighbor_list[*neighbor_count].elev = list[grid[down][cur_col].active].elev;
				*neighbor_count+=1;
			}
		}
	}
	
	/*WEST*/
	if(!(parents & (1 << 2))) { 
	/*if 2nd parent bit False, LEFTWARD cell is not parent: Continue*/
		if(!(grid[cur_row][left].active)) {
		/*if LEFTWARD cell is inactive in Global Grid, use grid elevation value*/
			if(current_cell.elev > grid[cur_row][left].elev) {
			/*if active cell's elevation is higher than LEFTWARD cell, flag left cell*/
				neighbor_list[*neighbor_count].row  = cur_row;
				neighbor_list[*neighbor_count].col  = left;
				neighbor_list[*neighbor_count].elev = grid[cur_row][left].elev;
				*neighbor_count+=1;
			}
		} else {
		/*if LEFTWARD cell is active in Global Grid, use active list elev value*/
			if(current_cell.elev > list[grid[cur_row][left].active].elev) {
			/*if active cell's elevation is higher than LEFTWARD cell, flag left cell*/
				neighbor_list[*neighbor_count].row  = cur_row;
				neighbor_list[*neighbor_count].col  = left;
				neighbor_list[*neighbor_count].elev = list[grid[cur_row][left].active].elev;
				*neighbor_count+=1;
			}
		}
	}
	
	/*EAST*/
	if(!(parents & (1 << 0))) { 
	/*if 0th parent bit False, RIGHTWARD cell is not parent: Continue*/
		if(!(grid[cur_row][right].active)) {
		/*if RIGHTWARD cell is inactive in Global Grid, use grid elevation value*/
			if(current_cell.elev > grid[cur_row][right].elev) {
			/*if active cell's elevation is higher than RIGHTWARD cell, flag right cell*/
				neighbor_list[*neighbor_count].row  = cur_row;
				neighbor_list[*neighbor_count].col  = right;
				neighbor_list[*neighbor_count].elev = grid[cur_row][right].elev;
				*neighbor_count+=1;
			}
		} else {
		/*if RIGHTWARD cell is active in Global Grid, use active list elev value*/
			if(current_cell.elev > list[grid[cur_row][right].active].elev) {
			/*if active cell's elevation is higher than RIGHTWARD cell, flag right cell*/
				neighbor_list[*neighbor_count].row  = cur_row;
				neighbor_list[*neighbor_count].col  = right;
				neighbor_list[*neighbor_count].elev = list[grid[cur_row][right].active].elev;
				*neighbor_count+=1;
			}
		}
	}
	
	/*DIAGONALS*/
	
	/*SOUTHWEST*/
	if(!(parents & (1 << 5))) { 
	/*if 5th parent bit False, LEFT-DOWNWARD cell is not parent: Continue*/
		if(!(grid[down][left].active)) {
		/*if L-D-WARD cell is inactive in Global Grid, use grid elevation value*/
			if(current_cell.elev > grid[down][left].elev) {
			/*if active cell's elevation is higher than L-D-WARD cell, flag L-D cell*/
				neighbor_list[*neighbor_count].row  = down;
				neighbor_list[*neighbor_count].col  = left;
				neighbor_list[*neighbor_count].elev = grid[down][left].elev;
				*neighbor_count+=1;
			}
		} else {
		/*if L-D-WARD cell is active in Global Grid, use active list elev value*/
			if(current_cell.elev > list[grid[down][left].active].elev) {
			/*if active cell's elevation is higher than L-D-WARD cell, flag L-D cell*/
				neighbor_list[*neighbor_count].row  = down;
				neighbor_list[*neighbor_count].col  = left;
				neighbor_list[*neighbor_count].elev = list[grid[down][left].active].elev;
				*neighbor_count+=1;
			}
		}
	}
	
	/*SOUTHEAST*/
	if(!(parents & (1 << 4))) { 
	/*if 4th parent bit False, RIGHT-DOWNWARD cell is not parent: Continue*/
		if(!(grid[down][right].active)) {
		/*if R-D-WARD cell is inactive in Global Grid, use grid elevation value*/
			if(current_cell.elev > grid[down][right].elev) {
			/*if active cell's elevation is higher than R-D-WARD cell, flag R-D cell*/
				neighbor_list[*neighbor_count].row  = down;
				neighbor_list[*neighbor_count].col  = right;
				neighbor_list[*neighbor_count].elev = grid[down][right].elev;
				*neighbor_count+=1;
			}
		} else {
		/*if R-D-WARD cell is active in Global Grid, use active list elev value*/
			if(current_cell.elev > list[grid[down][right].active].elev) {
			/*if active cell's elevation is higher than R-D-WARD cell, flag R-D cell*/
				neighbor_list[*neighbor_count].row  = down;
				neighbor_list[*neighbor_count].col  = right;
				neighbor_list[*neighbor_count].elev = list[grid[down][right].active].elev;
				*neighbor_count+=1;
			}
		}
	}
	
	/*NORTHEAST*/
	if(!(parents & (1 << 7))) { 
	/*if 7th parent bit False, RIGHT-UPWARD cell is not parent: Continue*/
		if(!(grid[up][right].active)) {
		/*if R-U-WARD cell is inactive in Global Grid, use grid elevation value*/
			if(current_cell.elev > grid[up][right].elev) {
			/*if active cell's elevation is higher than R-U-WARD cell, flag R-U cell*/
				neighbor_list[*neighbor_count].row  = up;
				neighbor_list[*neighbor_count].col  = right;
				neighbor_list[*neighbor_count].elev = grid[up][right].elev;
				*neighbor_count+=1;
			}
		} else {
		/*if R-U-WARD cell is active in Global Grid, use active list elev value*/
			if(current_cell.elev > list[grid[up][right].active].elev) {
			/*if active cell's elevation is higher than R-U-WARD cell, flag R-U cell*/
				neighbor_list[*neighbor_count].row  = up;
				neighbor_list[*neighbor_count].col  = right;
				neighbor_list[*neighbor_count].elev = list[grid[up][right].active].elev;
				*neighbor_count+=1;
			}
		}
	}
	
	
	/*NORTHWEST*/
	if(!(parents & (1 << 6))) { 
	/*if 6th parent bit False, LEFT-UPWARD cell is not parent: Continue*/
		if(!(grid[up][left].active)) {
		/*if L-U-WARD cell is inactive in Global Grid, use grid elevation value*/
			if(current_cell.elev > grid[up][left].elev) {
			/*if active cell's elevation is higher than L-U-WARD cell, flag L-U cell*/
				neighbor_list[*neighbor_count].row  = up;
				neighbor_list[*neighbor_count].col  = left;
				neighbor_list[*neighbor_count].elev = grid[up][left].elev;
				*neighbor_count+=1;
			}
		} else {
		/*if L-UWARD cell is active in Global Grid, use active list elev value*/
			if(current_cell.elev > list[grid[up][left].active].elev) {
			/*if active cell's elevation is higher than L-UWARD cell, flag L-U cell*/
				neighbor_list[*neighbor_count].row  = up;
				neighbor_list[*neighbor_count].col  = left;
				neighbor_list[*neighbor_count].elev = list[grid[up][left].active].elev;
				*neighbor_count+=1;
			}
		}
	}
	
	
	
	/*neighbor counter is updated as a pointer and is passed back to [DISTRIBUTE]*/
	/*return list of neighbors*/
	return neighbor_list;
}
