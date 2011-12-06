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

int main (void)
{
  int np = 20;
  double *left = ml_alocar_M (np, 2);
  double *right = ml_alocar_M (np, 2);
  double *A = ml_alocar_M (np, 9);

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

  

  ml_free_M (left);
  ml_free_M (right);
  return 0;
}
