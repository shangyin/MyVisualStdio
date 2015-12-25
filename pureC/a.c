#include <stdio.h>
#include "b.h"
#include <assert.h>

typedef struct tEmployeeTable
{
	char isUsed;
	long head;
	long tail;
	int num;
	char name[20];
}EmployeeTable;

typedef struct tGuy
{
	long size;
	long next;
	char name[10];
}Guy;

int main()
{
	FILE *fp;
	EmployeeTable infoTable = { 0,0,0,0,"test!" };
	BlockTable freeTable = { 0,0,0,0 };
	
	long position;

	creatFile(&fp, "workers", (BlockTable*)&infoTable, sizeof(EmployeeTable));
	//openFIle(&fp, "workers");
   

	/* show free block */
	getFreeTable(fp ,&freeTable);
	Guy sample1 = { 0,freeTable.head };
	printf("\n\nsite of head:%ld\n", freeTable.head);
	while (getNextBlock(fp, (Block*)&sample1) == 0)
	{
		printf("%ld\t%ld\n", sample1.size, sample1.next);
	}

	/* add a guy block*/
	Guy sample2 = { 0,0,"Mosby!" };
	addInfoBlock(fp, (Block*)&sample2, sizeof(Guy));
    addInfoBlock(fp, (Block*)&sample2, sizeof(Guy));
    addInfoBlock(fp, (Block*)&sample2, sizeof(Guy));
    addInfoBlock(fp, (Block*)&sample2, sizeof(Guy));

	/* show info block */
	getInfoTable(fp, (BlockTable*)&infoTable, sizeof(EmployeeTable));
	Guy sample3 = { 0,infoTable.head };
	printf("\n\nsite of head:%ld\n", infoTable.head);
	while (getNextBlock(fp, (Block*)&sample3) == 0)
	{
	    printf("%ld\t%ld\n", sample3.size, sample3.next);
	}

    
	fclose(fp);
	return 0;
}