#include "module_pulse_00.h"

int PULSE(Cell *actList, VentArr **ventList, unsigned actCt,
          double *vol_remaining, unsigned vent_cell_ct, double *gridinfo){
/*Module: PULSE
	If volume remains to be erupted, delivers a pre-defined pulse of magma to each
	vent cell and subtracts this pulse from the vent cell's volume. Returns the
	total remaining volume through a pointer to the vol_remaining variable.
	
	args: 1st Worker's Active List, Active Count, Total Pulse Volume, 
	        Remaining Total Volume, Vent Cell Count, grid metadata
	
	gridinfo elements used in this module:
		[1] w-e pixel resolution
		[5] n-s pixel resolution (negative value)
*/
	
	unsigned i;
	unsigned currentvent;
	double   pulse_thickness;
	VentArr  *PULSEVents; /*working array for vents in this module*/
	PULSEVents = *ventList;
	
	if (*vol_remaining > 0) {
	/*If there is still volume to pulse to vents...*/
		currentvent = 0;
		
		/*Search for Vents among the active cells*/
		for(i=0;i<=actCt;i++) {
			if (actList[i].vent) {
			/*if this cell is a vent cell (vent!=0), add pulse*/
				if (PULSEVents[currentvent].pulsevolume >
				    PULSEVents[currentvent].totalvolume)
					PULSEVents[currentvent].pulsevolume = PULSEVents[currentvent].totalvolume;
				
				
				pulse_thickness = PULSEVents[currentvent].pulsevolume / 
				                  (gridinfo[1] * gridinfo[5]);
				actList[i].thickness += pulse_thickness;
				actList[i].elev      += pulse_thickness;
				
				/*subtract vent's pulse from vent's total magma budget*/
				PULSEVents[currentvent].totalvolume -= PULSEVents[currentvent].pulsevolume;
				
				/*Move vent counter to next vent*/
				currentvent++;
				
				if(currentvent >= vent_cell_ct) {
				/*If all vents have been found and pulses delivered:*/
					/*Tally up remaining volume, from scratch*/
					*vol_remaining = 0;
					for(currentvent=0;currentvent<vent_cell_ct;currentvent++)
						*vol_remaining += PULSEVents[currentvent].totalvolume;
					
					/*Return 1, successful pulse*/
					return(1);
				}
			}
		}
	}
	else if (*vol_remaining == 0) return(0); /*No more volume to pulse.*/
	
	else { /*(*vol_remaining < 0) Volume remaining should NEVER come in below 0*/
		printf("\nError in [PULSE]: remaining volume input is negative.\n");
		return(-1);
	}
	
	/*Return -1: if pulse loop doesn't complete it's an error.*/
	printf("\nError in [PULSE]: All magma not delivered to vents/vents missing!\n");
	return(-1);
}
