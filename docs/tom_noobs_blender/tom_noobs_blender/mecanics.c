#include "mecanics.h"

unsigned long bestscore;

unsigned long score;

unsigned long nbf[7];


char* flist[] = {"APPLE", "ORANGE", "PEER", "PEACH", "SF", "FL", "FC"};
char* clist[] = {"\x1B[31m", "\x1B[33m", "\x1B[32m", "\x1B[35m", "\x1B[36m",
"\x1B[34m", KNRM};

void print_matrix(int matrix[HEIGHT][WIDTH])
{
	printf("   ");
	for (size_t i = 0; i < HEIGHT; i++)
		printf("   %ld    ", i);
	printf("\n   ----------------------------------------------------------------\n");
	for (size_t i = 0; i < HEIGHT; i++)
	{
		printf("%ld |", i);
		for (size_t j = 0; j < WIDTH; j++)
			printf("   %s%d   %s|", clist[matrix[i][j]], matrix[i][j], KNRM);
		if (i < 7)
		{
			printf("   %s%s%s", clist[i], flist[i], KNRM);
			for (size_t j = 0; 7 - strlen(flist[i]) - j; j++)
				printf(" ");
			printf(": %lu", nbf[i]);
		}
		printf("\n   ");
		printf("----------------------------------------------------------------\n");
	}
}

// put random int in the grid
void generategrid(int matrix[HEIGHT][WIDTH])
{
	srand(time(0));
	for(size_t i = 0; i < HEIGHT; i++)
	{
		for(size_t j = 0; j < WIDTH; j++)
		{
			matrix[i][j] = rand() % 4;
		}
	}
}

// make fruits fall by column
void fruitsfall(int matrix[HEIGHT][WIDTH], size_t column, int cond)
{
	//print_matrix(matrix);
	//sleep(1);
	int down = HEIGHT - 1;
	while (down >= 0 && matrix[down][column] != EMPTY)
	{
		down--;
	}
	int up = down;
	while (up >= 0 && matrix[up][column] == EMPTY)
	{
		up--;
	}
	while (up >= 0)
	{
		matrix[down][column] = matrix[up][column];
		matrix[up][column] = EMPTY;
		up--;
		down--;
	}
	if (cond == TRUE)
	{
		while (down >= 0)
		{
			matrix[down][column] = rand() % 4;
			down--;
		}
	}
	//print_matrix(matrix);
	//sleep(1);
}

void swap(int matrix[HEIGHT][WIDTH], size_t l1, size_t c1, size_t l2, size_t c2)
{
	int a = matrix[l1][c1];
	matrix[l1][c1] = matrix[l2][c2];
	matrix[l2][c2] = a;
}

void destroy(int matrix[HEIGHT][WIDTH], int cond)
{
  nbf[SF] += 2;
	score += 480;
	for (size_t i = 0; i < HEIGHT; i++)
	{
		for (size_t j = 0; j < WIDTH; j++)
		{
			if (matrix[i][j] == SF)
				score += 240;
			if (matrix[i][j] == FLine || matrix[i][j] == FColumn)
				score += 180;
			else
				score += 40;
			nbf[matrix[i][j]] ++;

			if (cond == TRUE)
				matrix[i][j] = rand() % 4;
      else
        matrix[i][j] = EMPTY;
		}
	}
}

void destroyline(int matrix[HEIGHT][WIDTH], size_t line, int rando)
{
	score += 180;
	for (size_t i = 0; i < WIDTH; i++)
	{
    if (matrix[line][i] == EMPTY)
      fruitsfall(matrix, i, rando);
    else
		  nbf[matrix[line][i]] ++;
		if (matrix[line][i] == SF)
		{
      matrix[line][i] = EMPTY;
      fruitsfall(matrix, i, rando);
      transSF(matrix, rand() % 4, rando);
    }
		if (matrix[line][i] == FLine || matrix[line][i] == FColumn)
		{
			matrix[line][i] = EMPTY;
			fruitsfall(matrix, i, rando);
			destroycolumn(matrix, i, rando);
		}
		else
		{
			score+=40;
			matrix[line][i] = EMPTY;
			fruitsfall(matrix, i, rando);
		}
	}
}

void destroycolumn(int matrix[HEIGHT][WIDTH], size_t column, int rando)
{
	score += 180;
	for (size_t i = 0; i < HEIGHT; i++)
	{
    if (matrix[i][column] == EMPTY)
      fruitsfall(matrix, column, rando);
    else
		  nbf[matrix[i][column]] ++;
		if (matrix[i][column] == SF)
		{
      matrix[i][column] = EMPTY;
      fruitsfall(matrix, column, rando);
      transSF(matrix, rand() % 4, rando);
    }
		if (matrix[i][column] == FLine || matrix[i][column] == FColumn)
		{
			matrix[i][column] = EMPTY;
			fruitsfall(matrix, column, rando);
			destroyline(matrix, i, rando);
		}
		else
		{
			score+=40;
			matrix[i][column] = EMPTY;
		}
	}
	fruitsfall(matrix, column, rando);
}

// explode every type fruits
void transSF(int matrix[HEIGHT][WIDTH], int type, int rando)
{
	nbf[SF]++;
	score += 240;
	size_t count = 0;
	for (size_t i = 0; i < HEIGHT; i++)
	{
		for (size_t j = 0; j < WIDTH; j++)
		{
			if (matrix[i][j] == type)
			{
				matrix[i][j] = EMPTY;
				fruitsfall(matrix, j, rando);
				count++;
			}
		}
	}
	nbf[type] += count;
	score += count*40;
}

// check around the fruit in matrix[l][c]
int checkaround(int matrix[HEIGHT][WIDTH], size_t l, size_t c, int condition,
	int rando)
{
	if (matrix[l][c] == EMPTY)
		return FALSE;

	size_t nbfruit = 1;

	int li = l;
	int co = c;

	// count the nb of matrix[l][c] type fruits on the left
	while (co-1 >= 0 && matrix[l][co-1] == matrix[l][c])
	{
		nbfruit++;
		co--;
	}
	co = c;
	// count the nb of matrix[l][c] type fruits on the right
	while (co+1 < WIDTH && matrix[l][co+1] == matrix[l][c])
	{
		nbfruit++;
		co++;
	}

	// line of 3 minimum
	if (nbfruit == 5)
	{
		if (condition == GAME)
		{
			nbf[matrix[l][c]]+=nbfruit;
			score+=200;
			for (size_t i = 0; i < nbfruit; i++)
			{
        		if (i == 2)
          		matrix[l][co-i] = SF;
				else
        		{
          			matrix[l][co-i] = EMPTY;
				  	fruitsfall(matrix, co-i, rando);
        		}
			}
			return TRUE;
		}
		else if (rando == TRUE)
		{
			for (size_t i = 0; i < nbfruit; i++)
			  matrix[l][co-i] = rand() % 4;
      		return TRUE;
		}
		return TRUE;
	}
	if (nbfruit == 4)
	{
		if (condition == GAME)
		{
			nbf[matrix[l][c]] += nbfruit;
			score += 160;
			for (size_t i = 0; i < nbfruit-1; i++)
			{
				if (i == 2)
					matrix[l][co-i] = FColumn;
				else
				{
					matrix[l][co-i] = EMPTY;
					fruitsfall(matrix, co-i, rando);
				}
			}
			return TRUE;
		}
		else if (rando == TRUE)
		{
			for (size_t i = 0; i < nbfruit; i++)
				matrix[l][co-i] = rand() % 4;
			return TRUE;
		}
		return TRUE;
	}
	if (nbfruit == 3)
	{
		if (condition == GAME)
		{
			score += 120;
			nbf[matrix[l][c]]+=nbfruit;
		}
		for (size_t i = 0; i< nbfruit; i++)
		{
			matrix[l][co-i] = EMPTY;
			fruitsfall(matrix, co-i, rando);
		}
		return TRUE;
	}
	nbfruit = 1;
	// same but above
	while (li-1 >= 0 && matrix[li-1][c] == matrix[l][c])
	{
		nbfruit++;
		li--;
	}
	li = l;
	// same but below
	while (li+1 < HEIGHT && matrix[li+1][c] == matrix[l][c])
	{
		nbfruit++;
		li++;
	}
	// column of 3 minimum
	if (nbfruit == 5)
	{
		if (condition == GAME)
		{
			nbf[matrix[l][c]] += nbfruit;
			score+=200;
			for (size_t i = 0; i < nbfruit; i++)
				matrix[li-i][c] = EMPTY;
			matrix[li][c] = SF;
			fruitsfall(matrix, c, rando);
			return TRUE;
		}
		else
		{
			for (size_t i = 0; i < nbfruit; i++)
				matrix[li-i][c] = rand() % 4;
			return TRUE;
		}
		return TRUE;
	}
	if (nbfruit == 4)
	{
		if (condition == GAME)
		{
			nbf[matrix[l][c]] += nbfruit;
			score += 160;
			for (size_t i = 0; i < nbfruit; i++)
				matrix[li-i][c] = EMPTY;
			matrix[li][c] = FLine;
			fruitsfall(matrix, c, rando);
			return TRUE;
		}
		else if (rando == TRUE)
		{
			for (size_t i = 0; i < nbfruit; i++)
				matrix[li-i][c] = rand() % 4;
			return TRUE;
		}
		return TRUE;
	}
	if (nbfruit == 3)
	{
		if (condition == GAME)
		{
			score += 120;
			nbf[matrix[l][c]] += nbfruit;
		}
		for (size_t i = 0; i < nbfruit; i++)
			matrix[li-i][c] = EMPTY;
		fruitsfall(matrix,c, rando);
		return TRUE;
	}
	else
		return FALSE;
}

// use this function when 2 fruits are selected
void cmd(int matrix[HEIGHT][WIDTH], size_t l1, size_t c1, size_t l2, size_t c2,
	int rando)
{
	if (matrix[l1][c1] == matrix[l2][c2] && matrix[l1][c1] <= 3)
		return;

  // 2 selected fruits are swapped
  swap(matrix, l1, c1, l2, c2);

  int fruit1 = matrix[l2][c2];
  int fruit2 = matrix[l1][c1];

  // 2 SF are selected
  if (fruit1 == fruit2 && fruit1 == SF)
  {
    nbf[SF] += 2;
    // the matrix is destroyed
    destroy(matrix, rando);
    while (checkgrid(matrix, GAME, rando) == TRUE)
      continue;
  }

  // 2 FC are selected
  if (fruit1 == fruit2 && fruit1 == FColumn)
  {
    nbf[FColumn] += 2;
    matrix[l1][c1] = EMPTY;
    matrix[l2][c2] = EMPTY;
    // column of fruit1 is destroyed
    destroycolumn(matrix, c2, rando);

    // line of fruit2 ile système es destroyed
    destroyline(matrix, l1, rando);
  }

  // 2 FL are selected
  if (fruit1 == fruit2 && fruit1 == FLine)
  {
    nbf[FLine] += 2;
    matrix[l1][c1] = EMPTY;
    matrix[l2][c2] = EMPTY;
    // line of fruit1 is destroyed
    destroyline(matrix, l2, rando);

    // column of fruit2 is destroyed
    destroycolumn(matrix, c1, rando);
  }

  // 1 SF and 1 normal fruit are selected
  if (fruit1 == SF && fruit2 <= 3)
  {
    nbf[SF] ++;
    matrix[l2][c2] = EMPTY;
    fruitsfall(matrix, c2, rando);
    transSF(matrix, fruit2, rando);
  }

  // 1 FC and 1 normal fruit are selected
  if (fruit1 == FColumn && fruit2 <= 3)
  {
    nbf[FColumn] ++;
    matrix[l2][c2] = EMPTY;
    fruitsfall(matrix, c2, rando);
    destroycolumn(matrix, c2, rando);
  }

  // 1 FL and 1 normal fruit are selected
  if (fruit1 == FLine && fruit2 <= 3)
  {
    nbf[FLine] ++;
    matrix[l2][c2] = EMPTY;
    fruitsfall(matrix, c2, rando);
    destroyline(matrix, l2, rando);
  }

  // 1 normal fruit and 1 SF are selected
  if (fruit1 <= 3 && fruit2 == SF)
  {
    nbf[SF] ++;
    matrix[l1][c1] = EMPTY;
    fruitsfall(matrix, c1, rando);
    transSF(matrix, fruit1, rando);
  }

  // 1 normal fruit and 1 FC are selected
  if (fruit1 <= 3 && fruit2 == FColumn)
  {
    nbf[FColumn] ++;
    matrix[l1][c1] = EMPTY;
    fruitsfall(matrix, c1, rando);
    destroycolumn(matrix, c1, rando);
  }

  // 1 normal fruit and 1 FL are selected
  if (fruit1 <= 3 && fruit2 == FLine)
  {
    nbf[FLine] ++;
    matrix[l1][c1] = EMPTY;
    fruitsfall(matrix, c1, rando);
    destroyline(matrix, l1, rando);
  }

  // 1 SF and 1 FC/FL are selected or
  // 1 FC/FL and 1 SF are selected
  if ((fruit1 == SF && (fruit2 == FColumn || fruit2 == FLine)) ||
    ((fruit1 == FColumn || fruit1 == FLine) && fruit2 == SF))
  {
    nbf[fruit2] ++;
    nbf[fruit1] ++;
    matrix[l1][c1] = EMPTY;
    fruitsfall(matrix, c1, rando);
    matrix[l2][c2] = EMPTY;
    fruitsfall(matrix, c2, rando);
    transSF(matrix, rand() % 4, rando);
  }

  // 1 FL and 1 FC are selected
  if (fruit1 == FLine && fruit2 == FColumn)
  {
    nbf[fruit1] ++;
    nbf[fruit2] ++;
    matrix[l1][c1] = EMPTY;
    fruitsfall(matrix, c1, rando);
    matrix[l2][c2] = EMPTY;
    fruitsfall(matrix, c2, rando);
    destroyline(matrix, l2, rando);
    destroycolumn(matrix, c1, rando);
  }

  // 1 FC and 1 FL are selected
  if (fruit1 == FColumn && fruit2 == FLine)
  {
    nbf[fruit1] ++;
    nbf[fruit2] ++;
    matrix[l1][c1] = EMPTY;
    fruitsfall(matrix, c1, rando);
    matrix[l2][c2] = EMPTY;
    fruitsfall(matrix, c2, rando);
    destroyline(matrix, l1, rando);
    destroycolumn(matrix, c2, rando);
  }

  else if (checkaround(matrix, l1, c1, GAME, rando) == FALSE &&
    checkaround(matrix, l2, c2, GAME, rando) == FALSE)
      swap(matrix, l1, c1, l2, c2);
}


// Called when a new grid is generated by generategrid
// or when a line or column is exploded
// return 1 if the grid has changed
// return 0 if the grid has not changed

int checkgrid(int matrix[HEIGHT][WIDTH], int condition, int rando)
{
	for (int i = HEIGHT - 1; i >= 0; i--)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (checkaround(matrix, i, j, condition, rando) == TRUE)
				return TRUE;
		}
	}
	return FALSE;
}
