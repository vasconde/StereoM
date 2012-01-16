/*
 * Implementacao da EPIPOLAR
 * Nome: epipolar.c
 * Autor: vasco conde
 *
 * Des: Conjunto de metodos para a determinacao
 * da matriz fundamental e dos parametros
 * da recta epipolar
 */

/*
 * BIBLIOTECAS AUXILIARES
 */
/*c padrao*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*stereom*/
#include "matrixlib.h"

/*MACRO*/
#define POS(i,j,n) (j+i*n)

/*passa para a memoria a matriz de um ficheiro ascii*/
void ep_readm (char *nfile, double *M, int m, int n)
{
  int i, j, k=0;

  FILE *f = fopen(nfile, "r");

  for(i=0; i < n; i++)
    for(j=0; j < m; j++)
      fscanf(f,"%lf", &M[k++]);
  fclose(f);
}

/* Calculo da matriz fundamental
 *
 * Entrada: numero de pontos homologos, 
 * coordenadas imagem dos pontos da imagem esquerda,
 * coordenadas imagem dos pontos da imagem direita,
 * matriz fundamental (vazia)
 * saida: matriz fundamental
 */
void ep_fundamentalMatrix (int np, double* left, double* right, double* FM)
{
  int i; /*ciclo for*/

  double *A = ml_alocar_M (np, 9);

  double *U = ml_alocar_M (np, np);
  double *S = ml_alocar_M (np, 1);
  double *V = ml_alocar_M (9, 9);

  double *F = ml_alocar_M (3, 3);

  double *FU = ml_alocar_M (3, 3);
  double *FS_ = ml_alocar_M (3, 1);
  double *FV = ml_alocar_M (3, 3);
  double *FS = ml_alocar_M_I (3);
  double *FSn = ml_alocar_M_I (3);
  
  double *Aux = ml_alocar_M (3, 3);

  double x1, y1, x2, y2;
  
  /*Algoritmo dos 8 pontos*/

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


  ml_svd(A, np, 9, U, S, V);

  for(i=0; i < 9; i++)
    F[i] = V[POS(i,8,9)];

  ml_svd(F, 3, 3, FU, FS_, FV);

  FS[POS(0,0,3)] = FS_[0];
  FS[POS(1,1,3)] = FS_[1];
  FS[POS(2,2,3)] = FS_[2];

  ml_AeqB (FSn, FS, 3, 3);
  FSn[POS(2,2,3)] = 0.0;

  ml_ABtt (FU, 3, 3, 0, FSn, 3, 3, 0, Aux);
  ml_ABtt (Aux, 3, 3, 0, FV, 3, 3, 1, FM);

  ml_free_M (A);
  ml_free_M (U);
  ml_free_M (S);
  ml_free_M (V);
  ml_free_M (F);
  ml_free_M (FU);
  ml_free_M (FS_);
  ml_free_M (FV);
  ml_free_M (FS);
  ml_free_M (FSn);  
}

/*
 * Determinacao dos parametros da recta epipolar
 * Entrada: Matriz fundamental, coordenadas imagem de um
 * ponto na imagem esquerda, ponteiros para os parametros da recta
 * saida: parametros a,b e c da recta epipolar na imagem direita
 */
void ep_lepipolar (double* FM, double xl, double yl, double* a, double* b, double* c)
{
    double *Pl = ml_alocar_M (3, 1);
    double *u = ml_alocar_M (3, 1);

    Pl[0] = xl;
    Pl[1] = yl;
    Pl[2] = 1.0;

    ml_ABtt (FM, 3, 3, 0, Pl, 3, 1, 0, u);

    *a = u[0];
    *b = u[1];
    *c = u[2];

    ml_free_M (Pl);
    ml_free_M (u);
}
