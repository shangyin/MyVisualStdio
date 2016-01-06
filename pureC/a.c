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

int cmpByName(void* des, void* src)
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

Guy inputGuy()
{
	Guy arg;
	arg.size = sizeof(Guy);
	printf("id:");
	fgets(arg.id, 20, stdin);
	printf("name:");
	fgets(arg.name, 20, stdin);
	printf("sex:");
	scanf_s("%d", &(arg.age));
	printf("id:");
	fgets(arg.sex, 10, stdin);
	return arg;
}

int addGuy(FILE *fp)
{
	Guy temp = inputGuy();
	return addInfoBlock(fp, (Block*)&temp);
}




int main()
{
	FILE *fp;
	EmployeeTable infoTable = { 0,0,0,0,"infomation for employee" };
	creatFile(&fp, "workers", (BlockTable*)&infoTable, sizeof(EmployeeTable));




	fclose(fp);
	return 0;
}