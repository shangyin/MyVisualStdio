#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>

/* the struct of employee */
struct employee {
	char name[10];
	char num[10];
	char sex[10];
	char age[10];
	char record[10];
	char position[10];
	char wanges[10];
	char tel[15];
	char addr[50];
};

/* declare the function */
void Menu();
void Add();
void Display();
void Search();
void SearchForName(FILE *fp);
void SearchForPosition(FILE *fp);
void SearchForNum(FILE *fp);
void Modify();
void Delete();
int IsUsed(const char*, FILE *fp);

/* main */
int ain()
{
	Menu();
	return 0;
}

/* implement of functions */
void Menu()
{
	int choice;
	while (1)
	{
		printf("You can type \"h\" for help\n>");
		scanf_s("%d", &choice);
		if (choice >= 0 && choice <= 6)
		{
			switch (choice)
			{
			case 1:
				Add();
				break;
			case 2:
				Search();
				break;
			case 3:
				Modify();
				break;
			case 4:
				Delete();
				break;
			case 5:
				Display();
				break;
			case 6:
				exit(0);
				break;
			default:
				break;
			}
		}
		else
		{
			printf("wrong input!\n");
		}
		while (getchar() != '\n');
	}
}

void Add()
{
	struct employee one;
	FILE *fp;
	int flag = 0;

	if (fopen_s(&fp,"workers", "ab") != 0)
	{
		printf("unable to open workers file!");
		exit(1);
	}

	printf("input the data of the employee\n");
	printf("name:");
	scanf_s("%s", one.name, 10);
	printf("ID:");
	scanf_s("%s", one.num, 10);
	while (flag != 1)
	{
		flag = IsUsed(one.num, fp);
		if (flag != 1)
		{
			printf("unavailable ID, try another\n");
			scanf_s("%s", one.num, 10);
		}
	}

	printf("more detail...\n");
	printf("age:");
	scanf_s("%s", one.age, 10);
	/*
	more info are required here
	*/

	if (fwrite(&one, sizeof(struct employee), 1, fp) != 1)
	{
		printf("unable to write!\n");
	}
	fclose(fp);
}

int IsUsed(const char *num, FILE *fp)
{
	struct employee one;

	fread(&one, sizeof(struct employee), 1, fp);
	while (feof(fp) != 0)
	{
		if (strcmp(num, one.num) == 0)
		{
			fclose(fp);
			return 0;
		}
		fread(&one, sizeof(struct employee), 1, fp);
	}
	return 1;
}

void Display()
{
	struct employee one;
	FILE *fp;

	if (fopen_s(&fp, "workers", "rb") != 0)
	{
		printf("unable to open file\n");
		exit(1);
	}

	printf("here is the info of an employee\n");
	fread(&one, sizeof(struct employee), 1, fp);
	while (feof(fp) == 0)
	{
		printf("%s\t\t\t%s\t\t\t%s\n", one.num, one.name, one.age);
		fread(&one, sizeof(struct employee), 1, fp);
	}
	fclose(fp);
}

void Search()
{
	FILE *fp;
	if (fopen_s(&fp, "workers", "rb") != 0)
	{
		printf("unable to open file\n");
		exit(1);
	}

	int i;
	scanf_s("%d", &i);
	switch (i)
	{
	case 1:
		SearchForName(fp);
		break;
	case 2:
//		SearchForNum(fp);
		break;
	case 3:
//		SearchForPosition(fp);
		break;
	default:
		printf("wrong input!\n");
		while (getchar() != '\n');
		break;
	}
	fclose(fp);
}

void SearchForName(FILE *fp)
{
	char name[10];
	struct employee one;

	scanf_s("%s", name, 10);

	fread_s(&one, sizeof(struct employee), sizeof(struct employee), 1, fp);
	while (feof(fp) == 0)
	{
		if (strcmp(name, one.name) == 0)
		{
			printf("%s\t\t\t%s\t\t%s\n", one.num, one.name, one.age);
		}
		fread_s(&one, sizeof(struct employee), sizeof(struct employee), 1, fp);
	}
	printf("\n\n");
}

void SearchForNum(FILE *fp)
{
	/* similar to SearchForName
	 * unfinish
	 */
}

void SearchForPosition(FILE *fp)
{
	/* similar to SearchForName
	* unfinish
	*/
}

void Modify()
{
	FILE *fp;
	if (fopen_s(&fp, "workers", "rb+") != 0)
	{
		printf("unable to open file\n");
		exit(1);
	}

	long a;
	char num[10];
	struct employee one;
	printf("input the num:");
	scanf_s("%s", num, 10);

	a = ftell(fp);
	fread_s(&one, sizeof(struct employee), sizeof(struct employee), 1, fp);
	
	while (feof(fp) == 0)
	{
		if (strcmp(num, one.num) == 0)
		{
			break;
		}
		printf("%d + %d ", a, sizeof(struct employee));
		a = ftell(fp);
		printf("%d\n", a);
		fread_s(&one, sizeof(struct employee), sizeof(struct employee), 1, fp);
	}

	if (feof(fp) != 0)
	{
		printf("no body here\n");
		return;
	}
	else
	{
		printf("new Name:");
		scanf_s("%s", one.name, 10);
		/*
			code for changes...
		*/
		fseek(fp, a, 0);
		fwrite(&one, sizeof(struct employee), 1, fp);
	}
	fclose(fp);
}

void Delete()
{
	FILE *fp;
	if (fopen_s(&fp, "workers", "rb+") != 0)
	{
		printf("unable to open file\n");
		exit(1);
	}

	char num[10];
	long a;
	struct employee one;
	char choose;

	printf("input the num:");
	scanf_s("%s", num, 10);
	while (getchar() != '\n');

	a = ftell(fp);
	fread_s(&one, sizeof(struct employee), sizeof(struct employee), 1, fp);
	while (feof(fp) == 0)
	{
		if (strcmp(num, one.num) == 0)
		{
			break;
		}
		a = ftell(fp);
		fread_s(&one, sizeof(struct employee), sizeof(struct employee), 1, fp);
	}

	if (feof(fp) != 0)
	{
		printf("nobody is here\n");
		fclose(fp);
		return;
	}
	else
	{
		printf(	"here is the info of the deleted guy:\n"
				"%s\t\t\t%s\t\t\t%s\n"
				"press y to delete it or n to cancel:",
				one.num, one.name, one.age);
		choose = getchar();
		
		if (choose == 'y')
		{
			while (feof(fp) == 0)
			{
				fread_s(&one, sizeof(struct employee), sizeof(struct employee), 1, fp);
			}
		}
		else ;
	}
	fclose(fp);
	return;
}