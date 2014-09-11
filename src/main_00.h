#ifndef _LAVA_STRUCTS_
#define _LAVA_STRUCTS_

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <float.h>
#include <string.h>

typedef struct Cell {
	unsigned row;
	unsigned col;
	double elev;
	double thickness;
	char parents;
	char vent;
} Cell;

typedef struct Location {
	double elev;
	unsigned owner;
	unsigned active;
	char AOICode;
} Location;

typedef struct VentArr {
	double northing;
	double easting;
	double totalvolume;
	double pulsevolume;
} VentArr;


/*Global Module Variables*/
int      (*module_initialize) (char*,char**,char**,double*,VentArr**,unsigned*);
double*  (*module_dem_loader) (char*, Location***);
int      (*module_init_flow)  (Location**,Cell***,VentArr*,unsigned*,unsigned*,
                               unsigned,unsigned**,double*,double*,double);
int      (*module_pulse)      (Cell*,VentArr**,unsigned,double*,unsigned,double*);
Cell*    (*module_distribute) (Location**,Cell*,unsigned*,unsigned,unsigned,double);
unsigned (*module_activation) (Location**,Cell*,unsigned,unsigned,unsigned,char,char,double);
Cell*    (*module_neighbor_id)(Cell,Location**,Cell*,int*);
int      (*module_output)     (Location**,Cell**,unsigned,unsigned*,char*,double*,double);

/*Global Variables*/
unsigned dem_row_max; /*maximum dimension of DEM*/
unsigned dem_col_max;
double   dem_row_size;/*height/width of row,columns*/
double   dem_col_size;
double   dem_ll_y;    /*lower left coordinates*/
double   dem_ll_x;
time_t starttime;
time_t endtime;

#endif /*#ifndef _LAVA_STRUCTS_*/
