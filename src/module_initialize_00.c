#include "module_initialize_00.h"

int INITIALIZE(char *CFGfilename, char **DEMfilename, char **OUTfilename, 
               double *param_modalthickness, VentArr **Vents, unsigned *ventct) {
/*Module INITIALIZE
	Accepts a configuration file and returns model variables. 
	
	string DEM_FILE
	string FLOW_FILE
	double  MODAL_THICKNESS
	double  PULSE_VOLUME
	double  TOTAL_VOLUME
	double  VENT_EASTING
	double  VENT_NORTHING
*/
	unsigned max_line_length = 256;
	char     line[256];
	char     var[64];
	char     value[256];
	char     *ptr;
	VentArr  *INITVents; /*working array for vents in this module*/

	FILE *ConfigFile;

	printf("Reading in Parameters...\n");
	
	/*open configuration file*/
	ConfigFile = fopen(CFGfilename, "r");
	if (ConfigFile == NULL) {
		printf("\nError:[INITIALIZE] Cannot open configuration file=[%s]:[%s]!\n",
		       CFGfilename,
		       strerror(errno));
		return(-1);
	}

	/* use each line to compare to needed values*/
	while (fgets(line, max_line_length, ConfigFile) != NULL) {
		/*if first character is comment, new line, space, return to next line*/
		if (line[0] == '#' || line[0] == '\n' || line[0] == ' ') continue;
		
		/*print incoming parameter*/
		sscanf (line,"%s = %s",var,value); /*split line into before ' = ' and after*/
		if (strncmp(var, "NEW_VENT", strlen("NEW_VENT"))) /*dont print if line=newvent*/
			printf("%20s = %-33s ",var,value); /*print incoming parameter value*/
		else printf("%20s (#%u)\n","NEW VENT",(*ventct+1)); /*print new vent flag*/
		
		/*INPUT DEM FILE*/
		if (!strncmp(var, "DEM_FILE", strlen("DEM_FILE"))) {
			if((*DEMfilename = (char*) malloc (sizeof (char) * (strlen(value)+1)))==NULL) {
				printf("\n[INITIALIZE] Out of Memory assigning DEM file name!\n");
				return(-1);
			}
			strcpy(*DEMfilename,value);
			printf("[assigned]\n");
		}
		
		/*OUTPUT FLOW FILE*/
		else if (!strncmp(var, "FLOW_FILE", strlen("FLOW_FILE"))) {
			if((*OUTfilename = (char*) malloc (sizeof (char) * (strlen(value)+1)))==NULL) {
				printf("\n[INITIALIZE] Out of Memory assigning output file name!\n");
				return(-1);
			}
			strcpy(*OUTfilename,value);
			printf("[assigned]\n");
		}
		
		/*MODAL THICKNESS*/
		else if (!strncmp(var, "MODAL_THICKNESS", strlen("MODAL_THICKNESS"))) {
			/*printf("%s is %s\n", var,value);*/
			*param_modalthickness = strtod(value,&ptr);
			printf("[assigned]\n");
		}
		
		/*VENT PARAMETERS*/
		else if (!strncmp(line, "NEW_VENT", strlen("NEW_VENT"))) {
			/*If a new vent is declared in the configuration file, check for existing
			  vent array*/
			if(*ventct==0) {
				/*Create vent array*/
				if((*Vents = (VentArr*) malloc (sizeof (VentArr) * ((++*ventct)+1)))
				    ==NULL) {
					printf("\n[INITIALIZE] Out of Memory creating vent array!\n");
					return(-1);
				}
				INITVents = *Vents;
				
				/*Set first vent values to DBL_MAX, to indicate they are not yet full*/
				INITVents[*ventct-1].northing    = DBL_MAX;
				INITVents[*ventct-1].easting     = DBL_MAX;
				INITVents[*ventct-1].totalvolume = DBL_MAX;
				INITVents[*ventct-1].pulsevolume = DBL_MAX;
			}
			
			else { /*If vent array already exists*/
				/*check that the last vent entered is full.*/
				if(((INITVents[*ventct-1].northing    != DBL_MAX)  &&
				    (INITVents[*ventct-1].easting     != DBL_MAX)) &&
				   ((INITVents[*ventct-1].totalvolume != DBL_MAX)  &&
				    (INITVents[*ventct-1].pulsevolume != DBL_MAX))) {
					
					/*Expand the vent array by one, add one to the vent count*/
					if((*Vents = (VentArr*) realloc(*Vents, (sizeof (VentArr) * ((++*ventct)+1))))
					    ==NULL) {
						printf("\n[INITIALIZE] Out of Memory adding vent to vent array!\n");
						return(-1);
					}
					INITVents = *Vents;
					
					/*Set new vent values to DBL_MAX, to indicate they are not yet full*/
					INITVents[*ventct-1].northing    = DBL_MAX;
					INITVents[*ventct-1].easting     = DBL_MAX;
					INITVents[*ventct-1].totalvolume = DBL_MAX;
					INITVents[*ventct-1].pulsevolume = DBL_MAX;
					
				}
				else { /*If last vent has not yet been filled*/
					/*Call Error, report correct syntax, return -1*/
					printf("\nError: [INITIALIZE] Previous vent missing data.");
					printf(" Configuration File Error.\n Syntax:\n");
					printf("  NEW VENT\n  VENT_NORTHING = ___\n  VENT_EASTING = ___\n");
					printf("  VENT_PULSE_VOLUME = ___\n  VENT_TOTAL_VOLUME = ___\n");
					return(-1);
				}
			}
		}
		else if (!strncmp(var, "VENT_PULSE_VOLUME", strlen("VENT_PULSE_VOLUME"))) {
			/*Check to see that vent count is not 0 and vent pulse volume is still 
			  unassigned, then assign vent pulse volume value to current vent array element*/
			if((*ventct > 0) && (INITVents[*ventct-1].pulsevolume == DBL_MAX)) {
				INITVents[*ventct-1].pulsevolume = strtod(value,&ptr);
				printf("[assigned]\n");
			} else { /*if vent pulse volume declared before the next NEW VENT declared*/
				printf("\nError: [INITIALIZE] Vent Pulse Volume declared before New Vent declared");
				printf(" in Configuration File.\n Correct Syntax:\n");
				printf("  NEW VENT\n  VENT_NORTHING = ___\n  VENT_EASTING = ___\n");
				printf("  VENT_PULSE_VOLUME = ___\n  VENT_TOTAL_VOLUME = ___\n");
				return(-1);
			}
		}
		else if (!strncmp(var, "VENT_TOTAL_VOLUME", strlen("VENT_TOTAL_VOLUME"))) {
			/*Check to see that vent count is not 0 and vent total volume is still 
			  unassigned, then assign vent total volume value to current vent array element*/
			if((*ventct > 0) && (INITVents[*ventct-1].totalvolume == DBL_MAX)) {
				INITVents[*ventct-1].totalvolume = strtod(value,&ptr);
				printf("[assigned]\n");
			} else { /*if vent total volume declared before the next NEW VENT declared*/
				printf("\nError: [INITIALIZE] Vent Total Volume declared before New Vent declared");
				printf(" in Configuration File.\n Correct Syntax:\n");
				printf("  NEW VENT\n  VENT_NORTHING = ___\n  VENT_EASTING = ___\n");
				printf("  VENT_PULSE_VOLUME = ___\n  VENT_TOTAL_VOLUME = ___\n");
				return(-1);
			}
		}
		else if (!strncmp(var, "VENT_EASTING", strlen("VENT_EASTING"))) {
			/*Check to see that vent count is not 0 and vent easting is still unassigned
			  then assign vent easting value to current vent array element*/
			if((*ventct > 0) && (INITVents[*ventct-1].easting == DBL_MAX)) {
				INITVents[*ventct-1].easting = strtod(value,&ptr);
				printf("[assigned]\n");
			} else { /*if vent easting declared before the next NEW VENT declared*/
				printf("\nError: [INITIALIZE] Vent Easting declared before New Vent declared");
				printf(" in Configuration File.\n Correct Syntax:\n");
				printf("  NEW VENT\n  VENT_NORTHING = ___\n  VENT_EASTING = ___\n");
				printf("  VENT_PULSE_VOLUME = ___\n  VENT_TOTAL_VOLUME = ___\n");
				return(-1);
			}
		}
		else if (!strncmp(var, "VENT_NORTHING", strlen("VENT_NORTHING"))) {
			/*Check to see that vent count is not 0 and vent northing is still unassigned
			  then assign vent northing value to current vent array element*/
			if((*ventct > 0) && (INITVents[*ventct-1].northing == DBL_MAX)) {
				INITVents[*ventct-1].northing = strtod(value,&ptr);
				printf("[assigned]\n");
			} else { /*if vent northing declared before the next NEW VENT declared*/
				printf("\nError: [INITIALIZE] Vent Northing declared before New Vent declared");
				printf(" in Configuration File.\n Correct Syntax:\n");
				printf("  NEW VENT\n  VENT_NORTHING = ___\n  VENT_EASTING = ___\n");
				printf("  VENT_PULSE_VOLUME = ___\n  VENT_TOTAL_VOLUME = ___\n");
				return(-1);
			}
		}
		
		else {
			printf("[not assigned]\n");
			continue;
		}
	}
	
	/*at end check for assigned last vent.*/
	if(*ventct == 0) { /*If no vents were found in configuration file*/
		printf("\nError: [INITIALIZE] No Vents Assigned in Configuration File!\n");
		return(-1);
	}
	else if(((INITVents[*ventct-1].northing    == DBL_MAX)  ||
	         (INITVents[*ventct-1].easting     == DBL_MAX)) ||
	        ((INITVents[*ventct-1].totalvolume == DBL_MAX)  ||
	         (INITVents[*ventct-1].pulsevolume == DBL_MAX))) {
	/*If vent is missing information*/
		printf("\nError: [INITIALIZE] Last Vent missing information:\n");
		printf(" Vent #%u\n",*ventct);
		if(INITVents[*ventct-1].northing != DBL_MAX)
		printf("  Northing     = %f\n",INITVents[*ventct-1].northing);
		else printf("  Northing     = BLANK\n");
		if(INITVents[*ventct-1].easting != DBL_MAX)
		printf("  Easting      = %f\n",INITVents[*ventct-1].easting);
		else printf("  Easting      = BLANK\n");
		if(INITVents[*ventct-1].totalvolume != DBL_MAX)
		printf("  Pulse Volume = %f\n",INITVents[*ventct-1].pulsevolume);
		else printf("  Pulse Volume = BLANK\n");
		if(INITVents[*ventct-1].pulsevolume != DBL_MAX)
		printf("  Total Volume = %f\n",INITVents[*ventct-1].totalvolume);
		else printf("  Total Volume = BLANK\n");
		
		return(-1);
	}
	
	(void) fclose(ConfigFile);
	return(0);
}
