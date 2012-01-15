/*
 * Implementacao da OEX
 * Nome: oex.c
 * Autor: vasco conde
 *
 * Description: Conjunto de metodos para a determinacao dos parametros
 * de orientacao externa de uma imagem utilizado um ajustamento por minimos
 * quadrados e o modelo das equacoes de colinearidade
 */

/*
 * BIBLIOTECAS AUXILIARES
 */
#include <stdio.h>   /* c padrao */
#include <stdlib.h>  /* c padrao */
#include <math.h>   /* c padrao */
/*#include <cblas.h>*/   /* c BLAS   */
#include "matrixlib.h"

#include "oex.h"

/*Estrutura para albergar 
 *os parametros de o. interna*/
struct oex_oin_param 
{
  double c;
  double xo;
  double yo;
};

/*Estrutura para albergar 
 *os parametros de o. externa*/
struct oex_param
{
  double Xo, Yo, Zo;
  double omega, phi, kappa;

  double *R;
};

/* estrutura para albergar as listas
 * de coordenadas dos pfs 
 * no espaco terreno e imagem */
struct oex_pfs
{
  int n_pfs;
  double *terreno;
  double *foto;
};

/*** TEMPORARIAS ***/

/* escrever no ecra a matriz M */
void oex_showm (double *M, int m, int n)
{
  int i, j;
  for(i = 0; i < m; i++)
    {
      for(j = 0; j < n; j++)
	printf("%.10lf ", M[j+i*n]);
      printf("\n");
    }
}

/*passa para a memoria a matriz de um ficheiro ascii*/
void oex_readm (char *nfile, double *M, int m, int n)
{
  int i, j, k=0;

  FILE *f = fopen(nfile, "r");

  for(i=0; i < n; i++)
    for(j=0; j < m; j++)
      fscanf(f,"%lf", &M[k++]);
  fclose(f);
}

/*carrega as coordenadas de uma lista ascii para uma matriz*/
void oex_carrega_coo(double *coo, int dim, char *nfile, char tipo)
{
  int i;
  int id;
  
  FILE *f = fopen(nfile, "r");

  switch(tipo)
    {
    case 't':
      for(i=0; i<dim; i+=3)
	{
	  fscanf(f,"%d %lf %lf %lf", &id, &coo[i], &coo[i+1], &coo[i+2]);
	}
      break;
    case 'f':
      for(i=0; i<dim; i+=2)
	{
	  fscanf(f,"%d %lf %lf", &id, &coo[i], &coo[i+1]);
	}
      break;
    }

  fclose(f);
}


p_oex_oin_param oex_alocar_oin_param ()
{
  p_oex_oin_param novo = (p_oex_oin_param)malloc(sizeof(struct oex_oin_param));
  return novo;
}

p_oex_param oex_alocar_param ()
{
  p_oex_param novo = (p_oex_param)malloc(sizeof(struct oex_param));
  return novo;
}

p_oex_pfs oex_alocar_pfs ()
{
  p_oex_pfs novo = (p_oex_pfs)malloc(sizeof(struct oex_pfs));
  return novo;
}

/* REVISAO */

void oex_libertar_oin_param (p_oex_oin_param e)
{
  free(e);
}

void oex_libertar_param (p_oex_param e)
{
  free(e->R);
  free(e);
}

void oex_libertar_pfs (p_oex_pfs e)
{
  free(e);
}

/* 
 * Calcula a matriz de rotacao 
 */
void oex_M_rot (double *R, double omega, double phi, double kappa, int op)
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
    }

  ml_free_M (Ro);
  ml_free_M (Rp);
  ml_free_M (Rk);
  ml_free_M (Aux);
}

/*
 * Adiciona os parametros de orientacao interna
 */
void oex_add_oin_param (p_oex_oin_param oin_param, double c, double xo, double yo)
{
  oin_param->c = c;
  oin_param->xo = xo;
  oin_param->yo = yo;  
}

/*
 * Adiciona os parametros de orientacao externa
 */
void oex_add_param (p_oex_param param, double Xo, double Yo, double Zo, double omega, double phi, double kappa, int opR)
{
  double *R = ml_alocar_M_zeros (3, 3);

  param->Xo = Xo;
  param->Yo = Yo;
  param->Zo = Zo;
  param->omega = omega;
  param->phi = phi;
  param->kappa = kappa;

  oex_M_rot (R, omega, phi, kappa, opR);

  param->R = R;
}

/*
 * Adiciona pfs
 */
void oex_add_pfs (p_oex_pfs pfs, int n_pfs, double *terreno, double *foto)
{
  pfs->n_pfs = n_pfs;
  pfs->terreno = terreno;
  pfs->foto = foto;  
}

/*
 * Modifica os parametros de orientacao externa
 */
void oex_set_param (p_oex_param param, double Xo, double Yo, double Zo, double omega, double phi, double kappa, int opR)
{
  param->Xo = Xo;
  param->Yo = Yo;
  param->Zo = Zo;
  param->omega = omega;
  param->phi = phi;
  param->kappa = kappa;

  oex_M_rot (param->R, omega, phi, kappa, opR);
} 

/*
 * Recolhe os parametros de orientacao externa
 */
void oex_get_param (p_oex_param param, double* Xo, double* Yo, double* Zo, double* omega, double* phi, double* kappa)
{
  *Xo = param->Xo;
  *Yo = param->Yo;
  *Zo = param->Zo;
  *omega = param->omega;
  *phi = param->phi;
  *kappa = param->kappa;
} 

/*
 * retorna as componetes Nx, Ny e D
 * das equacoes de colinearidade
 */
double oex_n_xy_d (int op, int i, p_oex_pfs pfs, p_oex_param param)
{
  double X = pfs->terreno[i*3];
  double Y = pfs->terreno[i*3+1];
  double Z = pfs->terreno[i*3+2];

  double *R = param->R;

  switch(op)
    {
    case 1: /* Nx */
      return R[0+0*3] * (X - param->Xo) + R[0+1*3] * (Y - param->Yo) + R[0+2*3] * (Z - param->Zo);
      break;
    case 2: /* Ny */
      return R[1+0*3] * (X - param->Xo) + R[1+1*3] * (Y - param->Yo) + R[1+2*3] * (Z - param->Zo);
      break;
    case 3: /* D */
      return R[2+0*3] * (X - param->Xo) + R[2+1*3] * (Y - param->Yo) + R[2+2*3] * (Z - param->Zo);
      break;
    }
  return 0;
}

/*
 * Matriz A
 */

void oex_designM (double *A, p_oex_oin_param oin_param, p_oex_pfs pfs, p_oex_param param)
{
  int i;
  int n = 6; /* num param */
  double *R = param->R;

  double Y;
  double Z;
  double Nx, Ny, D;

  for(i=0; i < pfs->n_pfs; i++)
    {
      Y = pfs->terreno[i*3+1];
      Z = pfs->terreno[i*3+2];

      Nx = oex_n_xy_d (1, i, pfs, param);
      Ny = oex_n_xy_d (2, i, pfs, param);
      D = oex_n_xy_d (3, i, pfs, param);

      /* dx/d */

      A[0+(i*2)*n] = - oin_param->c / pow(D,2) * 
	(R[2+0*3]*Nx - R[0+0*3]*D);
      
      A[1+(i*2)*n] = - oin_param->c / pow(D,2) * 
	(R[2+1*3]*Nx - R[0+1*3]*D);

      A[2+(i*2)*n] = - oin_param->c / pow(D,2) * 
	(R[2+2*3]*Nx - R[0+2*3]*D);

      A[3+(i*2)*n] = - oin_param->c / D *
	(((Y - param->Yo)*R[2+2*3] - (Z - param->Zo)*R[2+1*3]) * Nx/D - (Y - param->Yo)*R[0+2*3] + (Z - param->Zo)*R[0+1*3]);

      A[4+(i*2)*n] = oin_param->c / D *
	((Nx*cos(param->kappa) - Ny*sin(param->kappa))*Nx/D + D*cos(param->kappa));
      
      A[5+(i*2)*n] = - oin_param->c / D * Ny;

      /* dy/d */
      
      A[0+((i*2)+1)*n] = - oin_param->c / pow(D,2) * 
	(R[2+0*3]*Ny - R[1+0*3]*D);

      A[1+((i*2)+1)*n] = - oin_param->c / pow(D,2) * 
	(R[2+1*3]*Ny - R[1+1*3]*D);

      A[2+((i*2)+1)*n] = - oin_param->c / pow(D,2) * 
	(R[2+2*3]*Ny - R[1+2*3]*D);

      A[3+((i*2)+1)*n] = - oin_param->c / D *
	(((Y - param->Yo)*R[2+2*3] - (Z - param->Zo)*R[2+1*3]) * Ny/D - (Y - param->Yo)*R[1+2*3] + (Z - param->Zo)*R[1+1*3]);

      A[4+((i*2)+1)*n] = oin_param->c / D *
	((Nx*cos(param->kappa) - Ny*sin(param->kappa))*Ny/D - D*sin(param->kappa));

      A[5+((i*2)+1)*n] = oin_param->c / D * Nx;
    }
}

/*
 * Matriz de fecho
 */
void oex_Mfecho (p_oex_oin_param oin_param, p_oex_pfs pfs, p_oex_param param, double *W)
{

  int i;
  double Nx, Ny, D;

  for(i=0; i < pfs->n_pfs; i++)
    {
      Nx = oex_n_xy_d (1, i, pfs, param);
      Ny = oex_n_xy_d (2, i, pfs, param);
      D = oex_n_xy_d (3, i, pfs, param);

      W[i*2] = oin_param->xo - oin_param->c * Nx/D - pfs->foto[i*2];
      W[i*2+1] = oin_param->yo - oin_param->c * Ny/D - pfs->foto[i*2+1];;
    }
}

/*
 * Actualiza a matriz dos parametros X
 * com os valores da estrutura de dados param
 */
void oex_param2X (p_oex_param param, double *X)
{
  X[0] = param->Xo;
  X[1] = param->Yo;
  X[2] = param->Zo;
  X[3] = param->omega;
  X[4] = param->phi;
  X[5] = param->kappa;
}

/*
 * Actualiza os valores da estrutura de dados param
 * com os valor da matriz de parametros X
 */
void oex_X2param (double *X, p_oex_param param)
{
  oex_set_param (param, X[0], X[1], X[2], X[3], X[4], X[5], 1);
}

/*
 * Calcula a norma de uma matriz M
 */
double oex_norm_M (double *M, int m, int n)
{
  int i;
  double norm = 0;
  for(i=0; i<m*n; i++)
    norm += pow(M[i],2);
  return sqrt(norm);
}

/*
 * Transforma as coordenadas imagem
 * de: origen canto superior esquerdo e eixos (X >;Y v) 
 * para: origem centro da imagem e eixos (X >;Y ^) 
 */
void oex_transfotopfs (p_oex_oin_param oin_param, p_oex_pfs pfs)
{
  int i;
  double aux;

  for(i=0; i < pfs->n_pfs; i++)
    {
      aux = pfs->foto[i*2];
      pfs->foto[i*2] = aux - oin_param->xo;
      aux = pfs->foto[i*2+1];
      pfs->foto[i*2+1] = -aux + oin_param->yo;
    }
}

void oex_M_ascii (char *filename, double *M, int m, int n)
{
  FILE *cR = fopen(filename, "w"); /* temp */
  int i,j;

  if (!cR)
    {
      printf("ERRO: Na escrita no ficheiro %s\n!", filename);
    }

  for(i=0; i < m; i++)
    {
      for(j = 0; j < n; j++)
	fprintf(cR,"%lf ", M[j+i*n]);
      fprintf(cR,"\n");
    }
  fclose(cR);
}

/*
 * Calculo
 */
void oex_compute (int n, p_oex_oin_param oin_param, p_oex_param param, p_oex_pfs pfs)
{
  /*
   * Definicao de variaveis
   * Alocacao de memoria
   */
  /*const int n = 14;*/              /* Numero de PFS 1/2 de observacoes */
  /* char * ffoto = "dados/pfs_f.txt";
     char * fterreno = "dados/pfs_t.txt"; */

  /*
  char * ffoto = "dados/par/limpres/pfs_fr.txt";
  char * fterreno = "dados/par/limpres/pfs_tr.txt";
  */

  int n0 = 3;              /* numero de PFS minimo */
  int df = n*2 - n0*2;
  int u = 6;               /* numero de parametros */
  int it = 0;              /* numero de iteracoes */
  int i;

  double var0 = 1.0;       /* var a priori */
  double var0_;            /* var a posteriori */

  /*auxiliares para impressao dos residuos*/
  int k;
  int k_max;
  double max;
  double res_norma;
  double res_media;

  /*double d2r = (4.0*atan(1.0))/180;*/

  double *L = ml_alocar_M(n * 2, 1);
  double *V = ml_alocar_M(n * 2, 1);
  double *X = ml_alocar_M(u, 1);
  double *Cl = ml_alocar_M_I (n * 2);
  double *Pl = ml_alocar_M (n * 2, n * 2);
  double *A = ml_alocar_M (n * 2, u);
  double *W = ml_alocar_M(n * 2, 1);
  double *D = ml_alocar_M(u, 1);
  double *Cx = ml_alocar_M(u, u);
  double *InvN = ml_alocar_M(u, u);
  double *Aux = ml_alocar_M(u, n * 2);
  double *Aux2 = ml_alocar_M(u, n * 2);
  double *Aux3 = ml_alocar_M(u, 1);
  double *Aux4 = ml_alocar_M(n * 2, 1);
  double *Aux5 = ml_alocar_M(1, n * 2);
  
  /*
  p_oex_oin_param oin_param = oex_alocar_oin_param ();
  p_oex_param param = oex_alocar_param ();
  p_oex_pfs pfs = oex_alocar_pfs ();
  */
  /*
  double terreno[n*3];
  double foto[n*2];
  */

  /*Pl = var inv(Cl)*/

  /*
   * Carregar dados
   */
  /*oex_add_oin_param (oin_param, (2527.01371+2515.01405)/2.0, 1041.17490, 782.66840);*/
  /*  
oex_add_oin_param (oin_param, (2524.980712085470259+2512.961041523109998)/2, 1043.777785082072114, 782.329224502696547);

  oex_add_param (param, 3, 1, 9, 0.0, 0.0, 0.0, 1);
  
  oex_carrega_coo(terreno, n*3, fterreno, 't');
  oex_carrega_coo(foto, n*2, ffoto, 'f');

  oex_add_pfs (pfs, n, terreno, foto);
  */
  oex_transfotopfs (oin_param, pfs);

  /*
   * Ajustamento
   */

  /*definicao dos pesos*/
  ml_AeqB (Pl, Cl, n*2, n*2); /*Pl = Cl*/
  ml_invM(Pl, n*2);           /*Pl = inv(Pl)*/
  ml_aM (var0, Pl, n*2, n*2); /*Pl = var*Pl*/

  oex_param2X (param, X);

  do {

    /*matriz A*/
    oex_designM (A, oin_param, pfs, param);
    
    /*matriz de fecho */
    oex_Mfecho (oin_param, pfs, param, W);

    /*matriz delta*/
    ml_ABtt (A, n * 2, u, 1, Pl, n * 2, n * 2, 0, Aux);
    ml_ABtt (Aux, u, n * 2, 0, A, n * 2, u, 0, InvN);  
    ml_invM(InvN, u);
    ml_aM (-1.0, InvN, u, u);
    ml_ABtt (InvN, u, u, 0, A, n * 2, u, 1, Aux);
    ml_ABtt (Aux, u, n * 2, 0, Pl, n * 2, n * 2, 0, Aux2); 
    ml_ABtt (Aux2, u, n * 2, 0, W, n * 2, 1, 0, D);

    /*matriz dos parametros ajustados*/
    ml_AmmB (0, X, D, u, 1, Aux3);
    ml_AeqB (X, Aux3, u, 1);

    /*actualizacao dos param*/
    oex_X2param (X, param);
    
    it++; 

  } while(oex_norm_M (D, u, 1) > 0.0001 && it <= 10);


  /*matriz de residuos*/
  ml_AB (A, n*2, u, D, u, 1, Aux4);
  ml_AmmB (0, Aux4, W, n*2, 1, V);

  /*matriz de V&C dos parametros ajustados*/
  oex_designM (A, oin_param, pfs, param);
  ml_ABtt (A, n * 2, u, 1, Pl, n * 2, n * 2, 0, Aux);
  ml_ABtt (Aux, u, n * 2, 0, A, n * 2, u, 0, Cx);  
  ml_invM(Cx, u);
  ml_aM (var0, Cx, u, u);

  /*var a posteriori*/
  ml_ABtt (V, n*2, 1, 1, Pl, n * 2, n * 2, 0, Aux5);
  ml_AB (Aux5, 1, n * 2, V, n * 2, 1, &var0_);
  var0_ /= df;


  printf("\nINFO: %d iteracoes\n", it);


  printf("\nINFO: Parametros ajustados:\n\n");
  oex_showm(X, u, 1);
  printf("\nINFO: Respectivas incertezas:\n\n");
  for(i = 0; i < u; i++)
    {
      printf("%lf\n", sqrt(Cx[i+i*u]));
    }
  printf("\nINFO: Residuos:\n\n");
  
  for(i=0, k = 1, res_media = 0; i < n*2; i++)
    {
      if(i%2 == 0)
	printf("(%d) %lf\n", k, V[i]);
      else
	{
	  res_norma = sqrt(V[i]*V[i] + V[i-1]*V[i-1]);
	  
	  if (k == 1)
	    {
	      max = res_norma;
	      k_max = k;
	    }
	  else
	    if (res_norma > max)
	      {
		max = res_norma;
		k_max = k;
	      }

	  printf("(%d) %lf           norm = %lf\n", k, V[i], res_norma);
	  k++;
	}
      
      res_media += V[i];
    }

  res_media /= (n*2);

  printf("\nINFO: a media dos residuos eh de: %lf\n", res_media);

  printf("\nINFO: O maior residuo eh (%d) com %lf\n", k_max, max);

  printf("\nINFO: variancia de referencia: %lf \n", var0);

  printf("\nINFO: variancia de referencia a posteriori: %lf \n", var0_);
  
  /*oex_showm(V, n*2, 1);*/

  /*oex_showm (Pl, n*2, n*3);*/

  /*
   * Libertar memoria
   */
  ml_free_M(L);
  ml_free_M(V);
  ml_free_M(X);
  ml_free_M(Cl);
  ml_free_M(Pl);
  ml_free_M(A);
  ml_free_M(W);
  ml_free_M(D);
  ml_free_M(Cx);
  ml_free_M(InvN);
  ml_free_M(Aux);
  ml_free_M(Aux2);
  ml_free_M(Aux3);
  ml_free_M(Aux4);
  ml_free_M(Aux5);


}
