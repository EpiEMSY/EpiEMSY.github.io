#include<stddef.h>
#ifndef INTERNAL_FUN_H
#define INTERNAL_FUN_H

double sigmoid(double x);
double deriv_sigmoid(double x);
double random_double();
double power(double x);

void __print_matrix(char s[], double *m, size_t rows, size_t cols);

//This function return the transpose of the matrix.
void transpose(double *m, size_t rows, size_t cols, double *r);

//This function add two matrixes of the same size.
void add(double *m1, double *m2, size_t rows, size_t cols, double *r);

//This function do a matrix multiplication.
void mul(double *m1, double *m2, size_t r1, size_t c1, size_t c2,
  double *r);

//This function add a double val to every cell of the matrix.
void add_val(double *m, double val, size_t rows, size_t cols, double *r);

//This function multiply every cell to a double val.
void mul_val(double *m, double val, size_t rows, size_t cols, double *r);

//This function generate a matrix of size height, width with random double in
//each cell.
void rand_matrice(size_t rows, size_t cols, double *r);

//This is the loss function, the mean squarred error function that return a
//double.
double mse(size_t rows, double *y_true, double *y_pred);

//This is the derivation of the MSE function.
void mse_prime(size_t rows, double *y_true, double *y_pred, double *out);

//This copy a full matrice from A to B
void cpy(double *m1, double *m2, size_t rows, size_t cols);

// This function mutate randomly
// For every cells:
// If the random number generated between 0 and the size is equal we mutate
// meaning the cell's content is now a new random double
void mutate(double *matrix, size_t size);

//swap value of pointer a and b (a = *b and b = *a)
void __swap(double *a, double *b);

//cross 2 times between 2 mat of diferents size
void cross(double *a, double *b, double *c, double *d, size_t a_size, size_t b_size, size_t c_size, int num);

//random int from 1 to max (included)
int random_d(int max);

//sort a pointer list
void sort(int *list, int size);

//return 1 if k in list
int in(int *list, int size, int k);

double absolute(double num);

void cpy_mat(double* mat_i, double* mat_r, size_t size);

#endif
