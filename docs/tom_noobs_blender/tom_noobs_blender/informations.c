#include "informations.h"

void writeinfile(double *info_list, int count)
{
	FILE *f = fopen("information", "w");
	for(int i = 0; i < count; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			fprintf(f, "%f ", info_list[i * 8 + j]);
		}
		fprintf(f, "\n");
	}
	fclose(f);
}

void copy_matrix(int src[HEIGHT][WIDTH], int dst[HEIGHT][WIDTH])
{
	for (size_t i = 0; i < HEIGHT; i++)
		for (size_t j = 0; j < WIDTH; j++)
			dst[i][j] = src[i][j];
}


double simulation(int matrix[HEIGHT][WIDTH], size_t l1, size_t c1,
		size_t l2, size_t c2)
{
	int mat[HEIGHT][WIDTH];
	unsigned long cScore = score; // current score (before move)
	unsigned long mScore = 0; // points earned with the move
	copy_matrix(matrix, mat);
	if (((l1 == l2+1 || l1 == l2-1) && (c1 == c2)) || ((l1 == l2) && (c1 == c2+1 ||
		c1 == c2-1)))
		cmd(mat, l1, c1, l2, c2, FALSE);
	else
		return 0;
	while (checkgrid(mat, GAME, FALSE) == TRUE)
		continue;
	mScore = score - cScore;
	score = cScore;
	return (double)mScore;

}


// Double combination counts for 1
// if one move makes 2 combinations
// info_list[count*8 + 0] = line of the 1rst fruit
// info_list[count*8 + 1] = column of the 1rst fruit
// info_list[count*8 + 2] = line of the second fruit
// info_list[count*8 + 3] = column of the second fruit
// info_list[count*8 + 4] = score earned with a move/ 10000
// info_list[count*8 + 5] = 1 if SF appeared else 0
// info_list[count*8 + 6] = 1 if FC appeare else 0
// info_list[count*8 + 7] = 1 if FL appeared else 0
int getinfo1(int matrix[HEIGHT][WIDTH], size_t i, size_t j, double *info_list, int count)
{
	int now = matrix[i][j];

	// SPECIAL FRUIT
	// Column : 2 left above and 2 left below
	if ((i >= 1 && j >= 1 && matrix[i-1][j-1] == now) &&
			(i >= 2 && j >= 1 && matrix[i-2][j-1] == now) &&
			(i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now) &&
			(i+2 < HEIGHT && j >= 1 && matrix[i+2][j-1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)(i);
		info_list[count*8 + 3] = (double)((j-1));
		info_list[count*8 + 4] = (double)(simulation(matrix, i, j, i, j-1)/ 10000);
		info_list[count*8 + 5] = 1;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Column : 2 right above and 2 right below
	if ((i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now) &&
			(i >= 2 && j+1 < WIDTH && matrix[i-2][j+1] == now) &&
			(i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now) &&
			(i+2 < HEIGHT && j+1 < WIDTH && matrix[i+2][j+1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)(i);
		info_list[count*8 + 3] = (double)((j+1));
		info_list[count*8 + 4] = (double)(simulation(matrix, i, j, i, j+1)/ 10000);
		info_list[count*8 + 5] = 1;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 2 right above and 2 left above
	if ((i >= 1 && j >= 2 && matrix[i-1][j-2] == now) &&
			(i >= 1 && j >= 1 && matrix[i-1][j-1] == now) &&
			(i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now) &&
			(i >= 1 && j+2 < WIDTH && matrix[i-1][j+2] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)((i-1));
		info_list[count*8 + 3] = (double)(j);
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 1;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 2 right below and 2 left below
	if ((i+1 < HEIGHT && j >= 2 && matrix[i+1][j-2] == now) &&
			(i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now) &&
			(i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now) &&
			(i+1 < HEIGHT && j+2 < WIDTH && matrix[i+1][j+2] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)((i+1));
		info_list[count*8 + 3] = (double)(j);
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 1;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// COLUMN FRUIT
	// 1 left above and 2 right above
	if ((i >= 1 && j >= 1 && matrix[i-1][j-1] == now) &&
			(i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now) &&
			(i >= 1 && j+2 < WIDTH && matrix[i-1][j+2] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)((i-1));
		info_list[count*8 + 3] = (double)(j);
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 1;
		info_list[count*8 + 7] = 0;
		return TRUE;

	}

	// 1 left below and 2 right below
	if ((i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now) &&
			(i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now) &&
			(i+1 < HEIGHT && j+2 < WIDTH && matrix[i+1][j+2] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)((i+1));
		info_list[count*8 + 3] = (double)(j);
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 1;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// 2 left above and 1 right above
	if ((i >= 1 && j >= 2 && matrix[i-1][j-2] == now) &&
			(i >= 1 && j >= 1 && matrix[i-1][j-1] == now) &&
			(i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)((i-1));
		info_list[count*8 + 3] = (double)(j);
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 1;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// 2 left below and 1 right below
	if ((i+1 < HEIGHT && j >= 2 && matrix[i+1][j-2] == now) &&
			(i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now) &&
			(i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)((i+1));
		info_list[count*8 + 3] = (double)(j);
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 1;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// LINE FRUIT
	// 1 left above and 2 left below
	if ((i >= 1 && j >= 1 && matrix[i-1][j-1] == now) &&
			(i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now) &&
			(i+2 < HEIGHT && j >= 1 && matrix[i+2][j-1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)(i);
		info_list[count*8 + 3] = (double)((j-1));
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 1;
		return TRUE;
	}

	// 1 right above and 2 right below
	if ((i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now) &&
			(i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now) &&
			(i+2 < HEIGHT && j+1 < WIDTH && matrix[i+2][j+1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)(i);
		info_list[count*8 + 3] = (double)((j+1));
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 1;
		return TRUE;
	}

	// 2 left above and 1 left below
	if ((i >= 2 && j >= 1 && matrix[i-2][j-1] == now) &&
			(i >= 1 && j >= 1 && matrix[i-1][j-1] == now) &&
			(i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)(i);
		info_list[count*8 + 3] = (double)((j-1));
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 1;
		return TRUE;
	}

	// 2 right above and 1 right below
	if ((i >= 2 && j+1 < WIDTH && matrix[i-2][j+1] == now) &&
			(i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now) &&
			(i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)(i);
		info_list[count*8 + 3] = (double)((j+1));
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 1;
		return TRUE;
	}

	// NO SPECIAL
	// Column : 2 left below
	if ((i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now) &&
			(i+2 < HEIGHT && j >= 1 && matrix[i+2][j-1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)(i);
		info_list[count*8 + 3] = (double)((j-1));
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Column : 2 left above
	if ((i >= 2 && j >= 1 && matrix[i-2][j-1] == now) &&
			(i >= 1 && j >= 1 && matrix[i-1][j-1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)(i);
		info_list[count*8 + 3] = (double)((j-1));
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Column : 2 right below
	if ((i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now) &&
			(i+2 < HEIGHT && j+1 < WIDTH && matrix[i+2][j+1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)(i);
		info_list[count*8 + 3] = (double)((j+1));
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Column : 2 right above
	if ((i >= 2 && j+1 < WIDTH && matrix[i-2][j+1] == now) &&
			(i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)(i);
		info_list[count*8 + 3] = (double)((j+1));
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 1 right above and 1 left above
	if ((i >= 1 && j >= 1 && matrix[i-1][j-1] == now) &&
			(i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)((i-1));
		info_list[count*8 + 3] = (double)(j);
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 1 right below and 1 left below
	if ((i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now) &&
			(i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)((i+1));
		info_list[count*8 + 3] = (double)(j);
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Column : 1 right above and 1 right below
	if ((i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now) &&
			(i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)(i);
		info_list[count*8 + 3] = (double)((j+1));
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Column : 1 left above and 1 left below
	if ((i >= 1 && j >= 1 && matrix[i-1][j-1] == now) &&
			(i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)(i);
		info_list[count*8 + 3] = (double)((j-1));
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 2 left below
	if ((i+1 < HEIGHT && j >= 2 && matrix[i+1][j-2] == now) &&
			(i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)((i+1));
		info_list[count*8 + 3] = (double)(j);
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 2 right above
	if ((i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now) &&
			(i >= 1 && j+2 < WIDTH && matrix[i-1][j+2] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)((i-1));
		info_list[count*8 + 3] = (double)(j);
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 2 left above
	if ((i >= 1 && j >= 2 && matrix[i-1][j-2] == now) &&
			(i >= 1 && j >= 1 && matrix[i-1][j-1] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)((i-1));
		info_list[count*8 + 3] = (double)(j);
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 2 right below
	if ((i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now) &&
			(i+1 < HEIGHT && j+2 < WIDTH && matrix[i+1][j+2] == now))
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)((i+1));
		info_list[count*8 + 3] = (double)(j);
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3])
			/ 10000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 2 right
	if (j+2 < WIDTH && j+3 < WIDTH && matrix[i][j+2] == now &&
			matrix[i][j+3] == now)
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)(i);
		info_list[count*8 + 3] = (double)((j+1));
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3]) /
				1/ 100000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 2 left (X X 0 X)
	if (j >= 3 && matrix[i][j-3] == now && matrix[i][j-2] == now)
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)(i);
		info_list[count*8 + 3] = (double)((j-1));
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3]) /
				1/ 100000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Column : 2 below
	if (i+2 < HEIGHT && i+3 < HEIGHT &&
			matrix[i+2][j] == now && matrix[i+3][j] == now)
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)((i+1));
		info_list[count*8 + 3] = (double)(j);
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3]) /
				1/ 100000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// Column : 2 above
	if (i >= 3 &&
			matrix[i-3][j] == now && matrix[i-2][j] == now)
	{
		info_list[count*8 + 0] = (double)(i);
		info_list[count*8 + 1] = (double)(j);
		info_list[count*8 + 2] = (double)((i-1));
		info_list[count*8 + 3] = (double)(j);
		info_list[count*8 + 4] = (double)(simulation(matrix, info_list[count*8 + 0], info_list[count*8 + 1], info_list[count*8 + 2], info_list[count*8 + 3]) /
				1/ 100000);
		info_list[count*8 + 5] = 0;
		info_list[count*8 + 6] = 0;
		info_list[count*8 + 7] = 0;
		return TRUE;
	}

	// No combination
	return FALSE;
}


int getinfo(int matrix[HEIGHT][WIDTH], double *info_list)
{

	int count = 0;

	for (size_t i = 0; i < HEIGHT; i++)
	{
		for (size_t j = 0; j < WIDTH; j++)
		{
			if (getinfo1(matrix, i, j, info_list, count) == TRUE)
				count++;
		}
	}
	return count;
}


void save_info()
{
	// create new matrix of 8*8
	int matrix[HEIGHT][WIDTH];

	srand(time(0));

	generategrid(matrix);

	while (checkgrid(matrix, START, TRUE) == TRUE)
		continue;

	print_matrix(matrix);


	// creating the list of lists which contains informations about the possible
	// combinations in the matrix
	double *info_list = (double *) malloc (40 * 8 * sizeof(double));

	if (info_list == NULL)
		errx(EXIT_FAILURE, "malloc()");

	int count = getinfo(matrix, info_list);

	info_list = realloc(info_list, count * 8 * sizeof(double));
	/*if (temp == NULL)
		errx(EXIT_FAILURE, "realloc");

	info_list = temp;*/

	writeinfile(info_list, count);

	free(info_list);
}
