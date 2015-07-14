#include "prototypes.h"

Cell **ACTIVELIST_INIT(unsigned workerCt, unsigned capacity){
/*Reserves memory for workerCt number of lists with size [capacity]
  With the typedef of Cell.
*/
	int i;
	Cell **m;
	
	/*Allocate row pointers*/
	if((m=(Cell**)malloc((workerCt+1)*sizeof(Cell*)))==NULL){
		printf("[ACTIVELIST_INIT]\n");
		printf("   NO MORE MEMORY: Tried to allocate memory for %u Lists!! Exiting\n",
		       (workerCt+1));
		exit(0);
	}
	
	/*allocate cols & set previously allocated row pointers to point to these*/
	/*row 0 isnt enumerated, and will not be used. start at 1.*/
	for(i=1;i<=workerCt;i++){
		if((m[i]=(Cell*)malloc((capacity + 1)*sizeof(Cell)))==NULL){
		printf("[ACTIVELIST_INIT]\n");
		printf("   NO MORE MEMORY: Tried to allocate memory for %u cells in %u Lists!!",
		       capacity,(workerCt+1));
		printf(" Exiting\n");
			exit(0);
		}
	}

	/*return pointer to the array*/	
	return m;
}

Location **GLOBALDATA_INIT(unsigned rows,unsigned cols){
/*Reserves memory for matrix of size [rows]x[cols]
  With the typedef of Location.
*/
	int i;
	Location **m;
	
	/*Allocate row pointers*/
	if((m=(Location**)malloc((unsigned)(rows + 1)*sizeof(Location*)))==NULL){
		printf("[GLOBALDATA_INIT]\n");
		printf("   NO MORE MEMORY: Tried to allocate memory for %u Rows!! Exiting\n",
		       rows);
		exit(0);
	}
	
	/*allocate cols & set previously allocated row pointers to point to these*/
	
	for(i=0;i<=rows;i++){
		if((m[i]=(Location*)malloc((cols + 1)*sizeof(Location)))==NULL){
		printf("[GLOBALDATA_INIT]\n");
		printf("   NO MORE MEMORY: Tried to allocate memory for %u cols in %u rows!!",
		       cols,rows);
		printf(" Exiting\n");
			exit(0);
		}
	}
	
	/*return pointer to the array of row pointers*/
	return m;
}
