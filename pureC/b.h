#include <stdio.h>

#define FREE 0
#define USED 1
#define END (-1)


/* content struct, should be expand if use */
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


/* clean the file with 0 for size bytes,
set free block table and info table
use for initialization */
int creatFile(FILE **fp, char *name, BlockTable *infoTable, long tableSize);

int openFIle(FILE **fp, char *name);

int getFreeTable(FILE *fp, BlockTable *table);

int getInfoTable(FILE *fp, BlockTable *table, long tableSize);

/* write the new block with updating the free table,
the alloced block always be the first of the free list.
should not be api, for it may leads to mistakes */
int expandFile(FILE *fp, long size, BlockTable *freeTable);

/* harmless to use by developer,
but should not be api, for it don't check input */
int getNextBlock(FILE *fp, Block *current);

int mallocBlock(FILE *fp, long size, long *position);

int addInfoBlock(FILE *fp, Block *add, long size);