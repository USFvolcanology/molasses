#include "prototypes.h"

int INITIALIZE(char *CFGfilename, char ***Filenames, double *param_modalthickness,
               double *param_elevunc, VentArr **Vents, unsigned *ventct) {
/*Module INITIALIZE
	Accepts a configuration file and returns model variables:
	
	string DEM_FILE
	double ELEVATION_UNCERT (optional)
	double MODAL_THICKNESS
	
	string ASCII_THICKNESS  (optional, at least 1)
	string RASTER_HITMAP    (optional, at least 1)
	string RASTER_THICKNESS (optional, at least 1)
	string RASTER_ELEVATION (optional, at least 1)
	string RASTER_NEW_ELEV  (optional, at least 1)
	
	double PULSE_VOLUME
	double TOTAL_VOLUME
	double VENT_EASTING
	double VENT_NORTHING
	
	Checks at end for configuration file errors, where mandatory parameters were
	  not assigned.
*/

	unsigned maxLineLength = 256;
	char     line[256];             /*Line string from file       */
	char     var[64];               /*Parameter Name  in each line*/
	char     value[256];            /*Parameter Value in each line*/
	int      i;
	char     *ptr;
	VentArr  *INITVents=NULL;  /*working array for vents in this module         */
	char     **INITFiles;      /*working array for filenames in this module     */
	unsigned INITFilesCount = 7;   /*number of possible input files             */
	FILE     *Opener;     /*Dummy File variable to test valid output file paths */
	unsigned outputs = 0; /*Number of output types called for (must have >=1)   */
	
	FILE *ConfigFile;
	
	
	/*Allocate filename pointers*/
	if((*Filenames=(char**)malloc((size_t)(INITFilesCount)*sizeof(char*)))==NULL){
		printf("ERROR [INITIALIZE]:\n");
		printf("   NO MORE MEMORY: Tried to allocate memory for 7 filenames\n");
		return(-1);
	}
	INITFiles = *Filenames;
	for(i=0;i<(INITFilesCount);i++){
		/*Allocate memory for file names*/
		if((INITFiles[i]=(char*)malloc(sizeof(char)*(maxLineLength)))==NULL) {
			printf("\n[INITIALIZE] Out of Memory assigning filenames!\n");
			return(-1);
		}
		/*Set initial character in each filename to EOS character*/
		INITFiles[i][0] = '\0';
	}
	
	printf("Reading in Parameters...\n");
	
	/*open configuration file*/
	ConfigFile = fopen(CFGfilename, "r");
	if (ConfigFile == NULL) {
		printf("\nERROR [INITIALIZE]: Cannot open configuration file=[%s]:[%s]!\n",
		       CFGfilename,
		       strerror(errno));
		return(-1);
	}

	/* use each line to compare to needed values*/
	while (fgets(line, maxLineLength, ConfigFile) != NULL) {
		/*if first character is comment, new line, space, return to next line*/
		if (line[0] == '#' || line[0] == '\n' || line[0] == ' ') continue;
		
		/*print incoming parameter*/
		sscanf (line,"%s = %s",var,value); /*split line into before ' = ' and after*/
		if (strncmp(var, "NEW_VENT", strlen("NEW_VENT"))) /*dont print if line=newvent*/
			printf("%20s = %-33s ",var,value); /*print incoming parameter value*/
		else printf("%20s (#%u)\n","NEW VENT",(*ventct+1)); /*print new vent flag*/
		
		/*INPUT FILES AND GLOBAL MODEL PARAMETERS**********************************/
		/*INPUT DEM FILE*/
		if (!strncmp(var, "DEM_FILE", strlen("DEM_FILE"))) {
			strncpy(INITFiles[0],value,strlen(value)+1);
			printf("[assigned]\n");
		}
		
		/*MODAL THICKNESS*/
		else if (!strncmp(var, "MODAL_THICKNESS", strlen("MODAL_THICKNESS"))) {
			*param_modalthickness = strtod(value,&ptr);
			if (ptr == value) { /*NOT A NUMBER*/
				strncpy(INITFiles[1],value,strlen(value)+1);
				*param_modalthickness = -1; /*-1 is flag for file*/
				printf("[as.-file]\n");
			}
			else printf("[assigned]\n");
		}
		
		/*ELEVATION UNCERTAINTY*/
		else if (!strncmp(var, "ELEVATION_UNCERT", strlen("ELEVATION_UNCERT"))) {
			*param_elevunc = strtod(value,&ptr);
			if (ptr == value) { /*NOT A NUMBER*/
				strncpy(INITFiles[2],value,strlen(value)+1);
				*param_elevunc = -1; /*-1 is flag for file*/
				printf("[as.-file]\n");
			}
			else printf("[assigned]\n");
		}
		
		/*OUTPUT FILES*************************************************************/
		/*OUTPUT ASCII X,Y,THICKNESS FILE*/
		else if (!strncmp(var, "OUTFILE_A_THICKNESS", strlen("OUTFILE_A_THICKNESS"))) {
			strncpy(INITFiles[3],value,strlen(value)+1);
			
			Opener = fopen(INITFiles[3], "w");
			if (Opener == NULL) {
				printf("\nERROR [INITIALIZE]: Failed to create an output file at [%s]:[%s]!\n",
				       INITFiles[3],
				       strerror(errno));
				return(-1);
			}
			else {
				(void) fclose(Opener);
				outputs++;
				printf("[assigned]\n");
			}
		}
		
		/*OUTPUT HIT MAP FILE*/
		else if (!strncmp(var, "OUTFILE_R_HITMAP", strlen("OUTFILE_R_HITMAP"))) {
			strncpy(INITFiles[4],value,strlen(value)+1);
			
			Opener = fopen(INITFiles[4], "w");
			if (Opener == NULL) {
				printf("\nERROR [INITIALIZE]: Failed to create an output file at [%s]:[%s]!\n",
				       INITFiles[4],
				       strerror(errno));
				return(-1);
			}
			else {
				(void) fclose(Opener);
				outputs++;
				printf("[assigned]\n");
			}
		}
		
		/*OUTPUT RASTER THICKNESS FILE*/
		else if (!strncmp(var, "OUTFILE_R_THICKNESS", strlen("OUTFILE_R_THICKNESS"))) {
			strncpy(INITFiles[5],value,strlen(value)+1);
			
			Opener = fopen(INITFiles[5], "w");
			if (Opener == NULL) {
				printf("\nERROR [INITIALIZE]: Failed to create an output file at [%s]:[%s]!\n",
				       INITFiles[5],
				       strerror(errno));
				return(-1);
			}
			else {
				(void) fclose(Opener);
				outputs++;
				printf("[assigned]\n");
			}
		}
		
		/*OUTPUT RASTER THICKNESS FILE*/
		else if (!strncmp(var, "OUTFILE_R_ELEVATION", strlen("OUTFILE_R_ELEVATION"))) {
			strncpy(INITFiles[6],value,strlen(value)+1);
			
			Opener = fopen(INITFiles[6], "w");
			if (Opener == NULL) {
				printf("\nERROR [INITIALIZE]: Failed to create an output file at [%s]:[%s]!\n",
				       INITFiles[6],
				       strerror(errno));
				return(-1);
			}
			else {
				(void) fclose(Opener);
				outputs++;
				printf("[assigned]\n");
			}
		}
		
		/*OUTPUT RASTER THICKNESS FILE*/
		else if (!strncmp(var, "OUTFILE_R_NEW_ELEV", strlen("OUTFILE_R_NEW_ELEV"))) {
			strncpy(INITFiles[7],value,strlen(value)+1);
			
			Opener = fopen(INITFiles[7], "w");
			if (Opener == NULL) {
				printf("\nERROR [INITIALIZE]: Failed to create an output file at [%s]:[%s]!\n",
				       INITFiles[7],
				       strerror(errno));
				return(-1);
			}
			else {
				(void) fclose(Opener);
				outputs++;
				printf("[assigned]\n");
			}
		}
		
		/*FLOW PARAMETERS**********************************************************/
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
					printf("\nERROR [INITIALIZE]: Previous vent missing data.");
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
				if (ptr == value) { /*NOT A NUMBER*/
					printf("\nERROR [INITIALIZE]: Vent Pulse Volume is not a number!\n");
					return(-1);
				}
				printf("[assigned]\n");
			} else { /*if vent pulse volume declared before the next NEW VENT declared*/
				printf("\nERROR [INITIALIZE]: Vent Pulse Volume declared before New Vent declared");
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
				if (ptr == value) { /*NOT A NUMBER*/
					printf("\nERROR [INITIALIZE]: Vent Total Volume is not a number!\n");
					return(-1);
				}
				printf("[assigned]\n");
			} else { /*if vent total volume declared before the next NEW VENT declared*/
				printf("\nERROR [INITIALIZE]: Vent Total Volume declared before New Vent declared");
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
				if (ptr == value) { /*NOT A NUMBER*/
					printf("\nERROR [INITIALIZE]: Vent Easting is not a number!\n");
					return(-1);
				}
				printf("[assigned]\n");
			} else { /*if vent easting declared before the next NEW VENT declared*/
				printf("\nERROR [INITIALIZE]: Vent Easting declared before New Vent declared");
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
				if (ptr == value) { /*NOT A NUMBER*/
					printf("\nERROR [INITIALIZE]: Vent Northing is not a number!\n");
					return(-1);
				}
				printf("[assigned]\n");
			} else { /*if vent northing declared before the next NEW VENT declared*/
				printf("\nERROR [INITIALIZE]: Vent Northing declared before New Vent declared");
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
		printf("\nERROR [INITIALIZE]: No Vents Assigned in Configuration File!\n");
		return(-1);
	}
	else if(((INITVents[*ventct-1].northing    == DBL_MAX)  ||
	         (INITVents[*ventct-1].easting     == DBL_MAX)) ||
	        ((INITVents[*ventct-1].totalvolume == DBL_MAX)  ||
	         (INITVents[*ventct-1].pulsevolume == DBL_MAX))) {
	/*If vent is missing information*/
		printf("\nERROR [INITIALIZE]: Last Vent missing information:\n");
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
	
	/*Check for missing parameters*/
	if(!*param_elevunc) { /*Elevation uncertainty is either missing or is 0.*/
		*param_elevunc = 0;
		printf("ELEVATION UNCERTAINTY = 0: DEM values assumed to be true.\n");
	}
	if(!*param_modalthickness) { /*Modal thickness is missing.*/
		printf("\nERROR [INITIALIZE]: No Residual Thickness Given!!\n");
		return(-1);
	}
	if(!strcmp(INITFiles[0],"")) { /*DEM Filename is missing.*/
		printf("\nERROR [INITIALIZE]: No DEM Filename Given!!\n");
		return(-1);
	}
	if(!outputs) { /*No Output Filenames are given.*/
		printf("\nERROR [INITIALIZE]: No Output Filenames Given!!\n");
		return(-1);
	}
	
	
	(void) fclose(ConfigFile);
	return(0);
}
