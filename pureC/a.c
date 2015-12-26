#include <stdio.h>
#include "b.h"
#include <assert.h>
#include <string.h>

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

int compare(void* des, void* src)
{
	Guy *temp = (Guy*)des;
	if (strcmp(temp->name, (char*)src) == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int main()
{
	FILE *fp;
	EmployeeTable infoTable = { 0,0,0,0,"test!" };
	BlockTable freeTable = { 0,0,0,0 };

	creatFile(&fp, "workers", (BlockTable*)&infoTable, sizeof(EmployeeTable));
	//openFIle(&fp, "workers");

	/* add a guy block*/
	Guy sample2 = { 0,0,"Aosby!" };
	for (int i = 0; i < 26; i++)
	{
		addInfoBlock(fp, (Block*)&sample2, sizeof(Guy));
		sample2.name[0] += 1;
	}

	/* show info block */
	Guy sample3 = { 0,0 };
	while (getNextBlock(fp, (Block*)&sample3) == 0)
	{
	    printf("%ld\t%ld\t%s\n", sample3.size, sample3.next,sample3.name);
	}

	Guy res[100];
	getExactBlock(fp, (Block*)res, compare, "Xosby!");
	DeleInfoBlock(fp, (Block*)&res[0]);

	sample3.next = 0;
	while (getNextBlock(fp, (Block*)&sample3) == 0)
	{
		printf("%ld\t%ld\t%s\n", sample3.size, sample3.next, sample3.name);
	}

	fclose(fp);
	return 0;
}