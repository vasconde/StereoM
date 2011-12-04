/*
 * Implementacao da MAEASTEREO
 * Nome: maeastereo.c
 * Autor: vasco conde
 */

/*
 * BIBLIOTECAS AUXILIARES
 */
#include <stdio.h>   /* c padrao */
#include <stdlib.h>  /* c padrao */

#include "maeastereo.h"

struct ms_param_photo {
  double x,y,f;     /** orientacao interna **/
  double X, Y, Z;  /** orientacao externa **/
  double omega, phi, kappa;
};

double *photo2terrain (p_ms_param_photo ph1, double* meas1, 
		       p_ms_param_photo ph2, double* meas2)
{
  double res[2];

  res[0] = 1;

  res[1] = 1;
}
