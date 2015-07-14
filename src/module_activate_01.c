#include "prototypes.h"

unsigned ACTIVATE(Location **grid, Cell *list, unsigned row, unsigned col, 
                  unsigned aCt, char parent, char vent) {
/*ACTIVATE_01 is a PARENT-LESS Activation Sceme! No parents are recorded.
              
              It is advised that this is used with a DRIVER module that ends
              when the flow is no longer deforming, as opposed to a DRIVER
              module that ends when the volume at the vent has run out.*/

/*Module: ACTIVATE
	accepts a location marker (row,column)
	checks active value in a grid of typedef Location (global grid)
	assigns data to list of typdef Cell (active list) from grid
	
	if active value == 0 (inactive), changes:
		active count (aCt) is incremented
		grid[row][column] active value <- aCt
		list[aCt] data values <- grid[row][column]
		list[aCt] vent value <- vent argument
		list[aCt] thickness value <- -1*residual argument
		
	return the active count. 
	Active count should never be 0, so this can be used for error handling.
*/

	/*If inactive:*/
	if(!grid[row][col].active) {
		/*Here is where you can check for AOI*/
		
		aCt++;
		grid[row][col].active = (unsigned) aCt;
		
		list[aCt].row = (unsigned) row;
		list[aCt].col = (unsigned) col;
		list[aCt].elev = grid[row][col].elev;
		list[aCt].vent = (char) vent;
		list[aCt].lava_in = 0.0;
		list[aCt].lava_out = 0.0;
		list[aCt].thickness = (double) -1*grid[row][col].residual;
		list[aCt].parents = (char) 0;
		
		return(aCt); /*Return active count if just activated*/
	}
	
	return(aCt); /*Return active count if cell was active; nothing changes*/
}

