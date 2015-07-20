#ifndef _LAVA_STRUCTS_
#define _LAVA_STRUCTS_

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <float.h>
#include <string.h>

/*Active Cells*/
typedef struct Automata {
	unsigned row;
	unsigned col;
	double elev;
	double thickness;
	double lava_in;
	double lava_out;
	char parents;
	char vent;
} Automata;

/*Global Data Locations*/
typedef struct DataCell {
	double elev;
	unsigned active;
	char AOICode;
	double elev_uncert;
	double residual;
	double random_code;
	double dem_elev;
} DataCell;

/*Vent Information*/
typedef struct VentArr {
	double northing;
	double easting;
	double totalvolume;
	double pulsevolume;
} VentArr;

/*Global Variables*/
time_t startTime;
time_t endTime;

/*      ,``                 .`,   
      `,.+,               ``+`    
   ,.`#++``               ,;++`,  
 , +++'`.                  ,++++ `
` +++++++;.``  ,   ,     `#+++++ `
``+++++++++++`,   ,'++++++++++'`` 
  .,`  `++++ .   .++++++;.  ,,    
     ,+++++.   ` +++++ ,          
     ,`+++;, ``'++++`,            
    ,;+++`, . +++++,              
  . ++++;;:::++++',               
   ,'++++++++++#`,                
    ,.....````,`                  
                                  
        GO BULLS                */

#endif /*#ifndef _LAVA_STRUCTS_*/
