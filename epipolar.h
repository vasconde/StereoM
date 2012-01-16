/*
 * Interface da EPIPOLAR
 * Nome: epipolar.h
 * Autor: vasco conde
 *
 * Des: Conjunto de metodos para a determinacao
 * da matriz fundamental e dos parametros
 * da recta epipolar
 */

#ifndef _EPIPOLAR
#define _EPIPOLAR

/*passa para a memoria a matriz de um ficheiro ascii*/
void ep_readm (char *nfile, double *M, int m, int n);

/* Calculo da matriz fundamental
 *
 * Entrada: numero de pontos homologos, 
 * coordenadas imagem dos pontos da imagem esquerda,
 * coordenadas imagem dos pontos da imagem direita,
 * matriz fundamental (vazia)
 * saida: matriz fundamental
 */
void ep_fundamentalMatrix (int np, double* left, double* right, double* FM);

/*
 * Determinacao dos parametros da recta epipolar
 * Entrada: Matriz fundamental, coordenadas imagem de um
 * ponto na imagem esquerda, ponteiros para os parametros da recta
 * saida: parametros a,b e c da recta epipolar na imagem direita
 */
void ep_lepipolar (double* FM, double xl, double yl, double* a, double* b, double* c);


#endif
