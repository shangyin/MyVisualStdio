/*
 *	XuTao Xu, 2015, 12, 19
 */

/*
 *	change log:
 *	12.21	: finish mallocBlock(3), expandFile(3), getNextBlock(2)
 *	12.22	: finish openFile(2), modify creatFile(6)	
 *  12.23	: modify expandFile(2), finish addInfoBlock(3), need to debug
 *	12.24	: modify expandFile(3), finish get/set Table/Block, debug...
 *  12.25   : sync to github, finish debug of addInfoBlock(3)
 *	12.26	: modify getNextBlock(2)
 *
 *	mallocBlock has 3 places to test
 */

/*
 *	Only two BlockTables exist in the file.
 *	The first BlockTable should be the free block table,
 *	which save the messages for free block link,
 *	and the next few bytes are the info table based on BlockTable,
 *	but you can expand something it when developing.
 */

/*
 *	Call fseek(3) before running fread_s and fwrite_s to the same file
 */

/*
 *	When return >= 0, the function has run well.
 *	Return <0 means it occurs problems that need to be handled,
 *	and the error messages will be print to stderr by function panic(1).
 *
 *	-6 unable to open file
 *	-5 useless operation
 *	-4 fail to call this library's funciton
 *	-3 wrong file
 *	-2 unable to write
 *	-1 haven't initilized the FILE pointer
 *	 0 everything goes well
 *	 1 can't find the object
 *	 2 no space for malloc, system will expand it.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "b.h"

#define FREE 0
#define USED 1
#define END (-1)

void panic(char *str)
{
	fprintf_s(stderr, str);
}


/* Should not be api, just for the library itself */
int getFreeTable(FILE *fp, BlockTable *table)
{
	fseek(fp, 0, SEEK_SET);
	fread_s(table, sizeof(BlockTable), sizeof(BlockTable), 1, fp);
	if (feof(fp) != 0 || table->isUsed != USED)
	{
		panic("not the right file\n");
		exit(0);
	}
	return 0;
}

int getInfoTable(FILE *fp, BlockTable *table, long size)
{
	fseek(fp, sizeof(BlockTable), SEEK_SET);
	fread_s(table, size, size, 1, fp);
	if (feof(fp) != 0 || table->isUsed != USED)
	{
		panic("not the right file\n");
		exit(0);
	}
	return 0;
}

int setFreeTable(FILE *fp, BlockTable *table)
{
	fseek(fp, 0, SEEK_SET);
	if (fwrite(table, sizeof(BlockTable), 1, fp) != 1)
	{
		panic("unable to write\n");
		exit(0);
	}
	return 0;
}

int setInfoTable(FILE *fp, BlockTable *table, long size)
{
	fseek(fp, sizeof(BlockTable), SEEK_SET);
	if (fwrite(table, size, 1, fp) != 1)
	{
		panic("uanble to write\n");
		exit(0);
	}
	return 0;
}

/* if current->next is 0, get the first info block */
int getNextBlock(FILE *fp, Block *current)
{
	if (current->next == END)
	{
		panic("end of the file\n");
		return 1;
	}

	/* assume freeTable must exist */
	if (current->next == 0)
	{
		BlockTable infoTable;
		getInfoTable(fp, &infoTable, sizeof(BlockTable));
		if (infoTable.num > 0)
		{
			getBlock(fp, infoTable.head, current);
			return 0;
		}
		else
		{
			panic("end of the file\n");
			return 1;
		}
	}
	else
	{
		getBlock(fp, current->next, current);
		return 0;
	}
	
}

int getBlock(FILE *fp, long position, Block *get)
{
	fseek(fp, position, SEEK_SET);
	fread_s(get, sizeof(Block), sizeof(Block), 1, fp);

	if (feof(fp) != 0)
	{
		panic("get EOF\n");
		exit(0);
	}
	else
	{
		return 0;
	}
}

int setBlock(FILE *fp, long position, Block *set)
{
	fseek(fp, position, SEEK_SET);
	if (fwrite(set, sizeof(Block), 1, fp) != 1)
	{
		panic("unable to write\n");
		exit(0);
	}
	return 0;
}

/* if current->next is 0, get the first info block */
int getNextFullBlock(FILE *fp, Block *current)
{
	if (current->next == END)
	{
		panic("end of the file\n");
		return 1;
	}

	/* assume freeTable must exist */
	if (current->next == 0)
	{
		BlockTable infoTable;
		getInfoTable(fp, &infoTable, sizeof(BlockTable));
		if (infoTable.num > 0)
		{
			getFullBlock(fp, infoTable.head, current);
			return 0;
		}
		else
		{
			panic("end of the file\n");
			return 1;
		}
	}
	else
	{
		getFullBlock(fp, current->next, current);
		return 0;
	}
	
}

int getFullBlock(FILE *fp, long position, Block *get)
{
	fseek(fp, position, SEEK_SET);
	fread_s(get, sizeof(Block), sizeof(Block), 1, fp);
	fseek(fp, position, SEEK_SET);
	fread_s(get, get->size, get->size, 1, fp);

	if (feof(fp) != 0)
	{
		panic("get EOF\n");
		exit(0);
	}
	else
	{
		return 0;
	}
}

int setFullBlock(FILE *fp, long position, Block *set)
{
	fseek(fp, position, SEEK_SET);
	if (fwrite(set, set->size, 1, fp) != 1)
	{
		panic("unable to write\n");
		exit(0);
	}
	return 0;
}

/* it will updata the free table,
the alloced block always be the first of the free list. */
int expandFile(FILE *fp, long size, long *position)
{
	
	fseek(fp, 0, SEEK_END);
	*position = ftell(fp);

	/* write the block to file */
	char zero = 0;
	for (int i = 0; i < size; i++)
	{
		if (fwrite(&zero, sizeof(char), 1, fp) == 0)
		{
			panic("unable to write\n");
			exit(0);
		}
	}
	/* set new block'size */
	Block temp = { size, 0 };
	setBlock(fp, *position, &temp);
	return 0;
}

/* need to be tested
if size < sizeof(Block), set it as sizeof(Block),
this means it can malloc a sizeof(Block) block, althought it's meanningless*/
int mallocBlock(FILE *fp, long size, long *position)
{
	if (fp == NULL)
	{
		panic("null pointer\n");
		return -1;
	}

	BlockTable freeTable;
	getFreeTable(fp, &freeTable);


	if (freeTable.isUsed == FREE)
	{
		panic("not the right file\n");
		exit(0);
	}

	if (size < sizeof(Block))
	{
		size = sizeof(Block);
		panic("block has at least 8 bytes\n");
	}

	/*** all messages are OK, start to find suitable block ***/

	/* not tested */
	/* no free block */
	if (freeTable.num == 0)
	{
		expandFile(fp, size, position);
		return 0;
	}

	/* have free blocks, find it */
	Block temp = { 0,0 };
	long preSite;
	fseek(fp, freeTable.head, SEEK_SET);
	while ((preSite = ftell(fp), getNextBlock(fp, &temp) == 0) && temp.size < size);

	/* tested */
	/* fail, no suitable block */
	if (temp.size < size)
	{
		expandFile(fp, size, position);
		return 0;
	}


	if (temp.size - size <= sizeof(Block))
	{
		/* get suitable block, don't require to divide that block */

		if (freeTable.head != preSite)
		{
			/* not the first block, preSite is the end Site of preivous block */
			/* not tested */
			*position = ftell(fp) - sizeof(Block);

			Block current = { size, 0 };
			setBlock(fp, *position, &current);

			Block pre;
			getBlock(fp, preSite - sizeof(Block), &pre);
			pre.next = temp.next;
			setBlock(fp, preSite - sizeof(Block), &pre);
		}
		else
		{
			/* it's the first block */
			/* tested */
			*position = freeTable.head;
			
			Block current = { size, 0 };
			setBlock(fp, *position, &current);
		}

		/* updata free table, public for not divided */
		freeTable.num -= 1;
		freeTable.head = temp.next;
		if (freeTable.head == END)
		{
			freeTable.tail = END;
		}
		setFreeTable(fp, &freeTable);

		return 0;
	}
	else
	{
		/* get suitable block, need to divide that block */

		if (freeTable.head != preSite)
		{
			/* not the first block, divide it */
			/* not tested */
			*position = ftell(fp) - sizeof(Block);

			/* modify current block's size */
			Block current = { size, 0 };
			setBlock(fp, *position, &current);

			/* modify previous block's next */
			Block pre;
			long preNext = ftell(fp) - sizeof(Block) + size;
			getBlock(fp, preSite - sizeof(Block), &pre);
			pre.next = preNext;
			setBlock(fp, preSite - sizeof(Block), &pre);

			/* modify next block */
			Block next = { temp.size - size, temp.next };
			setBlock(fp, preNext, &next);
			/* don't require to upadta free table */
		}
		else
		{
			/* it's the first block, divide it */
			/* tested */
			*position = preSite;

			/* modify current block's size */
			Block current = { size, preSite + size };
			setBlock(fp, preSite, &current);

			/* modify freeTable's head */
			freeTable.head = preSite + size;
			setFreeTable(fp, &freeTable);

			/* modify next block */
			Block next = { temp.size - size, temp.next };
			setBlock(fp, preSite + size, &next);
			/* done here */
		}

		return 0;
	}
}


/**************************************************************************************************************************************/
/*****************************************                          api                      ******************************************/
/**************************************************************************************************************************************/

/* creat a new file,
init, free block table and info table.*/
int creatFile(FILE **fp, char *name, BlockTable *infoTable, long tableSize)
{
	if (fopen_s(fp, name, "wb+") != 0)
	{
		panic("unable to open\n");
		return -6;
	}
	if (infoTable == NULL || name == NULL || tableSize < sizeof(BlockTable))
	{
		panic("null pointer\n");
		return -1;
	}

	/* set free table */
	BlockTable freeTable;
	freeTable.head = END;
	freeTable.num = 0;
	freeTable.isUsed = USED;
	freeTable.tail = END;
	fseek(*fp, 0, SEEK_SET);
	setFreeTable(*fp, &freeTable);

	/* set info table */
	infoTable->isUsed = USED;
	infoTable->num = 0;
	infoTable->tail = END;
	infoTable->head = END;
	setInfoTable(*fp, infoTable, tableSize);

	return 0;
}

/* open a file rather than creat */
int openFIle(FILE **fp, char *name)
{
	if (name == NULL || fp == NULL)
	{
		panic("null pointer\n");
		return -1;
	}
	if (fopen_s(fp, name, "rb+") != 0)
	{
		panic("unable to open file\n");
		return -6;
	}
	return 0;
}

/* use buffer, 4096, return the number of targets */
int getExactBlock(FILE *fp, Block *target, int(*comp)(void* des, void* src), void* src)
{
	if (fp == NULL || target == NULL || comp == NULL)
	{
		panic("null pointer\n");
		return -1;
	}

	/* set buffer */
	Block *temp = (Block*)malloc(4096);
	if (temp == NULL)
	{
		panic("null pointer\n");
		return -1;
	}
	memset(temp, 0, sizeof(Block));

	/* find target */
	int index = 0;
	while ((getNextFullBlock(fp, temp)) == 0)
	{
		if (comp(temp, src) == 0)
		{
			memcpy((char*)target + index*(temp->size), temp, temp->size);
			index++;
		}
	}
	free(temp);

	if (index == 0)
	{
		panic("find nothing\n");
		return 0;
	}
	else
	{
		return index;
	}
}

/* api, assume the site of add block is correct */
int addInfoBlock(FILE *fp, Block *add)
{
	if (fp == NULL || add == NULL)
	{
		panic("null pointer");
		return -1;
	}

	/* a sizeof(Block) block is useless for user */
	if (add->size <= sizeof(Block))
	{
		panic("useless\n");
		return -5;
	}

	long position;
	mallocBlock(fp, add->size, &position);

	BlockTable infoTable;
	getInfoTable(fp, &infoTable, sizeof(BlockTable));
	add->next = infoTable.head;
	infoTable.head = position;
	infoTable.num += 1;
	if (infoTable.tail == END)
	{
		infoTable.tail = position;
	}
	setInfoTable(fp, &infoTable, sizeof(BlockTable));
	setFullBlock(fp, position, add);

	return 0;
}

/* dele info block by key(void* src) */
int DeleInfoBlock(FILE *fp, int (*compare)(void* dst, void* src), void* src)
{
	if (fp == NULL)
	{
		panic("null pointer\n");
		return -1;
	}

	long preSite;
	long curSite;
	BlockTable infoTable;
	getInfoTable(fp, &infoTable, sizeof(BlockTable));

	/* set buffer*/
	Block *pre = (Block*)malloc(4906);
	Block *temp = (Block*)malloc(4096);
	if (pre == NULL || temp == NULL)
	{
		panic("null pointer\n");
		return -1;
	}
	memset(temp, 0, sizeof(Block));
	
	/* find target */
	fseek(fp, 0, SEEK_SET);
	while (preSite = ftell(fp) - temp->size, getNextFullBlock(fp, temp) == 0)
	{
		if (compare((void*)temp , src) == 0)	
		{
			if (preSite == 0)
			{
				/* the target is first block, preSite is 0 */
				curSite = infoTable.head;
				infoTable.head = temp->next;
			}
			else
			{
				/* the target is not the first, modify pre block, preSite is the site of pre block */
				curSite = ftell(fp) - temp->size;
				getBlock(fp, preSite, pre);
				pre->next = temp->next;
				setBlock(fp, preSite, pre);
			}
			/* updata info table */
			infoTable.num -= 1;
			setInfoTable(fp, &infoTable, sizeof(BlockTable));

			/* add to free table */
			BlockTable freeTable;
			getFreeTable(fp, &freeTable);
			temp->next = freeTable.head;
			freeTable.head = curSite;
			freeTable.num += 1;
			setBlock(fp, freeTable.head, temp);
			setFreeTable(fp, &freeTable);
			temp->next = pre->next;			/* make the loop continue */
		}
	}

	free(temp);
	free(pre);
	return 0;
}

/* find the block by key(void* src1), modify that block to void* src2 */
int modifyBlock(FILE *fp, int (*compare)(void* dst, void* src), int (*change)(void* dst, void* src), void *src1, void *src2)
{
	if (fp == NULL)
	{
		panic("null pointer\n");
		return -1;
	}

	/* set buffer*/
	Block *current = (Block*)malloc(4096);
	if (current == NULL)
	{
		panic("null pointer\n");
		return -1;
	}
	memset(current, 0, sizeof(Block));

	/* find target */
	while (getNextFullBlock(fp, current) == 0)
	{
		if (compare((void*)current, src1) == 0)
		{
			change((void*)current, src2);
			setFullBlock(fp, ftell(fp) - current->size, current);
		}
	}

	free(current);
	return 0;
}