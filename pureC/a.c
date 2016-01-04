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
	char tableName[50];
}EmployeeTable;

typedef struct tGuy
{
	long size;
	long next;
	char id[20];
	char name[20];
	int age;
	char sex[10];
}Guy;

int compare(void* des, void* src)
{
	Guy *temp = (Guy*)des;
	if (temp->age > *(int*)src)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int compareName(void* des, void* src)
{
	Guy *temp = (Guy*)des;
	if (strcmp(temp->name,src) == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int modifyOperate(void* dst, void* src)
{
	memcpy(((Guy*)dst)->name, src, 10);
	return 0;
}

int main()
{
	FILE *fp;
	EmployeeTable infoTable = { 0,0,0,0,"infomation for employee" };
	creatFile(&fp, "workers", (BlockTable*)&infoTable, sizeof(EmployeeTable));

	/* add a guy block*/
	Guy sample2 = { sizeof(Guy),0, "15089277891","Mosby!",5,"girl" };
	for (int i = 0; i < 10; i++)
	{
		addInfoBlock(fp, (Block*)&sample2);
		sample2.id[2] += 1;
		sample2.age++;
	}

	Guy *res[100];

	int age = 10;
	int index = getExactBlock(fp, (Block**)res, compare, &age);
	for (int i = 0; i < index; i++)
	{
		printf("%s\t%s\t%d\t%s\n", res[i]->id, res[i]->name, res[i]->age, res[i]->sex);
	}

	fclose(fp);
	return 0;
}