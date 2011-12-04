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
#include <math.h>    /* c padrao */

#include "matrixlib.h"

#include "meastereo.h"

#define POSR(i,j) (j+i*3)

struct ms_param_photo {
  double x, y, f;     /** orientacao interna **/
  double X, Y, Z;  /** orientacao externa **/
  double omega, phi, kappa;
  double *R;
};

/*
 * Gestao de memoria
 */
p_ms_param_photo ms_alocar_param_photo (double x, double y, double f, 
					double X, double Y, double Z,
					double omega, double phi, double kappa)
{
  p_ms_param_photo param = (p_ms_param_photo)malloc(sizeof(struct ms_param_photo));
  double *R = ml_alocar_M (3, 3);

  param->x = x;
  param->y = y;
  param->f = f;

  param->X = X;
  param->Y = Y;
  param->Z = Z;

  param->omega = omega;
  param->phi = phi;
  param->kappa = kappa;

  ms_M_rot (R, omega, phi, kappa);

  param->R = R;

  return param;
}

void ms_free_param_photo (p_ms_param_photo param)
{
  free(param->R);
  free(param);
}


/* Calcula a matriz de rotacao */
void ms_M_rot (double *R, double omega, double phi, double kappa)
{
  double *Ro = ml_alocar_M_zeros (3, 3);
  double *Rp = ml_alocar_M_zeros (3, 3);
  double *Rk = ml_alocar_M_zeros (3, 3);

  double *Aux = ml_alocar_M (3,3);

  /* preenchimento das matrizes */
  ml_set_entry_M(Ro, 3, 0, 0, 1);
  ml_set_entry_M(Ro, 3, 1, 1, cos(omega));
  ml_set_entry_M(Ro, 3, 1, 2, -sin(omega));
  ml_set_entry_M(Ro, 3, 2, 1, sin(omega));
  ml_set_entry_M(Ro, 3, 2, 2, cos(omega));

  ml_set_entry_M(Rp, 3, 0, 0, cos(phi));
  ml_set_entry_M(Rp, 3, 0, 2, sin(phi));
  ml_set_entry_M(Rp, 3, 1, 1, 1);
  ml_set_entry_M(Rp, 3, 2, 0, -sin(phi));
  ml_set_entry_M(Rp, 3, 2, 2, cos(phi));

  ml_set_entry_M(Rk, 3, 0, 0, cos(kappa));
  ml_set_entry_M(Rk, 3, 0, 1, -sin(kappa));
  ml_set_entry_M(Rk, 3, 1, 0, sin(kappa));
  ml_set_entry_M(Rk, 3, 1, 1, cos(kappa));
  ml_set_entry_M(Rk, 3, 2, 2, 1);

  ml_AB (Ro, 3, 3, Rp, 3, 3, Aux);
  ml_AB (Aux, 3, 3, Rk, 3, 3, R);

  ml_free_M (Ro);
  ml_free_M (Rp);
  ml_free_M (Rk);
  ml_free_M (Aux);
}



double ms_ND(int op, p_ms_param_photo ph, double* meas)
{
  switch(op)
    {
    case 1:  /*Nx*/
      return ph->R[POSR(0, 0)] * (meas[0] - ph->x) + ph->R[POSR(0, 1)] * (meas[1] - ph->y) - ph->R[POSR(0, 2)] * ph->f;
      break;
    case 2: /*Ny*/
      return ph->R[POSR(1, 0)] * (meas[0] - ph->x) + ph->R[POSR(1, 1)] * (meas[1] - ph->y) - ph->R[POSR(1, 2)] * ph->f;
      break;
    default:  /*D*/
      return ph->R[POSR(2, 0)] * (meas[0] - ph->x) + ph->R[POSR(2, 1)] * (meas[1] - ph->y) - ph->R[POSR(2, 2)] * ph->f;
      break;
    }
}

double ms_kxy(int op, p_ms_param_photo ph, double* meas)
{
  switch (op)
    {
    case 1: /*Nx/D*/
      return ms_ND(1, ph, meas) / ms_ND(3, ph, meas);
      break;
    default: /*Ny/D*/
      return ms_ND(2, ph, meas) / ms_ND(3, ph, meas);
      break;
    }
}

/*transicao*/
void ms_firstAprox (p_ms_param_photo ph1, double* meas1, 
		    p_ms_param_photo ph2, double* meas2, double res)
{
  double X1, Y1, X2, Y2, Z;
  
  Z = (oExFoto2.Item2.getM() - oExFoto2.Item2.getH() * kxy(1, rot2, pFoto2, oIn) 
       + oExFoto1.Item2.getH() * kxy(1, rot1, pFoto1, oIn) - oExFoto1.Item2.getM()) /
    (kxy(1, rot1, pFoto1, oIn) - kxy(1, rot2, pFoto2, oIn));
  
  X1 = oExFoto1.Item2.getM() + (Z - oExFoto1.Item2.getH()) * kxy(1, rot1, pFoto1, oIn);
  
  Y1 = oExFoto1.Item2.getP() + (Z - oExFoto1.Item2.getH()) * kxy(2, rot1, pFoto1, oIn);
  
  X2 = oExFoto2.Item2.getM() + (Z - oExFoto2.Item2.getH()) * kxy(1, rot2, pFoto2, oIn);
  
  Y2 = oExFoto2.Item2.getP() + (Z - oExFoto2.Item2.getH()) * kxy(2, rot2, pFoto2, oIn);

  res[0] = (X1 + X2) / 2;
  
  res[1] = (Y1 + Y2) / 2;
  
  res[2] = Z;
  
  return pa;
  
}


void ms_photo2terrain (p_ms_param_photo ph1, double* meas1, 
		    p_ms_param_photo ph2, double* meas2, double res)
{

}
