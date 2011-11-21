/*
 * Interface da MATRIXLIB
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

#ifndef _MATRIXLIB
#define _MATRIXLIB

/*** GESTAO DE MEMORIA  ***/

/*
 * Alocacao de uma matriz
 */
double *ml_alocar_M (int m, int n);

/*
 * Alocacao de uma matriz a zeros
 */
double *ml_alocar_M_zeros (int m, int n);

/*
 * Alocacao de uma matriz identidade
 */
double *ml_alocar_M_I(int n);

/*
 * altera um valor pontual valor da matriz M
 */
void ml_set_entry_M(double *M, int n, int i, int j, double val);

/*
 * retorna um valor pontual da matriz M
 */
double ml_get_entry_M(const double *M, int n, int i, int j);

/* 
 * Libertar matriz da memoria
 */
void ml_free_M (double *M);

/*** OPERACOES DE MATRIZES ***/

/*
 * Copiar de A para B
 */
void ml_AeqB (double *A, double *B, int m, int n);

/*
 * Multiplicacao de duas Matrizes usando a BLAS
 */
double *ml_AB (double *A, const int ma, const int na, double *B, const int mb, const int nb, double *C);

/*
 * Multiplicacao de matrizes com opcao de transposicao
 */
double *ml_ABtt (double *A, int ma, int na, int ta, double *B, int mb, int nb, int tb, double *C);

/*
 * Inversa da matriz M usando a LAPACK
 */
void ml_invM(double* A, int N);

/*
 * transposta
 */
void ml_Mt (double *M, int m, int n, double *Mt);

/*
 * Multiplicacao de uma matriz por um escalar
 */
void ml_aM (double a, double *M, int m, int n);

/*
 * Somar(op != 0) e subtrair (op = 0) duas matrizes
 */
void ml_AmmB (int op, double *A, double *B, int m, int n, double *C);

#endif
