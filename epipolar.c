/*
 * Implementacao da EPIPOLAR
 * Nome: epipolar.c
 * Autor: vasco conde
 *
 * Description: Conjunto de metodos para a determinacao dos parametros
 * da recta epipolar
 */

/*
 * BIBLIOTECAS AUXILIARES
 */
#include <stdio.h>   /* c padrao */
#include <stdlib.h>  /* c padrao */
#include <math.h>

#include "matrixlib.h"

#define POS(i,j,n) (j+i*n)

/*passa para a memoria a matriz de um ficheiro ascii*/
void readm (char *nfile, double *M, int m, int n)
{
  int i, j, k=0;

  FILE *f = fopen(nfile, "r");

  for(i=0; i < n; i++)
    for(j=0; j < m; j++)
      fscanf(f,"%lf", &M[k++]);
  fclose(f);
}
/*
int main (void)
{
  int np = 20;
  int i, j;
  double *left = ml_alocar_M (np, 2);
  double *right = ml_alocar_M (np, 2);
  double *A = ml_alocar_M (np, 9);

  double *U = ml_alocar_M (np, np);
  double *S = ml_alocar_M (np, 1);
  double *Vt = ml_alocar_M (9, 9);

  double x1, y1, x2, y2;
  

  readm ("dados/epipolar/left.txt", left, np, 2);
  readm ("dados/epipolar/right.txt", right, np, 2);

  for(i = 0; i < np; i++)
    {
      x1 = left[POS(i,0,2)];
      y1 = left[POS(i,1,2)];
      x2 = right[POS(i,0,2)];
      y2 = right[POS(i,1,2)];
      
      A[POS(i,0,9)] = x1*x2;
      A[POS(i,1,9)] = y1*x2;
      A[POS(i,2,9)] = x2;
      A[POS(i,3,9)] = x1*y2;
      A[POS(i,4,9)] = y1*y2;
      A[POS(i,5,9)] = y2;
      A[POS(i,6,9)] = x1;
      A[POS(i,7,9)] = y1;
      A[POS(i,8,9)] = 1.0;
    }


  ml_svd(A, np, 9, U, S, Vt);


  printf("---------------------\n");
  for(i=0; i < np; i++)
    {
      for(j=0; j < np; j++)
	printf("%lf ", U[POS(i,j,np)]);
      printf("\n");
    }
  printf("---------------------\n");
  for(i=0; i < np; i++)
    {
      for(j=0; j < 1; j++)
	printf("%lf ", S[POS(i,j,1)]);
      printf("\n");
    }
  printf("......................\n");
  for(i=0; i < 9; i++)
    {
      for(j=0; j < 9; j++)
	printf("%lf ", Vt[POS(i,j,9)]);
      printf("\n");
    }

  ml_free_M (left);
  ml_free_M (right);
  ml_free_M (A);
  ml_free_M (U);
  ml_free_M (S);
  ml_free_M (Vt);
  return 0;
}
*/

 
int main (void)
{
  int i, j;
  const int m  = 6;
  const int n  = 5;
  double A[] = {8.79,9.93,9.83,5.45,3.16,
		6.11,6.91,5.04,-0.27,7.98,
		-9.15,-7.93,4.86,4.85,3.01,
		9.57,1.64,8.83,0.74,5.80,
		-3.49,4.02,9.80,10.00,4.27,
		9.84,0.15,-8.99,-6.02,-5.31};
  
  double U[m*m];
  double S[n*1];
  double Vt[n*n];
  
  /*
  double *U = (double*)malloc(m*m*sizeof(double));
  double *S = (double*)malloc(n*1*sizeof(double));
  double *Vt = (double*)malloc(n*n*sizeof(double));
  */

  printf("A ---------------------\n");
  for(i=0; i < m; i++)
    {
      for(j=0; j < n; j++)
	printf("%lf ", A[POS(i,j,n)]);
      printf("\n");
    }

  ml_svd(A, m, n, U, S, Vt);

  printf("\nU ---------------------\n");
  for(i=0; i < m; i++)
    {
      for(j=0; j < m; j++)
	printf("%lf ", U[POS(i,j,m)]);
      printf("\n");
    }

 printf("S ---------------------\n");
  for(i=0; i < n; i++)
    {
      for(j=0; j < 1; j++)
	printf("%lf ", S[POS(i,j,1)]);
      printf("\n");
    }

  printf("V ---------------------\n");
  for(i=0; i < n; i++)
    {
      for(j=0; j < n; j++)
	printf("%lf ", Vt[POS(i,j,n)]);
      printf("\n");
    }


  return 0;
}

