#include "prototypes.h"

Automata **ACTIVELIST_INIT(unsigned CAListCount, unsigned CAListSize){
/*Reserves memory for CAListCount number of lists with size [CAListSize]
  With the typedef of Automata.
*/
	int i;
	Automata **m;
	
	/*Allocate row pointers*/
	if((m=(Automata**)malloc((CAListCount+1)*sizeof(Automata*)))==NULL){
		printf("[ACTIVELIST_INIT]\n");
		printf("   NO MORE MEMORY: Tried to allocate memory for %u Lists!! Exiting\n",
		       (CAListCount+1));
		exit(0);
	}
	
	/*allocate cols & set previously allocated row pointers to point to these*/
	/*row 0 isnt enumerated, and will not be used. start at 1.*/
	for(i=1;i<=CAListCount;i++){
		if((m[i]=(Automata*)malloc((CAListSize + 1)*sizeof(Automata)))==NULL){
		printf("[ACTIVELIST_INIT]\n");
		printf("   NO MORE MEMORY: Tried to allocate memory for %u cells in %u Lists!!",
		       CAListSize,(CAListCount+1));
		printf(" Exiting\n");
			exit(0);
		}
	}

	/*return pointer to the array*/	
	return m;
}

DataCell **GLOBALDATA_INIT(unsigned rows,unsigned cols){
/*Reserves memory for matrix of size [rows]x[cols]
  With the typedef of DataCell.
*/
	int i;
	DataCell **m;
	
	/*Allocate row pointers*/
	if((m=(DataCell**)malloc((unsigned)(rows + 1)*sizeof(DataCell*)))==NULL){
		printf("[GLOBALDATA_INIT]\n");
		printf("   NO MORE MEMORY: Tried to allocate memory for %u Rows!! Exiting\n",
		       rows);
		exit(0);
	}
	
	/*allocate cols & set previously allocated row pointers to point to these*/
	
	for(i=0;i<=rows;i++){
		if((m[i]=(DataCell*)malloc((cols + 1)*sizeof(DataCell)))==NULL){
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
