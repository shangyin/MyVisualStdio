/*
 *	XuTao Xu, 2015, 12, 19
 */

/*
 *	change log:
 *	12.21	: finish mallocBlock(3), expandFile(3), getNextBlock(2)
 *	12.22	: finish openFile(2), modify creatFile(6)	
 *  12.23	: modify expandFile(2), finish addInfoBlock(3), need to debug
 *	12.24	: modify expandFile(3), finish get/set Table/Block, debug...
 *  12.25   : sync to github
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

#define FREE 0
#define USED 1
#define END (-1)

void panic(char *str)
{
	fprintf_s(stderr, str);
}

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


/* creat a new file,
init, free block table and info table.*/
int creatFile(FILE **fp, char *name, BlockTable *infoTable, long tableSize)
{
	if (fopen_s(fp, name, "wb+") != 0)
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
	if (setFreeTable(*fp, &freeTable) != 0)
	{
		panic("fail to call function\n");
		return -4;
	}

	/* set info table */
	infoTable->isUsed = USED;
	infoTable->num = 0;
	infoTable->tail = END;
	infoTable->head = END;
	if (fwrite(infoTable, tableSize, 1, *fp) == 0)
	{
		panic("unable to write\n");
		return -2;
	}

	/* set first free block */
	Block freeBlock;
	freeBlock.size = END;
	freeBlock.next = END;
	if (fwrite(&freeBlock, sizeof(Block), 1, *fp) == 0)
	{
		panic("unable to write\n");
		return -2;
	}

	return 0;
}

int openFIle(FILE **fp, char *name)
{
	if (fopen_s(fp, name, "rb+") != 0)
	{
		panic("null pointer\\n");
		return -1;
	}
	return 0;
}

/* Should not be api, just for me to test */
int getFreeTable(FILE *fp, BlockTable *table)
{
	fseek(fp, 0, SEEK_SET);
	fread_s(table, sizeof(BlockTable), sizeof(BlockTable), 1, fp);
	if (feof(fp) != 0 || table->isUsed != USED)
	{
		panic("not the right file\n");
		return -3;
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
		return -1;
	}
	return 0;
}

int setFreeTable(FILE *fp, BlockTable *table)
{
	fseek(fp, 0, SEEK_SET);
	if (fwrite(table, sizeof(BlockTable), 1, fp) != 1)
	{
		panic("unable to write\n");
		return -2;
	}
	return 0;
}

int setInfoTable(FILE *fp, BlockTable *table, long size)
{
	fseek(fp, sizeof(BlockTable), SEEK_SET);
	if (fwrite(table, size, 1, fp) != 1)
	{
		panic("uanble to write\n");
		return -2;
	}
	return -2;
}

int getNextBlock(FILE *fp, Block *current)
{
	if (current->next <= 0)
	{
		panic("end of the file\n");
		return 1;
	}

	fseek(fp, current->next, SEEK_SET);
	fread_s(current, sizeof(Block), sizeof(Block), 1, fp);
	return 0;

}

int getBlock(FILE *fp, long position, Block *get, long size)
{
	fseek(fp, position, SEEK_SET);
	fread_s(get, size, size, 1, fp);
	if (feof(fp) != 0)
	{
		panic("get EOF\n");
		return -6;
	}
	return 0;
}

int setBlock(FILE *fp, long position, Block *set, long size)
{
	fseek(fp, position, SEEK_SET);
	if (fwrite(set, size, 1, fp) != 1)
	{
		panic("unable to write\n");
		return -2;
	}
	return 0;
}

/* write the new block with updating the free table,
the alloced block always be the first of the free list.
should not be api, for it may leads to mistakes */
int expandFile(FILE *fp, long size, long *position)
{
	
	fseek(fp, 0, SEEK_END);
	long tempSite = ftell(fp);
	*position = tempSite;

	/* write the block to file */
	char zero = 0;
	for (int i = 0; i < size; i++)
	{
		if (fwrite(&zero, sizeof(char), 1, fp) == 0)
		{
			panic("unable to write\n");
return -2;
		}
	}
	/* set new block'size */
	Block temp = { size, END };
	if (setBlock(fp, tempSite, &temp, sizeof(Block)) != 0)
	{
		panic("fail to call function\n");
		return -4;
	}
	return 0;
}

/* need to be tested
if size < sizeof(Block), set it as sizeof(Block),
this means it can malloc a sizeof(Block) block, althought it's meanningless
Should not be api */
int mallocBlock(FILE *fp, long size, long *position)
{
	if (fp == NULL)
	{
		panic("null pointer\n");
		return -1;
	}

	BlockTable freeTable;
	if (getFreeTable(fp, &freeTable) != 0)
	{
		panic("fail to call function\n");
		return -4;
	}

	if (freeTable.isUsed == FREE)
	{
		panic("not the right file\n");
		return -3;
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
		if (expandFile(fp, size, position) < 0)
		{
			panic("fail to call function\n");
			return -4;
		}
		return 0;
	}

	/* have free blocks, find it */
	Block temp;
	long preSite;
	preSite = freeTable.head;
	if (setBlock(fp, freeTable.head, &temp, sizeof(Block)) != 0)
	{
		panic("fail to call function\n");
		return -4;
	}
	while (temp.size < size && (preSite = ftell(fp), getNextBlock(fp, &temp) == 0));

	/* tested */
	/* fail, no suitable block */
	if (temp.size < size)
	{
		if (expandFile(fp, size, position) < 0)
		{
			panic("fail to call function\n");
			return -4;
		}
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
			if (setBlock(fp, *position, &current, sizeof(Block)) != 0)
			{
				panic("fail to call function\n");
				return -4;
			}

			Block pre;
			if (getBlock(fp, preSite - sizeof(Block), &pre, sizeof(Block)) != 0)
			{
				panic("fail to call function\n");
				return -4;
			}
			pre.next = temp.next;
			if (setBlock(fp, preSite - sizeof(Block), &pre, sizeof(Block)) != 0)
			{
				panic("fail to call function\n");
				return -4;
			}
		}
		else
		{
			/* it's the first block */
			/* tested */
			*position = freeTable.head;
			
			Block current = { size, 0 };
			if (setBlock(fp, *position, &current, sizeof(Block)) != 0)
			{
				panic("fail to call function\n");
				return -4;
			}
		}

		/* updata free table, public for not divided */
		freeTable.num -= 1;
		freeTable.head = temp.next;
		if (freeTable.head == END)
		{
			freeTable.tail = END;
		}
		if (setFreeTable(fp, &freeTable) != 0)
		{
			panic("fail to call function\n");
			return -4;
		}

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
			if (setBlock(fp, *position, &current, sizeof(Block)) != 0)
			{
				panic("fail to call function\n");
				return -4;
			}

			/* modify previous block's next */
			Block pre;
			long preNext = ftell(fp) - sizeof(Block) + size;
			if (getBlock(fp, preSite - sizeof(Block), &pre, sizeof(Block)) != 0)
			{
				panic("fail to call function\n");
				return -4;
			}
			pre.next = preNext;
			if (setBlock(fp, preSite - sizeof(Block), &pre, sizeof(Block)) != 0)
			{
					panic("fail to call function\n");
					return -4;
			}

			/* modify next block */
			Block next = { temp.size - size, temp.next };
			if (setBlock(fp, preNext, &next, sizeof(Block)) != 0)
			{
					panic("fail to call function\n");
					return -4;
			}
			/* don't require to upadta free table */
		}
		else
		{
			/* it's the first block, divide it */
			/* tested */
			*position = preSite;

			/* modify current block's size */
			Block current = { size, preSite + size };
			if (setBlock(fp, preSite, &current, sizeof(Block)) != 0)
			{
				panic("fail to call function\n");
				return -4;
			}

			/* modify freeTable's head */
			freeTable.head = preSite + size;
			if (setFreeTable(fp, &freeTable) != 0)
			{
				panic("fail to call function\n");
				return -4;
			}

			/* modify next block */
			Block next = { temp.size - size, temp.next };
			if (setBlock(fp, preSite + size, &next, sizeof(Block)) != 0)
			{
				panic("fail to call function\n");
				return -4;
			}
			/* done here */
		}

		return 0;
	}
}

/* api */
int addInfoBlock(FILE *fp, Block *add, long size)
{
	if (fp == NULL)
	{
		panic("null pointer");
		return -1;
	}

	/* a sizeof(Block) block is useless for user */
	if (size <= sizeof(Block))
	{
		panic("useless\n");
		return -5;
	}

	long position;
	if (mallocBlock(fp, size, &position) != 0)
	{
		printf("fail to call function\n");
		return -4;
	}
	else
	{
		BlockTable infoTable;
		if (getInfoTable(fp, &infoTable, sizeof(BlockTable)) != 0)
		{
			panic("fail to call function\n");
			return -4;
		}
		long tempSite = infoTable.head;
		infoTable.head = position;
		infoTable.num += 1;
		if (infoTable.tail == END)
		{
			infoTable.tail = position;
		}
		if (setInfoTable(fp, &infoTable, sizeof(BlockTable)) != 0)
		{
			panic("fail to call function\n");
			return -4;
		}

		add->next = tempSite;
		add->size = size;
		if (setBlock(fp, position,&add, size) != 0)
		{
			printf("fail to call function\n");
			return -4;
		}
	
		return 0;
	}
}