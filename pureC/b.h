#include <stdio.h>

#define FREE 0
#define USED 1
#define END (-1)

typedef struct tBlock Block;

typedef struct tBlockTable BlockTable;

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

int creatFile(FILE **fp, char *name, BlockTable *infoTable, long tableSize);

int openFIle(FILE **fp, char *name);

int addInfoBlock(FILE *fp, Block *add);

int getExactBlock(FILE *fp, Block *target[], int(*comp)(void* des, void* src), void* src);

int DeleInfoBlock(FILE *fp, int(*compare)(void* dst, void* src), void* src);

int modifyBlock(FILE *fp, int(*compare)(void* dst, void* src), int(*change)(void* dst, void* src), void *src1, void *src2);
