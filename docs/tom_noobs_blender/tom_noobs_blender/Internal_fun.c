#include "Internal_fun.h"
#include <math.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

//This is the activation we chose, the sigmoid function.
double sigmoid(double x)
{
  return 1 / (1 + exp( -x));
}

//This is the sigmoid function's derviative.
double deriv_sigmoid(double x)
{
  double sig = sigmoid(x);
  return sig * (1 - sig);
}

//This is a function that generate a random number.
//Need to call "srand(time(NULL));" before using rand for the first time,
//time is in <time.h>.

double random_double()
{
  return  (double) rand() / (double) RAND_MAX - 0.5;
}

double power(double x)
{
  return x *x;
}

void __print_matrix(char s[], double *m, size_t rows, size_t cols)
{
    printf("%s =\n", s);
    for (size_t i = 0; i < rows; ++i)
    {
      for (size_t y = 0; y < cols; ++y)
      {
        printf("%8g ", m[i*cols+y]);
      }
      printf("\n");
    }
}

void transpose(double *m, size_t rows, size_t cols, double *r)
{
    for (size_t i = 0; i < rows; ++i)
    {
      for (size_t y = 0; y < cols; ++y)
      {
        r[y*rows+i] = m[i*cols+y];
      }
    }
}

void add(double *m1, double *m2, size_t rows, size_t cols, double *r)
{
    for (size_t i = 0; i < rows; ++i)
    {
      for (size_t y = 0; y < cols; ++y)
      {
        r[i*cols+y] = m1[i*cols+y] + m2[i*cols+y];
      }
    }
}

void mul(double *m1, double *m2, size_t r1, size_t c1, size_t c2, double *r)
{
    for (size_t i = 0; i < r1; ++i)
    {
      for (size_t y = 0; y < c2; ++y)
      {
        r[i*r1+y] = 0;
        for (size_t z = 0; z < c1; ++z)
        {
          r[i*c2+y] += m1[i*c1+z] * m2[z*c2+y];
        }
      }
    }
}

void add_val(double *m, double val, size_t rows, size_t cols, double *r)
{
  for (size_t i = 0; i < rows*cols; ++i)
  {
    r[i] = m[i] + val;
  }
}

void mul_val(double *m, double val, size_t rows, size_t cols, double *r)
{
  for (size_t i = 0; i < rows*cols; ++i)
  {
    r[i] = m[i] * val;
  }
}

void rand_matrice(size_t rows, size_t cols, double *r)
{
  for (size_t i = 0; i < rows*cols; ++i)
  {
    r[i] = random_double();
  }
}

double mse(size_t rows, double *y_true, double *y_pred)
{
  double res = 0;
  for (size_t i = 0; i < rows; ++i)
  { //pow(x) = x^2
    res += power(y_true[i]-y_pred[i]);
  }
  return res/rows;
}
void mse_prime(size_t rows, double *y_true, double *y_pred, double *out)
{
  for (size_t i = 0; i < rows; ++i)
  {
    out[i] = (2*(y_pred[i]-y_true[i]))/rows;
  }
}

void cpy(double *m1, double *m2, size_t rows, size_t cols)
{
  for (size_t i = 0; i < rows*cols; ++i)
  {
    m2[i] = m1[i];
  }
}

void mutate(double *matrix, size_t size)
{
	for (size_t i = 0; i < size; i ++)
			if (rand() % size == 0)
      //if(random_d(4) == 1)
				matrix[i] = random_double();
}

void __swap(double *a, double *b){
  double k = a[0];
  a[0] = b[0];
  b[0] = k;
}

int random_d(int max) {
  return ( rand() % max)+ 1;
}

void swap_i(int *list,int a, int b){
  int k = list[a];
  list[a] = list[b];
  list[b] = k;
}

int min(int *list, int size, int i){
  int minus = list[i];
  int minus_p = i;
  i++;
  for (; i < size; ++i)
  {
    if (list[i] < minus)
    {
      minus = list[i];
      minus_p = i;
    }
  }
  return minus_p;
}

void sort(int *list, int size){
  for (int i = 0; i < size-1; ++i)
  {
    swap_i(list, i, min(list, size, i));
  }
}

int in(int *list, int size, int k){
  for (int i = 0; i < size; ++i)
  {
    if (list[i] == k) return 1;
  }
  return 0;
}

size_t __min(size_t a, size_t b){
  if (a > b) return a;
  return b;
}

void cross(double *a, double *b, double *c, double *d, size_t a_size, size_t b_size, size_t c_size, int num){
  if (a_size < b_size)
  {
    errx(3,"Cross, a must be greater or equal at b");
  }
  int *bornes = (int*) calloc(num,sizeof(int));
  int k;
  for (int i = 0; i < num; ++i)
  {
    k = random_d(b_size)-1;
    while(in(bornes,num,k)){
      k = random_d(b_size)-1;
    }
    bornes[i] = k;
  }
  sort(bornes, num);
  k = 0;
  int n = 0;
  for (size_t i = 0; i < c_size; ++i)
  {
    if (i >= b_size)
    {
      {c[i] = a[i];d[i] = a[i];}
    }
    else
    {
      if (n < num && i == (size_t) bornes[n]) {k = !k; n+=1;}
      if (k) {c[i] = b[i];d[i] = a[i];}
      else   {c[i] = a[i];d[i] = b[i];}
    }
  }
  free(bornes);
}

double absolute(double num)
{
  if (num < 0)
  {
    return -num;
  }
  return num;;
}

void cpy_mat(double* mat_i, double* mat_r, size_t size)
{
  for (size_t i = 0; i < size; ++i) mat_r[i] = mat_i[i];
}
