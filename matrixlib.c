/*
 * Implementacao da MATRIXLIB
 * Nome: matrixlib.c
 * Autor: vasco conde
 *
 * Description: - Gestao de memoria
 *                 + alocacao
 *                 + libertacao
 *              - Operacoes com matrizes:
 *                 + copia de A para B
 *                 + soma e subtracao
 *                 + multiplicao por um escalar
 *                 + multiplicacao (com e sem transposicao);
 *                 + inversao
 *                 + transposicao
 * 
 * Dependencias: - cblaslib
 *               - lapacklib
 */

/*
 * BIBLIOTECAS AUXILIARES
 */
#include <stdlib.h>  /* c padrao */
#include <math.h>    /* c padrao */
#include <cblas.h>   /* c BLAS   */

#include "matrixlib.h"

/*** GESTAO DE MEMORIA  ***/

/*
 * Alocacao de uma matriz
 */
double *ml_alocar_M (int m, int n)
{
  double *M = (double *)calloc(m*n,sizeof(double));
  return M;
}

/*
 * Alocacao de uma matriz a zeros
 */
double *ml_alocar_M_zeros (int m, int n)
{
  double *M = ml_alocar_M (m, n);
  int num = m*n;
  int i;

  for(i = 0; i < num; i++)
    M[i] = 0;

  return M;
}

/*
 * Alocacao de uma matriz identidade
 */
double *ml_alocar_M_I(int n)
{
  double *M = ml_alocar_M_zeros (n,n);
  int i;

  for(i=0; i<n; i++)
    M[i+(n*i)] = 1.0;

  return M;
}

/*
 * altera um valor pontual valor da matriz M
 */
void ml_set_entry_M(double *M, int n, int i, int j, double val)
{
  M[j+i*n] = val;
}

/*
 * retorna um valor pontual da matriz M
 */
double ml_get_entry_M(const double *M, int n, int i, int j)
{
  return M[j+i*n];
}

/* 
 * Libertar matriz da memoria
 */
void ml_free_M (double *M)
{
  free(M);
}

/*** OPERACOES DE MATRIZES ***/

/*
 * Copiar de A para B
 */
void ml_AeqB (double *A, double *B, int m, int n)
{
  int i;
  int dim = m * n;
  for (i = 0; i < dim; i++)
    {
      A[i] = B[i];
    }
}

/*
 * Multiplicacao de duas Matrizes usando a BLAS
 */
double *ml_AB (double *A, const int ma, const int na, double *B, const int mb, const int nb, double *C)
{
  if(na != mb)
    return NULL;

  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, ma, nb, na, 1.0, A, na, B, nb, 0.0, C, nb);

  return C;
}

/*
 * Multiplicacao de matrizes com opcao de transposicao
 */
double *ml_ABtt (double *A, int ma, int na, int ta, double *B, int mb, int nb, int tb, double *C)
{
  int aux;
  int saltoa = na, saltob = nb, saltoc = nb;

  enum CBLAS_ORDER transposeA = CblasNoTrans;
  enum CBLAS_ORDER transposeB = CblasNoTrans;

  if(ta != 0)
    {
      transposeA = CblasTrans;
      aux = ma;
      ma = na;
      na = aux;
    }
  if(tb != 0)
    {
      saltoc = mb;
      transposeB = CblasTrans;
      aux = mb;
      mb = nb;
      nb = aux;
    }

  cblas_dgemm(CblasRowMajor, transposeA, transposeB, ma, nb, na, 1.0, A, saltoa, B, saltob, 0.0, C, saltoc);

  return C;
}

/*
 * Inversa da matriz M usando a LAPACK
 */
/* LU decomoposition of a general matrix - from LAPACK*/
void dgetrf_(int* M, int *N, double* A, int* lda, int* IPIV, int* INFO);
  
/* generate inverse of a matrix given its LU decomposition - from LAPACK*/
void dgetri_(int* N, double* A, int* lda, int* IPIV, double* WORK, int* lwork, int* INFO);

void ml_invM(double* A, int N)
{
  /*int *IPIV = new int[N+1];*/
    int *IPIV = (int *)calloc(N+1,sizeof(int));
    int LWORK = N*N;
    /*double *WORK = new double[LWORK];*/
    double *WORK = (double *)calloc(LWORK,sizeof(double));
    int INFO;

    dgetrf_(&N,&N,A,&N,IPIV,&INFO);
    dgetri_(&N,A,&N,IPIV,WORK,&LWORK,&INFO);

    free(IPIV);
    free(WORK);
}

/*
 * transposta
 */
void ml_Mt (double *M, int m, int n, double *Mt)
{
  int i, j;
  for(i=0; i < m; i++)
    for(j=0; j < n; j++)
      Mt[i+j*m] = M[j+i*n];
}

/*
 * Multiplicacao de uma matriz por um escalar
 */
void ml_aM (double a, double *M, int m, int n)
{
  int i;
  int dim = m*n;
  for(i=0; i < dim; i++)
    M[i] *= a;
}

/*
 * Somar(op == 0) e subtrair (op != 0) duas matrizes
 */
void ml_AmmB (int op, double *A, double *B, int m, int n, double *C)
{
  int i;
  int dim = m*n;
  if(op == 0)
    for(i=0; i < dim; i++)
      C[i] = A[i] + B[i];
  else
    for(i=0; i < dim; i++)
      C[i] = A[i] - B[i];
}
