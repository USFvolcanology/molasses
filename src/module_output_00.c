#include "prototypes.h"

int OUTPUT(DataCell **dataGrid, Automata *flowList, unsigned flowcount,
           char *outputfilename, char outputtype, double *geotransform,
           const char *projection) {
/*Module OUTPUT
	Prints grid cells locations inundated by lava and thicknesses to a tab-del
	file. For flexibility, more parameters are needed as arguments for this module
	in case of future needs to output additional information
	
	File types to output:
	0: X,Y,Thickness ascii flow list
	1: Hit Raster (1 = Hit, 0 = Not Hit)
	2: Thickness Raster
	3: Elevation Raster
	4: Elevation + Lava Raster (2+3)
	
*/
	/*GeoTransform Double List:
	
	  Transform Index    This Code                        GDAL
		geotransform[0]    lower left x                     top left x (SAME)
	  geotransform[1]    w-e pixel res                    SAME
	  geotransform[2]    number of cols                   0
	  geotransform[3]    lower left y                     top left y
	  geotransform[4]    number of lines                  0
	  geotransform[5]    n-s pixel res (positive value)   (negative value)
	
	*/
	
	FILE     *flowfile;
	unsigned i,j,k;
	
	GDALDatasetH hDstDS;
	double GDALGeoTransform[6];
	/*const char *pszFormat = "GTiff";*/
  GDALDriverH hDriver = GDALGetDriverByName("GTiff"); /*Try this*/
	GDALRasterBandH hBand;
	GByte *RasterDataB;
	double *RasterDataD;
	
	printf("\nFlow Output:\n");
	
	/*X,Y,Thickness ASCII Flow List**********************************************/
	if(outputtype==0) {
		/*Open flow file (x,y,thickness)*/
		flowfile  = fopen(outputfilename, "w");
		if (flowfile == NULL) {
			printf("Cannot open FLOW file=[%s]:[%s]! Exiting.\n",
			outputfilename, strerror(errno));
			return(-1);
		}
	
			for(j=1;j<=flowcount;j++) { /*for each cell in the current worker list*/
				/*Retrieve Active Cell values and print to flowfile*/
				/*x,y,z: easting, northing, thickness*/
				fprintf(flowfile, "%0.3f\t%0.3f\t%0.6f\n", 
					     (float) geotransform[0] + (geotransform[1] * flowList[j].col),
					     (float) geotransform[3] + (geotransform[5] * flowList[j].row),
					     (float) (flowList[j].thickness + 
					              dataGrid[flowList[j].row][flowList[j].col].residual));
			}
		fclose(flowfile);
		printf(" ASCII   Output file: %s successfully written.\n (x,y,thickness)\n",
		       outputfilename);
	}
	/*END ASCII DATATYPES********************************************************/
	
	/*RASTER DATATYPES***********************************************************/
	else {
		/*Modify Metadata back to GDAL format*/
		GDALGeoTransform[0] = geotransform[0];
		GDALGeoTransform[1] = geotransform[1];
		GDALGeoTransform[3] = geotransform[3] + (geotransform[5] * geotransform[4]);
		GDALGeoTransform[5] = -1 * geotransform[5];
		GDALGeoTransform[2] = GDALGeoTransform[4] = 0;
		
		/*BINARY HIT RASTER********************************************************/
		if(outputtype==1) {
			
			/*Create Data Block*/
			if((RasterDataB = malloc (sizeof (GByte) * geotransform[2] *
				                        geotransform[4]))==NULL) {
				printf("[OUTPUT] Out of Memory creating Outgoing Raster Data Array\n\n");
				return(-1);
			}
			
			/*Assign Model Data to Data Block*/
			k=0; /*Data Counter*/
			for(i=geotransform[4];i>0;i--) { /*For each row, TOP DOWN*/
				for(j=0;j<geotransform[2];j++) {         /*For each col, Left->Right*/
					if(dataGrid[i-1][j].active) RasterDataB[k++] = 1;
					else RasterDataB[k++] = 0;
				}
			}
			
			/*Setup Raster Dataset*/
			hDstDS = GDALCreate(hDriver, outputfilename, geotransform[2], 
			                    geotransform[4], 1, GDT_Byte, NULL);
			
			GDALSetGeoTransform( hDstDS, GDALGeoTransform ); /*Set Transform*/
			GDALSetProjection( hDstDS, projection );     /*Set Projection*/
			hBand = GDALGetRasterBand( hDstDS, 1 );      /*Set Band to 1*/
			
			/*Write the formatted raster data to a file*/
			GDALRasterIO(hBand, GF_Write, 0, 0, geotransform[2], geotransform[4],
					         RasterDataB, geotransform[2], geotransform[4], GDT_Byte, 
					         0, 0 );
			
			/*Properly close the raster dataset*/
			GDALClose( hDstDS );
			free(RasterDataB);
			
			printf(" %s Output file: %s successfully written.\n (hit count)\n",
			       GDALGetDriverLongName( hDriver ),outputfilename);
		}
		
		/*DOUBLE RASTER TYPES******************************************************/
		else if ((outputtype>=2)&&(outputtype<=4)) {
			
			/*Create Data Block*/
			if((RasterDataD = malloc (sizeof (double) * geotransform[2] *
					                      geotransform[4]))==NULL) {
				printf("[OUTPUT] Out of Memory creating Outgoing Raster Data Array\n");
				return(-1);
			}
			
			/*Assign Model Data to Data Block*/
			/*Thickness Raster**********************************/
			if(outputtype==2) {
				k=0; /*Data Counter*/
				for(i=geotransform[4];i>0;i--) { /*For each row, TOP DOWN*/
					for(j=0;j<geotransform[2];j++) {         /*For each col, Left->Right*/
						if(dataGrid[i-1][j].active) {
							RasterDataD[k++] = flowList[dataGrid[i-1][j].active].thickness + 
							                   dataGrid[i][j].residual;
						}
						else RasterDataD[k++] = 0.0;
					}
				}
			}
			/*Elevation Raster**********************************/
			else if(outputtype==3) {
				k=0; /*Data Counter*/
				for(i=geotransform[4];i>0;i--) { /*For each row, TOP DOWN*/
					for(j=0;j<geotransform[2];j++) {         /*For each col, Left->Right*/
						RasterDataD[k++] = dataGrid[i-1][j].elev;
					}
				}
			}
			/*Elevation + Lava Thickness Raster*****************/
			else if (outputtype==4) {
				k=0; /*Data Counter*/
				for(i=geotransform[4];i>0;i--) { /*For each row, TOP DOWN*/
					for(j=0;j<geotransform[2];j++) {         /*For each col, Left->Right*/
						if(dataGrid[i-1][j].active) {
							RasterDataD[k++] = flowList[dataGrid[i-1][j].active].elev;
						}
						else RasterDataD[k++] = dataGrid[i-1][j].elev;
					}
				}
			}
			
			
			
			/*Setup Raster Dataset*/
			hDstDS = GDALCreate(hDriver, outputfilename, geotransform[2], 
			                    geotransform[4], 1, GDT_Float64, NULL);
			
			GDALSetGeoTransform( hDstDS, GDALGeoTransform ); /*Set Transform*/
			GDALSetProjection( hDstDS, projection );     /*Set Projection*/
			hBand = GDALGetRasterBand( hDstDS, 1 );      /*Set Band to 1*/
			
			/*Write the formatted raster data to a file*/
			GDALRasterIO(hBand, GF_Write, 0, 0, geotransform[2], geotransform[4],
					         RasterDataD, geotransform[2], geotransform[4], GDT_Float64, 
					         0, 0 );
			
			/*Properly close the raster dataset*/
			GDALClose( hDstDS );
			free(RasterDataD);
			
			printf(" %s Output file: %s successfully written.\n",
			       GDALGetDriverLongName( hDriver ),outputfilename);
		}
		else{
			printf("ERROR: [OUTPUT] Invalid file type code used!\n");
			return(-1);
		}
	
	}
	/*END RASTER DATATYPES*******************************************************/
	
	return(0);
}
