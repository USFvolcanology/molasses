#include "structs.h"  /* Global Structures and Variables*/
#include "gdal.h"     /* GDAL */
#include "cpl_conv.h" /* GDAL for CPLMalloc() */

int PULSE(Cell*,VentArr**,unsigned,double*,unsigned,double*);
	/*args: 1st Worker's Active List,
	        Active Count,
	        Total Pulse Volume, 
	        Remaining Total Volume,
	        Vent Cell Count
	*/

int OUTPUT(Location**, Cell*, unsigned, char*, char, double*, const char*);
	/*args:
		Global Grid,
		Flow List,
		Flow List Cell Count,
		Output File Name,
		Output File Type,
		DEM transform metadata,
		DEM projection metadata
	*/

unsigned ACTIVATE(Location**,Cell*,unsigned,unsigned,unsigned,char,char);
	/*args:
		Global Grid,
		Active List,
		row,
		col,
		active count,
		parent code,
		vent code,
		residual
	*/

int INIT_FLOW (Location**,Cell***,VentArr*,unsigned*,unsigned*,unsigned,
               unsigned**,double*,double*);
	/*args:
		data array,
		*active list,
		vent array,
		*worker count,
		worker capacity,
		vent count,
		*active counts,
		DEM transform data,
		*total volume in,
		residual
	*/

Cell *NEIGHBOR_ID(Cell,Location**,double*,Cell*,int*);
	/*args:   Active Cell,
	          Global Grid,
	          Global Grid Metadata,
	          Active List,
	          neighbor count
	  return: neighbor_list
	*/

Cell **ACTIVELIST_INIT(unsigned,unsigned);
Location **GLOBALDATA_INIT(unsigned,unsigned);
	/*args: rows, colums
	*/

int DISTRIBUTE(Location**,Cell*,unsigned*,double*);
	/*args: Global Grid, Active List, Active Count, DEM metadata*/

int INITIALIZE(char*,char***,double*,double*,VentArr**,unsigned*);
	/*args:
		Configuration File Name,
		DEM File Name,
		Flow Output File Name,
		Modal Thickness,
		Vent Array,
		Vent Count
	*/

double *DEM_LOADER(char*, Location***,char*);
	/*args: DEM file name, Null Global Data Grid pointer, Model Code*/
	/*Model Codes:
	  ELEV
	  RESID
	  TOPOG
	  T_UNC
	*/
