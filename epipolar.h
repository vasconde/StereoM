/*
 * Interface da EPIPOLAR
 * Nome: epipolar.h
 * Autor: vasco conde
 */

#ifndef _EPIPOLAR
#define _EPIPOLAR

/*passa para a memoria a matriz de um ficheiro ascii*/
void ep_readm (char *nfile, double *M, int m, int n);

void ep_fundamentalMatrix (int np, double* left, double* right, double* FM);

void ep_lepipolar (double* FM, double xl, double yl, double* a, double* b, double* c);


#endif
