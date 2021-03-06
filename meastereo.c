/*
 * Implementacao da MAEASTEREO
 * Nome: maeastereo.c
 * Autor: vasco conde
 *
 * Des: Modulo dedicado ah determinacao de 
 * coordenadas objecto a partir de coordenadas
 * imagem de pontos homologos em pares de imagens
 * em condicoes de estereoscopia a partir das equacoes
 * de colinearida.
 */

/*
 * BIBLIOTECAS AUXILIARES
 */
/* c padrao */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*stereom*/
#include "matrixlib.h"

#include "meastereo.h"

/*MACROS*/
#define POSR(i,j) (j+i*3)
#define POS(i,j,n) (j+i*n)


/* estrutura de dados para parametros 
 * de orientacao interna e externa
 */
struct ms_param_photo {
  double x, y, f;     /** orientacao interna **/
  
  /** orientacao externa **/
  double X, Y, Z;     
  double omega, phi, kappa;
  
  /*matriz de rotacao*/
  int op; /*1 = omega phi kappa; 2 = pok*/
  double *R;
};


/* alocacao de memoria para 
 * a estrutura dos parametros
 */
p_ms_param_photo ms_alocar_param_photo (double x, double y, double f, 
					double X, double Y, double Z, int op,
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

  ms_M_rot (op, R, omega, phi, kappa);

  param->R = R;

  return param;
}

/*libertacao de memoria para 
 *a estrutura dos parametros
 */
void ms_free_param_photo (p_ms_param_photo param)
{
  free(param->R);
  free(param);
}


/* Calcula a matriz de rotacao */
void ms_M_rot (int op, double *R, double omega, double phi, double kappa)
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

  switch(op)
    {
    case 1: /*omega phi kappa */
      ml_AB (Ro, 3, 3, Rp, 3, 3, Aux);
      ml_AB (Aux, 3, 3, Rk, 3, 3, R);
      break;
    case 2: /*phi omega kappa */
      ml_AB (Rp, 3, 3, Ro, 3, 3, Aux);
      ml_AB (Aux, 3, 3, Rk, 3, 3, R);
      break;
    }

  ml_free_M (Ro);
  ml_free_M (Rp);
  ml_free_M (Rk);
  ml_free_M (Aux);
}


/* Determina o valor do numerador (Nx ou Ny)
 * ou denominador (D) das equacoes de 
 * colinearida
 */
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

/* Determina valor de Nx/D ou Ny/D
 * das equacoes de colinearidade
 */
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

/* Determina uma primeira aproximacao para as coordenadas
 * objecto, para utilizacao no ajustamento
 * entrada: parametros e coordenadas imagem
 * saida: 1a aproximacao as coordenadas terreno
 */
void ms_firstAprox (p_ms_param_photo ph1, double* meas1, 
		    p_ms_param_photo ph2, double* meas2, double* res)
{
  double X1, Y1, X2, Y2, Z;
  
  Z = (ph2->X - ph2->Z * ms_kxy(1, ph2, meas2) 
       + ph1->Z * ms_kxy(1, ph1, meas1) - ph1->X) /
    (ms_kxy(1, ph1, meas1) - ms_kxy(1, ph2, meas2));
  
  X1 = ph1->X + (Z - ph1->Z) * ms_kxy(1, ph1, meas1);
  
  Y1 = ph1->Y + (Z - ph1->Z) * ms_kxy(2, ph1, meas1);
  
  X2 = ph2->X + (Z - ph2->Z) * ms_kxy(1, ph2, meas2);
  
  Y2 = ph2->Y + (Z - ph2->Z) * ms_kxy(2, ph2, meas2);

  res[0] = (X1 + X2) / 2;
  
  res[1] = (Y1 + Y2) / 2;
  
  res[2] = Z;  
}

/* Determinacao das coordenadas objecto de um ponto medido
 * em duas imagens recorrendo a um ajustamento por minimos
 * quadrados das equacoes de colinearidade
 * Entrada: parametros de orientacao da imagem 1 e coordenadas
 * imagem do ponto na imagem 1;
 * parametros de orientacao da imagem 2 e coordenadas
 * imagem do ponto na imagem 2;
 * ponteiro para receber a proposta
 */
void ms_photo2terrain (p_ms_param_photo ph1, double* meas1, 
		       p_ms_param_photo ph2, double* meas2, double* res)
{
  int i;
  double var0 = 1;

  /*primeira aproximacao aos parametros*/
  ms_firstAprox (ph1, meas1, ph2, meas2, res);

  /*passa param para vector*/
  double *param = ml_alocar_M (3, 1);

  param[POS(0, 0, 1)] = res[0];
  param[POS(1, 0, 1)] = res[1];
  param[POS(2, 0, 1)] = res[2];

  /*vector de observacoes*/
  double *l = ml_alocar_M (6, 1);

  l[POS(0, 0, 1)] = meas1[0];
  l[POS(1, 0, 1)] = meas1[1];
  l[POS(2, 0, 1)] = meas1[2];
  l[POS(3, 0, 1)] = meas2[0];
  l[POS(4, 0, 1)] = meas2[1];
  l[POS(5, 0, 1)] = meas2[2];

  /* Matriz pesos (identidade) */
  double *Pesos = ml_alocar_M_I(6);
  double *Pesos_inv = ml_alocar_M (6, 6); 
  double *Cl = ml_alocar_M_I(6);

  /*primeira matriz de configuracao df/dx*/
  double *A = ml_alocar_M (4, 3);

  /*segunda matriz de configuracao df/dl*/
  double *B = ml_alocar_M (4, 6);

  /* Vector de fecho */
  double *fecho = ml_alocar_M (4, 1);

  /* vector das correcoes */
  double *delta = ml_alocar_M (3, 1);

  double *A_t = ml_alocar_M (3, 4); 
  double *B_t = ml_alocar_M (6, 4);

  double *M = ml_alocar_M (4, 4);
  double *M_inv = ml_alocar_M (4, 4);

  double *N = ml_alocar_M (3, 3);
  double *N_inv = ml_alocar_M (3, 3);

  double *Aux = ml_alocar_M(4, 6);
  double *Aux2 = ml_alocar_M(3, 4);
  double *Aux3 = ml_alocar_M(3, 4);
  double *Aux4 = ml_alocar_M(3, 1);


  /* ciclo */

  i = 0;
  do
    {
      /* Matriz A */

      A[POS(0, 0, 3)] = -1; 
      A[POS(0, 1, 3)] = 0; 
      A[POS(0, 2, 3)] = ms_kxy(1, ph1, meas1);
      A[POS(1, 0, 3)] = 0; 
      A[POS(1, 1, 3)] = -1; 
      A[POS(1, 2, 3)] = ms_kxy(2, ph1, meas1);

      A[POS(2, 0, 3)] = -1; 
      A[POS(2, 1, 3)] = 0; 
      A[POS(2, 2, 3)] = ms_kxy(1, ph2, meas2);
      A[POS(3, 0, 3)] = 0; 
      A[POS(3, 1, 3)] = -1; 
      A[POS(3, 2, 3)] = ms_kxy(2, ph2, meas2);

      /* Matriz B */

      B[POS(0, 0, 6)] = (param[POS(2, 0, 1)] - ph1->Z) * (ph1->R[POSR(0, 0)] * ms_ND(3, ph1, meas1) - ph1->R[POSR(2, 0)] * ms_ND(1, ph1, meas1)) / pow(ms_ND(3, ph1, meas1), 2); 
      B[POS(0, 1, 6)] = (param[POS(2, 0, 1)] - ph1->Z) * (ph1->R[POSR(0, 1)] * ms_ND(3, ph1, meas1) - ph1->R[POSR(2, 1)] * ms_ND(1, ph1, meas1)) / pow(ms_ND(3, ph1, meas1), 2); 
      B[POS(0, 2, 6)] = 0.0; 
      B[POS(0, 3, 6)] = 0.0; 
      B[POS(0, 4, 6)] = 0.0; 
      B[POS(0, 5, 6)] = 0.0;
      
      B[POS(1, 0, 6)] = (param[POS(2, 0, 1)] - ph1->Z) * (ph1->R[POSR(1, 0)] * ms_ND(3, ph1, meas1) - ph1->R[POSR(2, 0)] * ms_ND(2, ph1, meas1)) / pow(ms_ND(3, ph1, meas1), 2); 
      B[POS(1, 1, 6)] = (param[POS(2, 0, 1)] - ph1->Z) * (ph1->R[POSR(1, 1)] * ms_ND(3, ph1, meas1) - ph1->R[POSR(2, 1)] * ms_ND(2, ph1, meas1)) / pow(ms_ND(3, ph1, meas1), 2); 
      B[POS(1, 2, 6)] = 0.0; 
      B[POS(1, 3, 6)] = 0.0; 
      B[POS(1, 4, 6)] = 0.0; 
      B[POS(1, 5, 6)] = 0.0;

      B[POS(2, 0, 6)] = 0.0; 
      B[POS(2, 1, 6)] = 0.0; 
      B[POS(2, 2, 6)] = 0.0; 
      B[POS(2, 3, 6)] = (param[POS(2, 0, 1)] - ph2->Z) * (ph2->R[POSR(0, 0)] * ms_ND(3, ph2, meas2) - ph2->R[POSR(2, 0)] * ms_ND(1, ph2, meas2)) / pow(ms_ND(3, ph2, meas2), 2); 
      B[POS(2, 4, 6)] = (param[POS(2, 0, 1)] - ph2->Z) * (ph2->R[POSR(0, 1)] * ms_ND(3, ph2, meas2) - ph2->R[POSR(2, 1)] * ms_ND(1, ph2, meas2)) / pow(ms_ND(3, ph2, meas2), 2); 
      B[POS(2, 5, 6)] = 0.0;
      
      B[POS(3, 0, 6)] = 0.0; 
      B[POS(3, 1, 6)] = 0.0; 
      B[POS(3, 2, 6)] = 0.0; 
      B[POS(3, 3, 6)] = (param[POS(2, 0, 1)] - ph2->Z) * (ph2->R[POSR(1, 0)] * ms_ND(3, ph2, meas2) - ph2->R[POSR(2, 0)] * ms_ND(2, ph2, meas2)) / pow(ms_ND(3, ph2, meas2), 2); 
      B[POS(3, 4, 6)] = (param[POS(2, 0, 1)] - ph2->Z) * (ph2->R[POSR(1, 1)] * ms_ND(3, ph2, meas2) - ph2->R[POSR(2, 1)] * ms_ND(2, ph2, meas2)) / pow(ms_ND(3, ph2, meas2), 2); 
      B[POS(3, 5, 6)] = 0.0;

      /* vector de fecho */
      fecho[POS(0, 0, 1)] = ph1->X + (param[POS(2, 0, 1)] - ph1->Z) * ms_kxy(1, ph1, meas1) - param[POS(0, 0, 1)];
      fecho[POS(1, 0, 1)] = ph1->Y + (param[POS(2, 0, 1)] - ph1->Z) * ms_kxy(2, ph1, meas1) - param[POS(1, 0, 1)];
      fecho[POS(2, 0, 1)] = ph2->X + (param[POS(2, 0, 1)] - ph2->Z) * ms_kxy(1, ph2, meas2) - param[POS(0, 0, 1)];
      fecho[POS(3, 0, 1)] = ph2->Y + (param[POS(2, 0, 1)] - ph2->Z) * ms_kxy(2, ph2, meas2) - param[POS(1, 0, 1)];


      /*definicao dos pesos*/
      ml_AeqB (Pesos, Cl, 6, 6);   /*Pl = Cl*/
      ml_invM(Pesos, 6);           /*Pl = inv(Pl)*/
      ml_aM (var0, Pesos, 6, 6);   /*Pl = var*Pl*/

      /*Inversao de Pl*/
      ml_AeqB (Pesos_inv, Pesos, 6, 6);
      ml_invM(Pesos_inv, 6);

      /*M = B inv(Pl) * B' */
      ml_ABtt (B, 4, 6, 0, Pesos_inv, 6, 6, 0, Aux);
      ml_ABtt (Aux, 4, 6, 0, B, 4, 6, 1, M);

      /*Inversao de M*/
      ml_AeqB (M_inv, M, 4, 4);
      ml_invM (M_inv, 4);

      /*N = A_t * M_inv * A;*/
      ml_ABtt (A, 4, 3, 1, M_inv, 4, 4, 0, Aux2);
      ml_ABtt (Aux2, 3, 4, 0, A, 4, 3, 0, N);

      /*Inversao de N*/
      ml_AeqB (N_inv, N, 3, 3);
      ml_invM (N_inv, 3);

      ml_aM (-1.0, N_inv, 3, 3);
      ml_ABtt (/*-*/N_inv, 3, 3, 0, A, 4, 3, 1, Aux2);
      ml_ABtt (Aux2, 3, 4, 0, M_inv, 4, 4, 0, Aux3);
      ml_ABtt (Aux3, 3, 4, 0, fecho, 4, 1, 0, delta);

      /*param = param + delta;*/
      ml_AeqB (Aux4, param, 3, 1);
      ml_AmmB (0, Aux4, delta, 3, 1, param);

      i++;

    } while (sqrt(pow(delta[0],2) + pow(delta[1],2) + pow(delta[2],2)) >= 0.000001 && i < 10);

  /*fim de ciclo*/
  
  res[0] = param[0];
  res[1] = param[1];
  res[2] = param[2];
  
  /*
  printf("%d\n", i);
  printf("M = %lf\n", res[0]);
  printf("P = %lf\n", res[1]);
  printf("H = %lf\n", res[2]);
  */  

  
  ml_free_M (param);
  ml_free_M (l);
  ml_free_M (Pesos);
  ml_free_M (Pesos_inv);
  
  ml_free_M (A);
  
  ml_free_M (B);
  
  /* Vector de fecho */
  ml_free_M (fecho);
  
  /* vector das correcoes */
  ml_free_M (delta);
  
  /* Matrizes */
  ml_free_M (A_t);
  ml_free_M (B_t);
  
  ml_free_M (M);
  ml_free_M (M_inv);
  
  ml_free_M (N);
  ml_free_M (N_inv);
  
  ml_free_M (Aux);
  ml_free_M (Aux2);
  ml_free_M (Aux3);
  ml_free_M (Aux4);
  
}
