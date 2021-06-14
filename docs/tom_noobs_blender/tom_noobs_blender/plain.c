//#define _GNU_SOURCE

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include "plain.h"
#include "NeuralNetwork.h"

// Functions from informations.c ---------------------------------------------

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
int getinfo1(int matrix[HEIGHT][WIDTH], size_t i, size_t j, double *info_list,
		int *count)
{
	int now = matrix[i][j];

	if ((now == SF || now == FLine || now == FColumn) && (i < HEIGHT && j < WIDTH))
	{
		double bs;
		size_t l1 = i;
		size_t c1 = j;
		size_t l2;
		size_t c2;
		// Left
		if (j >= 1)
		{
			bs = simulation(matrix, i, j, i, j-1);
			l2 = i;
			c2 = j-1;
		}
		else if (j+1 < WIDTH)
		{
			bs = simulation(matrix, i, j, i, j+1);
			l2 = i;
			c2 = j+1;
		}
		else if (i >= 1)
		{
			bs = simulation(matrix, i, j, i-1, j);
			l2 = i-1;
			c2 = j;
		}
		else if (i+1 < HEIGHT)
		{
			bs = simulation(matrix, i, j, i+1, j);
			l2 = i+1;
			c2 = j;
		}
		// Right
		if (c2 != j+1 && j+1 < WIDTH && simulation(matrix, i, j, i, j+1) > bs)
		{
			bs = simulation(matrix, i, j, i, j+1);
			l2 = i;
			c2 = j+1;
		}
		// Up
		if (l2 != i-1 && i >= 1 && simulation(matrix, i, j, i-1, j) > bs)
		{
			bs = simulation(matrix, i, j, i-1, j);
			l2 = i-1;
			c2 = j;
		}
		// Down
		if (l2 != i+1 && i+1 < HEIGHT && simulation(matrix, i, j, i+1, j) > bs)
		{
			bs = simulation(matrix, i, j, i+1, j);
			l2 = i+1;
			c2 = j;
		}
		info_list[*count*8 + 0] = (double)l1;
		info_list[*count*8 + 1] = (double)c1;
		info_list[*count*8 + 2] = (double)l2;
		info_list[*count*8 + 3] = (double)c2;
		info_list[*count*8 + 4] = simulation(matrix, l1, c1, l2, c2) / 10000;
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// SPECIAL FRUIT
	// Column : 2 left above and 2 left below
	if ((i >= 1 && j >= 1 && matrix[i-1][j-1] == now) &&
			(i >= 2 && j >= 1 && matrix[i-2][j-1] == now) &&
			(i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now) &&
			(i+2 < HEIGHT && j >= 1 && matrix[i+2][j-1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)(i);
		info_list[*count*8 + 3] = (double)((j-1));
		info_list[*count*8 + 4] = (double)(simulation(matrix, i, j, i, j-1)/ 10000);
		info_list[*count*8 + 5] = 1;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Column : 2 right above and 2 right below
	if ((i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now) &&
			(i >= 2 && j+1 < WIDTH && matrix[i-2][j+1] == now) &&
			(i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now) &&
			(i+2 < HEIGHT && j+1 < WIDTH && matrix[i+2][j+1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)(i);
		info_list[*count*8 + 3] = (double)((j+1));
		info_list[*count*8 + 4] = (double)(simulation(matrix, i, j, i, j+1)/ 10000);
		info_list[*count*8 + 5] = 1;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 2 right above and 2 left above
	if ((i >= 1 && j >= 2 && matrix[i-1][j-2] == now) &&
			(i >= 1 && j >= 1 && matrix[i-1][j-1] == now) &&
			(i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now) &&
			(i >= 1 && j+2 < WIDTH && matrix[i-1][j+2] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)((i-1));
		info_list[*count*8 + 3] = (double)(j);
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 1;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 2 right below and 2 left below
	if ((i+1 < HEIGHT && j >= 2 && matrix[i+1][j-2] == now) &&
			(i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now) &&
			(i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now) &&
			(i+1 < HEIGHT && j+2 < WIDTH && matrix[i+1][j+2] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)((i+1));
		info_list[*count*8 + 3] = (double)(j);
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 1;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// COLUMN FRUIT
	// 1 left above and 2 right above
	if ((i >= 1 && j >= 1 && matrix[i-1][j-1] == now) &&
			(i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now) &&
			(i >= 1 && j+2 < WIDTH && matrix[i-1][j+2] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)((i-1));
		info_list[*count*8 + 3] = (double)(j);
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 1;
		info_list[*count*8 + 7] = 0;
		return TRUE;

	}

	// 1 left below and 2 right below
	if ((i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now) &&
			(i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now) &&
			(i+1 < HEIGHT && j+2 < WIDTH && matrix[i+1][j+2] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)((i+1));
		info_list[*count*8 + 3] = (double)(j);
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 1;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// 2 left above and 1 right above
	if ((i >= 1 && j >= 2 && matrix[i-1][j-2] == now) &&
			(i >= 1 && j >= 1 && matrix[i-1][j-1] == now) &&
			(i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)((i-1));
		info_list[*count*8 + 3] = (double)(j);
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 1;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// 2 left below and 1 right below
	if ((i+1 < HEIGHT && j >= 2 && matrix[i+1][j-2] == now) &&
			(i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now) &&
			(i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)((i+1));
		info_list[*count*8 + 3] = (double)(j);
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 1;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// LINE FRUIT
	// 1 left above and 2 left below
	if ((i >= 1 && j >= 1 && matrix[i-1][j-1] == now) &&
			(i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now) &&
			(i+2 < HEIGHT && j >= 1 && matrix[i+2][j-1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)(i);
		info_list[*count*8 + 3] = (double)((j-1));
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 1;
		return TRUE;
	}

	// 1 right above and 2 right below
	if ((i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now) &&
			(i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now) &&
			(i+2 < HEIGHT && j+1 < WIDTH && matrix[i+2][j+1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)(i);
		info_list[*count*8 + 3] = (double)((j+1));
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 1;
		return TRUE;
	}

	// 2 left above and 1 left below
	if ((i >= 2 && j >= 1 && matrix[i-2][j-1] == now) &&
			(i >= 1 && j >= 1 && matrix[i-1][j-1] == now) &&
			(i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)(i);
		info_list[*count*8 + 3] = (double)((j-1));
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 1;
		return TRUE;
	}

	// 2 right above and 1 right below
	if ((i >= 2 && j+1 < WIDTH && matrix[i-2][j+1] == now) &&
			(i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now) &&
			(i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)(i);
		info_list[*count*8 + 3] = (double)((j+1));
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 1;
		return TRUE;
	}

	// NO SPECIAL
	// Column : 2 left below
	if ((i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now) &&
			(i+2 < HEIGHT && j >= 1 && matrix[i+2][j-1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)(i);
		info_list[*count*8 + 3] = (double)((j-1));
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Column : 2 left above
	if ((i >= 2 && j >= 1 && matrix[i-2][j-1] == now) &&
			(i >= 1 && j >= 1 && matrix[i-1][j-1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)(i);
		info_list[*count*8 + 3] = (double)((j-1));
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Column : 2 right below
	if ((i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now) &&
			(i+2 < HEIGHT && j+1 < WIDTH && matrix[i+2][j+1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)(i);
		info_list[*count*8 + 3] = (double)((j+1));
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Column : 2 right above
	if ((i >= 2 && j+1 < WIDTH && matrix[i-2][j+1] == now) &&
			(i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)(i);
		info_list[*count*8 + 3] = (double)((j+1));
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 1 right above and 1 left above
	if ((i >= 1 && j >= 1 && matrix[i-1][j-1] == now) &&
			(i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)((i-1));
		info_list[*count*8 + 3] = (double)(j);
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 1 right below and 1 left below
	if ((i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now) &&
			(i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)((i+1));
		info_list[*count*8 + 3] = (double)(j);
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Column : 1 right above and 1 right below
	if ((i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now) &&
			(i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)(i);
		info_list[*count*8 + 3] = (double)((j+1));
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Column : 1 left above and 1 left below
	if ((i >= 1 && j >= 1 && matrix[i-1][j-1] == now) &&
			(i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)(i);
		info_list[*count*8 + 3] = (double)((j-1));
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 2 left below
	if ((i+1 < HEIGHT && j >= 2 && matrix[i+1][j-2] == now) &&
			(i+1 < HEIGHT && j >= 1 && matrix[i+1][j-1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)((i+1));
		info_list[*count*8 + 3] = (double)(j);
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 2 right above
	if ((i >= 1 && j+1 < WIDTH && matrix[i-1][j+1] == now) &&
			(i >= 1 && j+2 < WIDTH && matrix[i-1][j+2] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)((i-1));
		info_list[*count*8 + 3] = (double)(j);
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 2 left above
	if ((i >= 1 && j >= 2 && matrix[i-1][j-2] == now) &&
			(i >= 1 && j >= 1 && matrix[i-1][j-1] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)((i-1));
		info_list[*count*8 + 3] = (double)(j);
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 2 right below
	if ((i+1 < HEIGHT && j+1 < WIDTH && matrix[i+1][j+1] == now) &&
			(i+1 < HEIGHT && j+2 < WIDTH && matrix[i+1][j+2] == now))
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)((i+1));
		info_list[*count*8 + 3] = (double)(j);
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3])
				/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 2 right
	if (j+2 < WIDTH && j+3 < WIDTH && matrix[i][j+2] == now &&
			matrix[i][j+3] == now)
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)(i);
		info_list[*count*8 + 3] = (double)((j+1));
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3]) /
				1/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Line : 2 left (X X 0 X)
	if (j >= 3 && matrix[i][j-3] == now && matrix[i][j-2] == now)
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)(i);
		info_list[*count*8 + 3] = (double)((j-1));
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3]) /
				1/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Column : 2 below
	if (i+2 < HEIGHT && i+3 < HEIGHT &&
			matrix[i+2][j] == now && matrix[i+3][j] == now)
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)((i+1));
		info_list[*count*8 + 3] = (double)(j);
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3]) /
				1/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// Column : 2 above
	if (i >= 3 &&
			matrix[i-3][j] == now && matrix[i-2][j] == now)
	{
		info_list[*count*8 + 0] = (double)(i);
		info_list[*count*8 + 1] = (double)(j);
		info_list[*count*8 + 2] = (double)((i-1));
		info_list[*count*8 + 3] = (double)(j);
		info_list[*count*8 + 4] = (double)(simulation(matrix, info_list[*count*8 + 0], info_list[*count*8 + 1], info_list[*count*8 + 2], info_list[*count*8 + 3]) /
				1/ 10000);
		info_list[*count*8 + 5] = 0;
		info_list[*count*8 + 6] = 0;
		info_list[*count*8 + 7] = 0;
		return TRUE;
	}

	// No combination
	return FALSE;
}


void getinfo(int matrix[HEIGHT][WIDTH], double *info_list, int *count)
{
	for (size_t i = 0; i < HEIGHT; i++)
	{
		for (size_t j = 0; j < WIDTH; j++)
		{
			if (getinfo1(matrix, i, j, info_list, count) == TRUE)
				(*count)++;
		}
	}
}


double* save_info(int matrix[HEIGHT][WIDTH], int *count)
{
	// creating the list of lists which contains informations about the possible
	// combinations in the matrix
	double *info_list = (double *) malloc (40 * 8 * sizeof(double));

	if (info_list == NULL)
		errx(EXIT_FAILURE, "malloc()");

	getinfo(matrix, info_list, count);

	info_list = realloc(info_list, (*count) * 8 * sizeof(double));

	return info_list;
}


// Functions from mecanics.c -------------------------------------------------

unsigned long bestscore;

unsigned long score;

unsigned long nbf[7];


char* flist[] = {"APPLE", "ORANGE", "PEER", "PEACH", "SF", "FL", "FC"};
char* clist[] = {"\x1B[31m", "\x1B[33m", "\x1B[32m", "\x1B[35m", "\x1B[36m",
	"\x1B[34m", KNRM};

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

			/*if (cond == TRUE)
				matrix[i][j] = rand() % 4;
				else
				matrix[i][j] = EMPTY;*/
			matrix[i][j] = EMPTY;
		}
	}
	for (size_t i = 0; i < WIDTH; i++)
		fruitsfall(matrix, i, cond, GAME);
}

void destroyline(int matrix[HEIGHT][WIDTH], size_t line, int rando)
{
	score += 180;
	for (size_t i = 0; i < WIDTH; i++)
	{
		if (matrix[line][i] == EMPTY)
			fruitsfall(matrix, i, rando, GAME);
		else
			if (rando == TRUE)
				nbf[matrix[line][i]] ++;
		if (matrix[line][i] == SF)
		{
			matrix[line][i] = EMPTY;
			//fruitsfall(matrix, i, rando, GAME);
			transSF(matrix, rand() % 4, rando);
		}
		if (matrix[line][i] == FLine || matrix[line][i] == FColumn)
		{
			matrix[line][i] = EMPTY;
			//fruitsfall(matrix, i, rando, GAME);
			destroycolumn(matrix, i, rando);
		}
		else
		{
			score+=40;
			matrix[line][i] = EMPTY;
			//fruitsfall(matrix, i, rando, GAME);
		}
	}
	for (size_t i = 0; i < WIDTH; i++)
		fruitsfall(matrix, i, rando, GAME);
}

void destroycolumn(int matrix[HEIGHT][WIDTH], size_t column, int rando)
{
	score += 180;
	for (size_t i = 0; i < HEIGHT; i++)
	{
		if (matrix[i][column] == EMPTY)
			fruitsfall(matrix, column, rando, GAME);
		else
			if (rando == TRUE)
				nbf[matrix[i][column]] ++;
		if (matrix[i][column] == SF)
		{
			matrix[i][column] = EMPTY;
			//fruitsfall(matrix, column, rando, GAME);
			transSF(matrix, rand() % 4, rando);
			fruitsfall(matrix, column, rando, GAME);
		}
		if (matrix[i][column] == FLine || matrix[i][column] == FColumn)
		{
			matrix[i][column] = EMPTY;
			//fruitsfall(matrix, column, rando, GAME);
			destroyline(matrix, i, rando);
			for (size_t i = 0; i < WIDTH; i++)
				fruitsfall(matrix, column, rando, GAME);
		}
		else
		{
			score+=40;
			matrix[i][column] = EMPTY;
		}
	}
	fruitsfall(matrix, column, rando, GAME);
}

// explode every type fruits
void transSF(int matrix[HEIGHT][WIDTH], int type, int rando)
{
	score += 240;
	size_t count = 0;
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (matrix[i][j] == type)
			{
				matrix[i][j] = EMPTY;
				//fruitsfall(matrix, j, rando, GAME);
				count++;
			}
		}
	}
	if (rando == TRUE)
		nbf[type] += count;
	score += count*40;
	for (size_t i = 0; i < WIDTH; i++)
		fruitsfall(matrix, i, rando, GAME);
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
	while (co > 0 && matrix[l][co-1] == matrix[l][c])
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
	if (nbfruit >= 5)
	{
		if (condition == GAME)
		{
			if (rando == TRUE)
				nbf[matrix[l][c]]+=nbfruit;
			score+=200;
			for (size_t i = 0; i < nbfruit; i++)
			{
				if (i == 2)
					matrix[l][co-i] = SF;
				else
				{
					matrix[l][co-i] = EMPTY;
				}
			}
			for(size_t i = 0; i < nbfruit; i ++)
			{
				if (matrix[l][co-i] == EMPTY)
					fruitsfall(matrix, co-i, rando, condition);
			}
		}
		else
		{
			for (size_t i = 0; i < nbfruit; i++)
				matrix[l][co-i] = rand() % 4;
		}
		return TRUE;
	}
	if (nbfruit == 4)
	{
		if (condition == GAME)
		{
			if (rando == TRUE)
				nbf[matrix[l][c]] += nbfruit;
			score += 160;
			for (size_t i = 0; i < nbfruit; i++)
			{
				if (i == 2)
					matrix[l][co-i] = FColumn;
				else
				{
					matrix[l][co-i] = EMPTY;
				}
			}
			for (size_t i = 0; i < nbfruit; i++)
				fruitsfall(matrix, co-i, rando, condition);
		}
		else
		{
			nbf[matrix[l][c]] += nbfruit;
			for (size_t i = 0; i < nbfruit; i++)
				matrix[l][co-i] = EMPTY;
			for (size_t i = 0; i < nbfruit; i++)
				fruitsfall(matrix, co - i, rando, condition);
		}
		return TRUE;
	}
	if (nbfruit == 3)
	{
		if (condition == GAME)
		{
			score += 120;
			if (rando == TRUE)
				nbf[matrix[l][c]]+=nbfruit;
		}
		for (size_t i = 0; i< nbfruit; i++)
		{
			matrix[l][co-i] = EMPTY;
		}
		for (size_t i = 0; i < nbfruit; i++)
			fruitsfall(matrix, co-i, rando, condition);
		return TRUE;
	}
	nbfruit = 1;
	li = l;
	// same but above
	while (li > 0 && matrix[li - 1][c] == matrix[l][c])
	{
		nbfruit++;
		li--;
	}
	li = l;
	// same but below
	while (li + 1 < HEIGHT && matrix[li + 1][c] == matrix[l][c])
	{
		nbfruit++;
		li++;
	}
	// column of 3 minimum
	if (nbfruit >= 5)
	{
		if (condition == GAME)
		{
			if (rando == TRUE)
				nbf[matrix[l][c]] += nbfruit;
			score+=200;
			for (size_t i = 0; i < nbfruit; i++)
				matrix[li-i][c] = EMPTY;
			matrix[li][c] = SF;
			fruitsfall(matrix, c, rando, condition);
		}
		else
		{
			for (size_t i = 0; i < nbfruit; i++)
				matrix[li-i][c] = rand() % 4;
		}
		return TRUE;
	}
	if (nbfruit == 4)
	{
		if (condition == GAME)
		{
			if (rando == TRUE)
				nbf[matrix[l][c]] += nbfruit;
			score += 160;
			for (size_t i = 0; i < nbfruit; i++)
				matrix[li-i][c] = EMPTY;
			matrix[li][c] = FLine;
			fruitsfall(matrix, c, rando, condition);
		}
		else
		{
			for (size_t i = 0; i < nbfruit; i++)
				matrix[li-i][c] = rand() % 4;
		}
		return TRUE;
	}
	if (nbfruit == 3)
	{
		if (condition == GAME)
		{
			score += 120;
			if (rando == TRUE)
				nbf[matrix[l][c]] += nbfruit;
		}
		for (size_t i = 0; i < nbfruit; i++)
			matrix[li-i][c] = EMPTY;
		fruitsfall(matrix,c, rando, condition);
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
		if (rando == TRUE)
			nbf[SF] += 2;
		// the matrix is destroyed
		destroy(matrix, rando);
		while (checkgrid(matrix, GAME, rando) == TRUE)
			continue;
	}

	// 2 FC are selected
	if (fruit1 == fruit2 && fruit1 == FColumn)
	{
		if (rando == TRUE)
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
		if (rando == TRUE)
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
		if (rando == TRUE)
			nbf[SF] ++;
		matrix[l2][c2] = EMPTY;
		fruitsfall(matrix, c2, rando, GAME);
		transSF(matrix, fruit2, rando);
	}

	// 1 FC and 1 normal fruit are selected
	if (fruit1 == FColumn && fruit2 <= 3)
	{
		if (rando == TRUE)
			nbf[FColumn] ++;
		matrix[l2][c2] = EMPTY;
		fruitsfall(matrix, c2, rando, GAME);
		destroycolumn(matrix, c2, rando);
	}

	// 1 FL and 1 normal fruit are selected
	if (fruit1 == FLine && fruit2 <= 3)
	{
		if (rando == TRUE)
			nbf[FLine] ++;
		matrix[l2][c2] = EMPTY;
		fruitsfall(matrix, c2, rando, GAME);
		destroyline(matrix, l2, rando);
	}

	// 1 normal fruit and 1 SF are selected
	if (fruit1 <= 3 && fruit2 == SF)
	{
		if (rando == TRUE)
			nbf[SF] ++;
		matrix[l1][c1] = EMPTY;
		fruitsfall(matrix, c1, rando, GAME);
		transSF(matrix, fruit1, rando);
	}

	// 1 normal fruit and 1 FC are selected
	if (fruit1 <= 3 && fruit2 == FColumn)
	{
		if (rando == TRUE)
			nbf[FColumn] ++;
		matrix[l1][c1] = EMPTY;
		fruitsfall(matrix, c1, rando, GAME);
		destroycolumn(matrix, c1, rando);
	}

	// 1 normal fruit and 1 FL are selected
	if (fruit1 <= 3 && fruit2 == FLine)
	{
		if (rando == TRUE)
			nbf[FLine] ++;
		matrix[l1][c1] = EMPTY;
		fruitsfall(matrix, c1, rando, GAME);
		destroyline(matrix, l1, rando);
	}

	// 1 SF and 1 FC/FL are selected or
	// 1 FC/FL and 1 SF are selected
	if ((fruit1 == SF && (fruit2 == FColumn || fruit2 == FLine)) ||
			((fruit1 == FColumn || fruit1 == FLine) && fruit2 == SF))
	{
		if (rando == TRUE)
		{
			nbf[fruit2] ++;
			nbf[fruit1] ++;
		}
		matrix[l1][c1] = EMPTY;
		fruitsfall(matrix, c1, rando, GAME);
		matrix[l2][c2] = EMPTY;
		fruitsfall(matrix, c2, rando, GAME);
		transSF(matrix, rand() % 4, rando);
	}

	// 1 FL and 1 FC are selected
	if (fruit1 == FLine && fruit2 == FColumn)
	{
		if (rando == TRUE)
		{
			nbf[fruit1] ++;
			nbf[fruit2] ++;
		}
		matrix[l1][c1] = EMPTY;
		fruitsfall(matrix, c1, rando, GAME);
		matrix[l2][c2] = EMPTY;
		fruitsfall(matrix, c2, rando, GAME);
		destroyline(matrix, l2, rando);
		destroycolumn(matrix, c1, rando);
	}

	// 1 FC and 1 FL are selected
	if (fruit1 == FColumn && fruit2 == FLine)
	{
		if (rando == TRUE)
		{
			nbf[fruit1] ++;
			nbf[fruit2] ++;
		}
		matrix[l1][c1] = EMPTY;
		fruitsfall(matrix, c1, rando, GAME);
		matrix[l2][c2] = EMPTY;
		fruitsfall(matrix, c2, rando, GAME);
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

gboolean checkgrid(int matrix[HEIGHT][WIDTH], int condition, int rando)
{
	for (int i = HEIGHT - 1; i >= 0; i--)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (checkaround(matrix, i, j, condition, rando) == TRUE)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

void quickSortList(double *arr, int first, int last)
{
	int i;
	int j;
	int pivot;
	double temp[8];
	if (first < last)
	{
		pivot = first;
		i = first;
		j = last;

		while (i < j)
		{
			while (arr[i*8 + 4] <= arr[pivot*8 + 4] && i < last)
				i++;
			while (arr[j*8 + 4] > arr[pivot*8 + 4])
				j--;
			if (i < j)
			{
				for(int a = 0; a < 8; a++)
				{
					temp[a] = arr[i*8 + a];
					arr[i*8+a] = arr[j*8+a];
					arr[j*8+a] = temp[a];
				}
			}
		}
		for(int a = 0; a < 8; a++)
			temp[a] = arr[pivot*8+a];
		for(int a = 0; a < 8; a++)
			arr[pivot*8+a] = arr[j*8+a];
		for(int a = 0; a < 8; a++)
			arr[j*8+a] = temp[a];
		quickSortList(arr, first, j-1);
		quickSortList(arr, j+1, last);

	}
}
// ----------------------------------------------------------------------------

int mat[8][8];
int ex_queue[4];
GtkBuilder* builder;
GtkBuilder* builder1;
GtkBuilder* builder2;
GtkBuilder* builder3;
GtkWindow* window;
GtkWindow* window2;
GtkImage* loading;
int moves = 10;
int playable = 0;
int aipl = 0;

//fonctions de jeu.c
//############################################
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

	bestscore = score;
	FILE *BS = fopen("bestscore", "w");

	fprintf(BS, "%lu",score);

	fclose(BS);
}
//############################################
gboolean up_mat()
{
	char* stri;

	for(int count = 7; count >= 0; count--)
	{
		for(int counter = 7; counter >= 0; counter--)
		{
			int longueur = 0;
			longueur = asprintf(&stri, "%d.%d", count, counter);
			if (longueur == 0)
			{
				printf("error: asprintf");
			}
			if (longueur == 0)
			{
				printf("error: asprintf\n");
			}
			GtkImage* image = GTK_IMAGE(gtk_builder_get_object(builder, stri));

			if(mat[count][counter] == 0)
			{
				gtk_image_set_from_file(image, "images/AC_pomme.png");
			}
			if(mat[count][counter] == 1)
			{
				gtk_image_set_from_file(image, "images/AC_peche.png");
			}
			if(mat[count][counter] == 2)
			{
				gtk_image_set_from_file(image, "images/AC_orange.bmp");
			}
			if(mat[count][counter] == 3)
			{
				gtk_image_set_from_file(image, "images/AC_poire.png");
			}
			if(mat[count][counter] == 4)
			{
				gtk_image_set_from_file(image, "images/AC_Special_Fruit.png");
			}
			if(mat[count][counter] == 5)
			{
				gtk_image_set_from_file(image, "images/AC_Fruit_Horizontal.png");
			}
			if(mat[count][counter] == 6)
			{
				gtk_image_set_from_file(image, "images/AC_Fruit_Vertical.png");
			}
			if(mat[count][counter] == EMPTY)
			{
				gtk_image_set_from_file(image, "images/EMPTY.png");
			}
		}
	}
	int longueur = 0;
	GtkLabel* gscore = GTK_LABEL(gtk_builder_get_object(builder, "score"));
	longueur = asprintf(&stri, "%lu", score);
	gtk_label_set_text(gscore, stri);
	GtkLabel* gmoves = GTK_LABEL(gtk_builder_get_object(builder, "moves_no"));
	longueur = asprintf(&stri, "%u", moves);
	gtk_label_set_text(gmoves, stri);
	GtkLabel* apple = GTK_LABEL(gtk_builder_get_object(builder, "pomme_no"));
	longueur = asprintf(&stri, "%lu", nbf[0]);
	gtk_label_set_text(apple , stri);
	GtkLabel* orange = GTK_LABEL(gtk_builder_get_object(builder, "peche_no"));
	longueur = asprintf(&stri, "%lu", nbf[1]);
	gtk_label_set_text(orange, stri);
	GtkLabel* peer = GTK_LABEL(gtk_builder_get_object(builder, "orange_no"));
	longueur = asprintf(&stri, "%lu", nbf[2]);
	gtk_label_set_text(peer, stri);
	GtkLabel* peach = GTK_LABEL(gtk_builder_get_object(builder, "poire_no"));
	longueur = asprintf(&stri, "%lu", nbf[3]);
	gtk_label_set_text(peach, stri);
	GtkLabel* sf = GTK_LABEL(gtk_builder_get_object(builder, "special_no"));
	longueur = asprintf(&stri, "%lu", nbf[4]);
	gtk_label_set_text(sf, stri);
	GtkLabel* fl = GTK_LABEL(gtk_builder_get_object(builder, "ligne_no"));
	longueur = asprintf(&stri, "%lu", nbf[5]);
	gtk_label_set_text(fl, stri);
	GtkLabel* fc = GTK_LABEL(gtk_builder_get_object(builder, "colone_no"));
	longueur = asprintf(&stri, "%lu", nbf[6]);
	gtk_label_set_text(fc, stri);
	if (longueur == 0)
	{
		printf("error: asprintf\n");
	}

	return TRUE;
}

void fruitsfall(int matrix[HEIGHT][WIDTH], size_t column, int rando, int cond)
{
	if (cond == GAME && rando == TRUE)
		up_mat();
	while (gtk_events_pending())
	{
		gtk_main_iteration();
		usleep(50000);
	}
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
	while (up >= 0 && matrix[up][column] != EMPTY)
	{
		matrix[down][column] = matrix[up][column];
		matrix[up][column] = EMPTY;
		up--;
		down--;
	}
	if (up < 0)
	{
		if (rando == TRUE)
		{
			while (down >= 0)
			{
				matrix[down][column] = rand() % 4;
				down--;
			}
		}
		if (cond == GAME && rando == TRUE)
			up_mat();
		while (gtk_events_pending())
		{
			gtk_main_iteration();
			usleep(50000);
		}
	}
	else
		fruitsfall(matrix, column, rando, cond);
}


void player_mode()
{
	playable = 1;
	gtk_widget_show((GtkWidget*)window);
	gtk_widget_hide((GtkWidget*)window2);
}

void afficher()
{
	getbestscore();

	char* stri;
	int longueur = 0;
	GtkLabel* sc_o = GTK_LABEL(gtk_builder_get_object(builder1, "score_ok"));
	gtk_widget_show((GtkWidget*)sc_o);
	GtkLabel* sc_l = GTK_LABEL(gtk_builder_get_object(builder1, "score_label"));
	gtk_widget_show((GtkWidget*)sc_l);
	GtkLabel* bs_o = GTK_LABEL(gtk_builder_get_object(builder1, "bestscore_ok"));
	gtk_widget_show((GtkWidget*)bs_o);
	longueur = asprintf(&stri, "%lu", score);
	gtk_label_set_text(sc_l, stri);
	GtkLabel* bs_l = GTK_LABEL(gtk_builder_get_object(builder1, "bestscore_label"));
	gtk_widget_show((GtkWidget*)bs_l);
	longueur = asprintf(&stri, "%lu", bestscore);
	gtk_label_set_text(bs_l, stri);
	if (longueur == 0)
	{
		printf("error: asprintf");
	}

	savebestscore(score);
}

void end_popup()
{
	gtk_init(NULL, NULL);

	builder1 = gtk_builder_new();
	GError* error = NULL;
	if (gtk_builder_add_from_file(builder1, "popup.glade", &error) == 0)
	{
		g_printerr("Error loading file: %s\n", error->message);
		g_clear_error(&error);
		return ;
	}

	GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(builder1, "window"));
	g_signal_connect(G_OBJECT(win), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	GtkButton* but = GTK_BUTTON(gtk_builder_get_object(builder1, "ok_button"));
	g_signal_connect(G_OBJECT(but), "clicked", G_CALLBACK(afficher), NULL);
	gtk_widget_show((GtkWidget*)win);

	gtk_main();
}

void train_exec()
{
	GtkLabel* training_l = GTK_LABEL(gtk_builder_get_object(builder3, "training_l"));
	GtkLabel* ok_l = GTK_LABEL(gtk_builder_get_object(builder3, "ok_l"));
	gtk_widget_show((GtkWidget*)training_l);
	sleep(45);
	gtk_widget_hide((GtkWidget*)training_l);
	gtk_widget_show((GtkWidget*)ok_l);
}

void train_popup()
{
	gtk_init(NULL, NULL);

	builder3 = gtk_builder_new();
	GError* error = NULL;
	if (gtk_builder_add_from_file(builder3, "training.glade", &error) == 0)
	{
		g_printerr("Error loading file: %s\n", error->message);
		g_clear_error(&error);
		return ;
	}

	GtkWindow* win = GTK_WINDOW(gtk_builder_get_object(builder3, "window"));
	g_signal_connect(G_OBJECT(win), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	GtkButton* but = GTK_BUTTON(gtk_builder_get_object(builder3, "train_ok"));
	g_signal_connect(G_OBJECT(but), "clicked", G_CALLBACK(train_exec), NULL);
	gtk_widget_show((GtkWidget*)win);

	gtk_main();
}

void check_moves()
{
	if (moves <= 0)
	{
		playable = 0;
		end_popup();
	}
}

void movement()
{
	int l1 = ex_queue[0];
	int c1 = ex_queue[1];
	int l2 = ex_queue[2];
	int c2 = ex_queue[3];
	//printf("%d,%d,%d,%d\n",l1,c1,l2,c2);
	if (playable == 1 && (((l1 == l2+1 || l1 == l2-1) && (c1 == c2)) || ((l1 == l2) && (c1 ==
						c2+1 || c1 == c2-1))))
	{
		moves --;

		playable = 0;
		gtk_widget_show((GtkWidget*)loading);
		cmd(mat, l1, c1, l2, c2, TRUE);
	}
	while(checkgrid(mat, GAME, TRUE) == TRUE)
	{
		continue;
	}
	playable = 1;
	gtk_widget_hide((GtkWidget*)loading);
	check_moves();
}

void ai_movement()
{
	int l1 = ex_queue[0];
	int c1 = ex_queue[1];
	int l2 = ex_queue[2];
	int c2 = ex_queue[3];
	//printf("%d,%d,%d,%d\n",l1,c1,l2,c2);
	if (aipl == 1 && (((l1 == l2+1 || l1 == l2-1) && (c1 == c2)) || ((l1 == l2) && (c1 ==
						c2+1 || c1 == c2-1))))
	{
		//moves --;
		//printf("%d\n", moves);
		//check_moves();

		aipl= 0;
		gtk_widget_show((GtkWidget*)loading);
		cmd(mat, l1, c1, l2, c2, TRUE);
	}
	while(checkgrid(mat, GAME, TRUE) == TRUE)
	{
		continue;
	}
	aipl = 1;
	gtk_widget_hide((GtkWidget*)loading);
	check_moves();
}

//neural network function
void bruteplays()
{
	int count = 0;
	double *info_list = save_info(mat, &count);
	quickSortList(info_list, 0, count-1);
	ex_queue[0] = info_list[(count-1)*8];
	ex_queue[1] = info_list[(count-1)*8+1];
	ex_queue[2] = info_list[(count-1)*8+2];
	ex_queue[3] = info_list[(count-1)*8+3];
	sleep(1);
	ai_movement();

	free(info_list);
}

unsigned long trainnn(int matrix[HEIGHT][WIDTH], void* nn)
{
	for(int i = 0; i < moves; i++)
	{
		int count = 0;
		double *L = save_info(matrix, &count);
		choose_move(nn, L, (size_t)count);
		double x1 = L[(count-1)*8 + 0];
		double y1 = L[(count-1)*8 + 1];
		double x2 = L[(count-1)*8 + 2];
		double y2 = L[(count-1)*8 + 3];
		cmd(matrix, x1, y1, x2, y2, TRUE);
	}
	return score;
}

void nnplays()
{
	int count = 0;
	Network* net = Load_Network(8);
	double *L = save_info(mat, &count);
	choose_move(net, L, (size_t)count);
	ex_queue[0] = L[(count-1)*8];
	ex_queue[1] = L[(count-1)*8+1];
	ex_queue[2] = L[(count-1)*8+2];
	ex_queue[3] = L[(count-1)*8+3];

	sleep(1);
	// Appel à la fonction pour relier avec le bouton
	ai_movement();

	free(L);
}

void auto_play()
{
	while(moves >= 0)
	{
		bruteplays();
		up_mat();
		moves--;
	}
}

void auto_nnplay()
{
	while(moves >= 0)
	{
		nnplays();
		up_mat();
		moves--;
	}
}

void AI_mode()
{
	aipl = 0;
	gtk_widget_show((GtkWidget*)window);
	gtk_widget_hide((GtkWidget*)window2);
	//sleep(1);
	auto_play();
}

void NN_mode()
{
	aipl = 0;
	gtk_widget_show((GtkWidget*)window);
	gtk_widget_hide((GtkWidget*)window2);
	auto_nnplay();
}

void fruit_click(int x,int y)
{
	int true_x;
	int true_y;
	true_x = y/100 - 1;
	true_y = x/100;
	printf("in matrice %d || %d\n", true_x, true_y);
	ex_queue[0] = true_x;
	ex_queue[1] = true_y;
}

static gboolean button_press_callback(GtkWidget * event_box, GdkEventButton * event, gpointer data)
{
	g_print("Event box clicked at coordinates %f || %f\n", event->x, event->y);
  if(event_box == NULL || data == NULL)
  {
    printf("...\n");
  }
	fruit_click(event->x, event->y);
	return TRUE;
}

void fruit_release(int x,int y)
{
	int true_x;
	int true_y;
	true_x = y/100 - 1;
	true_y = x/100;
	printf("in matrix %d || %d\n", true_x, true_y);
	ex_queue[2] = true_x;
	ex_queue[3] = true_y;
	if (!gtk_events_pending())
		movement();
}

static gboolean button_release_callback(GtkWidget * event_box, GdkEventButton * event, gpointer data)
{
	g_print("Event box released at coordinates %f || %f\n", event->x, event->y);
  if(event_box == NULL || data == NULL)
  {
    printf("...\n");
  }
	fruit_release(event->x, event->y);
	return TRUE;
}

void gen_mat()
{
	while(checkgrid(mat, START, TRUE) == TRUE)
	{
		continue;
	}
	char* stri;

	for(int count = 0; count < 8; count++)
	{
		for(int counter = 0; counter < 8; counter ++)
		{
			int longueur = 0;
			longueur = asprintf(&stri, "%d.%d", count, counter);
			if (longueur == 0)
			{
				printf("error: asprintf");
			}
			GtkImage* image = GTK_IMAGE(gtk_builder_get_object(builder, stri));

			if(mat[count][counter] == 0)
			{
				gtk_image_set_from_file(image, "images/AC_pomme.png");
			}
			if(mat[count][counter] == 1)
			{
				gtk_image_set_from_file(image, "images/AC_peche.png");
			}
			if(mat[count][counter] == 2)
			{
				gtk_image_set_from_file(image, "images/AC_orange.bmp");
			}
			if(mat[count][counter] == 3)
			{
				gtk_image_set_from_file(image, "images/AC_poire.png");
			}

		}
	}
}


// Main function.
int main ()
{
	// Initializes GTK.
	gtk_init(NULL, NULL);

	// Loads the UI description and builds the UI.
	// (Exits if an error occurs.)
	builder = gtk_builder_new();
	builder2 = gtk_builder_new();
	GError* error = NULL;
	if (gtk_builder_add_from_file(builder, "plain.glade", &error) == 0)
	{
		g_printerr("Error loading file: %s\n", error->message);
		g_clear_error(&error);
		return 1;
	}

	if (gtk_builder_add_from_file(builder2, "start.glade", &error) == 0)
	{
		g_printerr("Error loading file: %s\n", error->message);
		g_clear_error(&error);
		return 1;
	}

	GtkCssProvider *pCssProvider = NULL;
	pCssProvider = gtk_css_provider_new();
	GError *error1 = NULL;
	if(!gtk_css_provider_load_from_path(pCssProvider, "style.css", &error1))
	{
		g_error("%s\n", error1->message);
		exit(error1->code);
	}
	g_object_unref(pCssProvider);

	// Gets the widgets from plain.glade.
	window = GTK_WINDOW(gtk_builder_get_object(builder, "window"));
	loading = GTK_IMAGE(gtk_builder_get_object(builder, "loading"));
	GtkEventBox* event_board = GTK_EVENT_BOX(gtk_builder_get_object(builder, "event_board"));
	GtkGrid* board = GTK_GRID(gtk_builder_get_object(builder, "board"));

	// Connects signal handlers from plain.glade.
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(event_board), "button_press_event", G_CALLBACK(button_press_callback), board);
	g_signal_connect(G_OBJECT(event_board), "button_release_event", G_CALLBACK(button_release_callback), board);

	// Gets the widgets from start.glade.
	window2 = GTK_WINDOW(gtk_builder_get_object(builder2, "window"));
	GtkButton* player_b = GTK_BUTTON(gtk_builder_get_object(builder2, "player_b"));
	GtkButton* AI_b = GTK_BUTTON(gtk_builder_get_object(builder2, "AI_b"));
	GtkButton* NN_b = GTK_BUTTON(gtk_builder_get_object(builder2, "NN_b"));
	GtkButton* train_b = GTK_BUTTON(gtk_builder_get_object(builder2, "train_b"));

	// Connects signal handlers from start.glade.
	g_signal_connect(window2, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(player_b, "clicked", G_CALLBACK(player_mode), NULL);
	g_signal_connect(AI_b, "clicked", G_CALLBACK(AI_mode), NULL);
	g_signal_connect(NN_b, "clicked", G_CALLBACK(NN_mode), NULL);
	g_signal_connect(train_b, "clicked", G_CALLBACK(train_popup), NULL);

	gtk_widget_hide((GtkWidget*)window);
	gtk_widget_show((GtkWidget*)window2);

	generategrid(mat);
	gen_mat();

	// Runs the main loop.
	gtk_main();

	// Exits.
	return 0;
}
