#include <stdio.h>
#include "mecanics.h"
#include <err.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// oui c'est bon
void getbestscore()
{
	FILE *BS = fopen("bestscore", "r");

	char str[1000];
	if (BS == NULL)
		bestscore = 0;
	else
	{
		if (fgets(str, 1000, BS) != NULL)
			sscanf(str, "%lu", &bestscore);
	}
}

void savebestscore(unsigned long score)
{
	if (score <= bestscore)
		return;

	score = bestscore;
	FILE *BS = fopen("bestscore", "w");

	fprintf(BS, "%lu",score);

	fclose(BS);
}

void invalidargscan()
{
	int c;
	while ((c = fgetc(stdin)) != '\n');
}

int main()
{

	// create new matrix of 8*8
	int matrix[HEIGHT][WIDTH];

	srand(time(0));
	// fill the matrix with random int between 0 and 3
	/* for (size_t i = 0; i < HEIGHT; i++)
	   {
	   for (size_t j = 0; j < WIDTH; j++)
	   matrix[i][j] = rand() % 4;
	   }*/
	generategrid(matrix);

	// testing if there's no column or line of 3 in the grid
	while (checkgrid(matrix, START, TRUE) == TRUE)
		continue;
	getbestscore();
	print_matrix(matrix);
	for (size_t i = 0; i < 7; i++)
		nbf[i] = 0;
	score = 0;
	printf("\n\tSCORE = %lu\tNUMBER OF MOVES REMAINING = %d\t\tBEST SCORE = %lu\n", score,
			NBMOVE, bestscore);

	// the game starts

	size_t l1, c1, l2, c2;
	int i = 0;

	while (i < NBMOVE)
	{
		printf("First fruit line: ");
		while (scanf("%lu", &l1) <= 0)
		{
			printf("%sYou did not enter any number.%s\n", clist[APPLE], KNRM);
			invalidargscan();
			printf("First fruit line: ");
		}
		printf("First fruit column: ");
		while (scanf("%lu", &c1) <= 0)
		{
			printf("%sYou did not enter any number.%s\n", clist[APPLE], KNRM);
			invalidargscan();
			printf("First fruit column: ");
		}

		printf("Second fruit line: ");
		while (scanf("%lu", &l2) <= 0)
		{
			printf("%sYou did not enter any number.%s\n", clist[APPLE], KNRM);
			invalidargscan();
			printf("Second fruit line: ");
		}
		printf("Second fruit column: ");
		while(scanf("%lu", &c2) <= 0)
		{
			printf("%sYou did not enter any number.%s\n", clist[APPLE], KNRM);
			invalidargscan();
			printf("Second fruit column: ");
		}

		if (((l1 == l2+1 || l1 == l2-1) && (c1 == c2)) || ((l1 == l2) && (c1 ==
						c2+1 || c1 == c2-1)))
		{
			cmd(matrix, l1, c1, l2, c2, TRUE);
			i++;
		}

		else
			printf("%s\n\t\tThe two fruits are not next to each other.%s\n",
			clist[APPLE], KNRM);
		while (checkgrid(matrix, GAME, TRUE) == TRUE)
			continue;

		print_matrix(matrix);
		printf("\n\tSCORE = %lu\tNUMBER OF MOVES REMAINING = %d\t\tBEST SCORE = %lu\n", score, NBMOVE - i, bestscore);
	}
	savebestscore(score);
	printf("YOUR SCORE = %lu\t\t\tBEST SCORE = %lu\n", score, bestscore);
	return 0;
}
