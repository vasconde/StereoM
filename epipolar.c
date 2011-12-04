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

/*** TEMPORARIAS ***/

/* ler parametros x,y,x,omega,phi,kappa */
int ep_read_ascii_param (char* filen, double* Param)
{
  FILE *f = fopen(filen, "r");

  if(f == NULL)
    {
      printf("INFO: erro na leitura do ficheiro de parametros.\n");
      return 1;
    }

  fscanf(f,"%lf %lf %lf", &Param[0], &Param[1], &Param[2]);
  fscanf(f,"%lf %lf %lf", &Param[3], &Param[4], &Param[5]);

  fclose(f);
  return 0;
}


/*
 * Calculo da matriz fundamental
 */

/* Calcula a matriz de rotacao */
void ep_M_rot (double *R, double omega, double phi, double kappa)
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

/*
 * Gera a matriz de translacao S e rotacao R de l -> r
 * e determina a matrir essencial
 */
void ep_EssentialMatrix (double *ParamL, double *ParamR, double *E)
{
  double *dP = ml_alocar_M (6, 1);
  double *S = ml_alocar_M (3, 3);
  double *R = ml_alocar_M (3, 3);

  ml_AmmB (1, ParamR, ParamL, 6, 1, dP); /*dP = ParamR - ParamL*/
  
  /* matriz S */
  ml_set_entry_M(S, 3, 0, 0, 0); 
  ml_set_entry_M(S, 3, 0, 1, -dP[2]); 
  ml_set_entry_M(S, 3, 0, 2, dP[1]);
  ml_set_entry_M(S, 3, 1, 0, dP[2]); 
  ml_set_entry_M(S, 3, 1, 1, 0); 
  ml_set_entry_M(S, 3, 1, 2, -dP[0]);
  ml_set_entry_M(S, 3, 2, 0, -dP[1]); 
  ml_set_entry_M(S, 3, 2, 1, dP[0]); 
  ml_set_entry_M(S, 3, 2, 2, 0);

  /* matriz R */
  ep_M_rot (R, dP[3], dP[4], dP[5]);

  /* matriz essencial E = RS */

  ml_AB (R, 3, 3, S, 3, 3, E);

  ml_free_M (dP);
  ml_free_M (S);
  ml_free_M (R);
}

/*
 * Calculo da reta epipolar E*Pl
 */
void ep_epipolarLine (double *E, double *Pl, double *EqEp)
{
  ml_AB(E, 3, 3, Pl, 3, 1, EqEp);  
}

int main (void)
{
  char fl[] = "dados/par/paramL.txt";
  char fr[] = "dados/par/paramR.txt";

  double* ParamL = ml_alocar_M (6, 1);
  double* ParamR = ml_alocar_M (6, 1);
  double* E = ml_alocar_M (3, 3);

  double *Pl = ml_alocar_M (3, 1);

  double *EqEp = ml_alocar_M (3, 1);
 
  double f = 2512.961041523109998;
  double pxs = 8e-3 / f; /*pixel em m*/
  

  /* PF 109 */
  Pl[0] = -80.7778 * pxs;
  Pl[1] = 282.3292 * pxs;
  Pl[2] = - f * pxs;

  printf("%.10lf\n", pxs);

  if(ep_read_ascii_param (fl, ParamL)) return 1;
  if(ep_read_ascii_param (fr, ParamR)) return 1;

  /*
  printf("\nL\n");
  showm (ParamL, 6, 1);
  printf("\nR\n");
  showm (ParamR, 6, 1);
  printf("\ndR dT\n");
  */

  ep_EssentialMatrix (ParamL, ParamR, E);
  ep_epipolarLine (E, Pl, EqEp);

  showm (Pl, 3, 1);
  printf("\n\n");
  /*ml_aM (1.0/pxs, EqEp, 3, 1);*/
  showm (EqEp, 3, 1);

  ml_free_M (ParamL);
  ml_free_M (ParamR);
  ml_free_M (E);
  ml_free_M (Pl);

  return 0;
}
