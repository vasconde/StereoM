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

/*** TEMPORARIAS ***/

/* escrever no ecra a matriz M */
void showm (double *M, int m, int n)
{
  int i, j;
  for(i = 0; i < m; i++)
    {
      for(j = 0; j < n; j++)
	printf("%.4lf ", M[j+i*n]);
      printf("\n");
    }
}

int read_ascii_param (char* filen, double* T, double* Opk)
{
  FILE *f = fopen(filen, "r");

  if(f == NULL)
    {
      printf("INFO: erro na leitura do ficheiro de parametros.\n");
      return 1;
    }

  fscanf(f,"%lf %lf %lf", &T[0], &T[1], &T[2]);
  fscanf(f,"%lf %lf %lf", &Opk[0], &Opk[1], &Opk[2]);

  fclose(f);
  return 0;
}

int main (void)
{
  char fl[] = "dados/par/paramL.txt";
  char fr[] = "dados/par/paramR.txt";

  double* Tl = ml_alocar_M (3, 1);
  double* Opkl = ml_alocar_M (3, 1);
  double* Tr = ml_alocar_M (3, 1);
  double* Opkr = ml_alocar_M (3, 1);

  read_ascii_param (fl, Tl, Opkl);
  read_ascii_param (fr, Tr, Opkr);

  printf("\nL\n");
  showm (Tl, 3, 1);
  showm (Opkl, 3, 1);
  printf("\nR\n");
  showm (Tr, 3, 1);
  showm (Opkr, 3, 1);

  ml_free_M (Tl);
  ml_free_M (Opkl);
  ml_free_M (Tr);
  ml_free_M (Opkr);

  return 0;
}
