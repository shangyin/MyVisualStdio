#include <stdio.h>

#define FREE 0
#define USED 1
#define END (-1)

typedef struct tBlock
{
	long size;
	long next;
} Block;

/* head of BlockTable */
typedef struct tBlockTable
{
	char isUsed;
	long head;
	long tail;
	int num;
} BlockTable;

/*
 *	the structs should follow types like these:
 *
 *	struct tBlock
 *	{
 *		long size;
 *		long next;
 *      ** add type **
 *	};
 *	struct tBlockTable
 *	{
 *		char isUsed;
 *		long head;
 *		long tail;
 *		int num;
 *      ** add type **
 *	};
 */

int getFreeTable(FILE *fp, BlockTable *table);

int getInfoTable(FILE *fp, BlockTable *table, long size);

int setFreeTable(FILE *fp, BlockTable *table);

int setInfoTable(FILE *fp, BlockTable *table, long size);

int getNextBlock(FILE *fp, Block *current);

int getBlock(FILE *fp, long position, Block *get);

int setBlock(FILE *fp, long position, Block *set);

int expandFile(FILE *fp, long size, long *position);

int mallocBlock(FILE *fp, long size, long *position);

int creatFile(FILE **fp, char *name, BlockTable *infoTable, long tableSize);

int openFIle(FILE **fp, char *name);

int getExactBlock(FILE *fp, Block *target, int(*comp)(void* des, void* src), void* src);

int addInfoBlock(FILE *fp, Block *add, long size);

int DeleInfoBlock(FILE *fp, Block *dele);


