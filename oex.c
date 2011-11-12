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
#include <math.h>    /* c padrao */
#include <cblas.h>   /* c BLAS   */

/*#include "oex.h"*/

/*** ALOCACAO E OPERACOES DE MATRIZES ***/

/*
 * Alocacao de uma matriz
 */
double *oex_alocar_M (int m, int n)
{
  double *M = (double *)calloc(m*n,sizeof(double));
  return M;
}

/*
 * Alocacao de uma matriz a zeros
 */
double *oex_alocar_M_zeros (int m, int n)
{
  double *M = oex_alocar_M (m, n);
  int num = m*n;
  int i;

  for(i = 0; i < num; i++)
    M[i] = 0;

  return M;
}

/*
 * Alocacao de uma matriz identidade
 */
double *oex_alocar_M_I(int n)
{
  double *M = oex_alocar_M_zeros (n,n);
  int i;

  for(i=0; i<n; i++)
    M[i+(n*i)] = 1.0;

  return M;
}

/*
 * altera um valor pontual valor da matriz M
 */
void oex_set_entry_M(double *M, int n, int i, int j, double val)
{
  M[j+i*n] = val;
}

/*
 * retorna um valor pontual da matriz M
 */
double oex_get_entry_M(const double *M, int n, int i, int j)
{
  return M[j+i*n];
}

/* 
 * Libertar matriz da memoria
 */
void oex_free_M (double *M)
{
  free(M);
}

/*
 * Copiar de A para B
 */
void oex_AeqB (double *A, double *B, int m, int n)
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
double *oex_AB (double *A, const int ma, const int na, double *B, const int mb, const int nb, double *C)
{
  if(na != mb)
    return NULL;

  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, ma, nb, na, 1.0, A, na, B, nb, 0.0, C, nb);

  return C;
}

/*
 * Multiplicacao de matrizes com opcao de transposicao
*/
double *oex_ABtt (double *A, int ma, int na, int ta, double *B, int mb, int nb, int tb, double *C)
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

void oex_invM(double* A, int N)
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
void oex_Mt (double *M, int m, int n, double *Mt)
{
  int i, j;
  for(i=0; i < m; i++)
    for(j=0; j < n; j++)
      Mt[i+j*m] = M[j+i*n];
}

/*
 * Multiplicacao de uma matriz por um escalar
 */
void oex_aM (double a, double *M, int m, int n)
{
  int i;
  int dim = m*n;
  for(i=0; i < dim; i++)
    M[i] *= a;
}

/*
 * Somar(op != 0) e subtrair (op = 0) duas matrizes
 */
void oex_AmmB (int op, double *A, double *B, int m, int n, double *C)
{
  int i;
  int dim = m*n;
  if(!op)
    for(i=0; i < dim; i++)
      C[i] = A[i] + B[i];
  else
    for(i=0; i < dim; i++)
      C[i] = A[i] - B[i];
}

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

/*passa para a memoria a matriz de um ficheiro ascii*/
void readm (char *nfile, double *M, int m, int n)
{
  int i, j, k=0;

  FILE *f = fopen(nfile, "r");

  for(i=0; i < n; i++)
    for(j=0; j < m; j++)
      fscanf(f,"%lf", &M[k++]);
  fclose(f);
}

/*carrega as coordenadas de uma lista ascii para uma matriz*/
void carrega_coo(double *coo, int dim, char *nfile, char tipo)
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

/*** ORIENTACAO EXTERNA ***/

typedef struct oex_oin_param 
{
  double c;
  double xo;
  double yo;
} *p_oex_oin_param;

typedef struct oex_param
{
  double Xo, Yo, Zo;
  double omega, phi, kappa;

  double *R;
} *p_oex_param;

typedef struct oex_pfs
{
  int n_pfs;
  double *terreno;
  double *foto;
} *p_oex_pfs;

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
  double *Ro = oex_alocar_M_zeros (3, 3);
  double *Rp = oex_alocar_M_zeros (3, 3);
  double *Rk = oex_alocar_M_zeros (3, 3);

  double *Aux = oex_alocar_M (3,3);

  /* preenchimento das matrizes */
  oex_set_entry_M(Ro, 3, 0, 0, 1);
  oex_set_entry_M(Ro, 3, 1, 1, cos(omega));
  oex_set_entry_M(Ro, 3, 1, 2, -sin(omega));
  oex_set_entry_M(Ro, 3, 2, 1, sin(omega));
  oex_set_entry_M(Ro, 3, 2, 2, cos(omega));

  oex_set_entry_M(Rp, 3, 0, 0, cos(phi));
  oex_set_entry_M(Rp, 3, 0, 2, sin(phi));
  oex_set_entry_M(Rp, 3, 1, 1, 1);
  oex_set_entry_M(Rp, 3, 2, 0, -sin(phi));
  oex_set_entry_M(Rp, 3, 2, 2, cos(phi));

  oex_set_entry_M(Rk, 3, 0, 0, cos(kappa));
  oex_set_entry_M(Rk, 3, 0, 1, -sin(kappa));
  oex_set_entry_M(Rk, 3, 1, 0, sin(kappa));
  oex_set_entry_M(Rk, 3, 1, 1, cos(kappa));
  oex_set_entry_M(Rk, 3, 2, 2, 1);

  switch(op)
    {
    case 1: /*omega phi kappa */
      oex_AB (Ro, 3, 3, Rp, 3, 3, Aux);
      oex_AB (Aux, 3, 3, Rk, 3, 3, R);
      break;
    }

  oex_free_M (Ro);
  oex_free_M (Rp);
  oex_free_M (Rk);
  oex_free_M (Aux);
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
  double *R = oex_alocar_M_zeros (3, 3);

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
	((Y - param->Yo)*R[2+2*3] - (Z - param->Zo)*R[2+1*3] * Nx/D - (Y - param->Yo)*R[0+2*3] + (Z - param->Zo)*R[0+1*3]);

      A[4+(i*2)*n] = - oin_param->c / D *
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
	((Y - param->Yo)*R[2+2*3] - (Z - param->Zo)*R[2+1*3] * Ny/D - (Y - param->Yo)*R[1+2*3] + (Z - param->Zo)*R[1+1*3]);

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
 * MAIN
 */
int main (void)
{
  /*
   * Definicao de variaveis
   * Alocacao de memoria
   */
  int n = 25;              /* Numero de PFS 1/2 de observacoes */
  int n0 = 3;              /* numero de PFS minimo */
  int u = 6;               /* numero de parametros */
  double var0 = 1.0;       /* var a priori */
  
  /*double d2r = (4.0*atan(1.0))/180;*/

  double *L = oex_alocar_M(n * 2, 1);
  double *V = oex_alocar_M(n * 2, 1);
  double *X = oex_alocar_M(u, 1);
  double *Cl = oex_alocar_M_I (n * 2);
  double *Pl = oex_alocar_M (n * 2, n * 2);
  double *A = oex_alocar_M (n * 2, u);
  double *W = oex_alocar_M(n * 2, 1);
  double *D = oex_alocar_M(u, 1);
  double *Cx = oex_alocar_M(u, u);
  double *InvN = oex_alocar_M(u, u);
  double *Aux = oex_alocar_M(u, n * 2);
  double *Aux2 = oex_alocar_M(u, n * 2);
  double *Aux3 = oex_alocar_M(u, 1);
  
  p_oex_oin_param oin_param = oex_alocar_oin_param ();
  p_oex_param param = oex_alocar_param ();
  p_oex_pfs pfs = oex_alocar_pfs ();

  double terreno[25*3];
  double foto[25*2];

  /*Pl = var inv(Cl)*/

  /*
   * Carregar dados
   */
  oex_add_oin_param (oin_param, (2527.01371+2515.01405)/2.0, 1041.17490, 782.66840);
  oex_add_param (param, 3, 1, 9, 0.0, 0.0, 0.0, 1);
  
  carrega_coo(terreno, 25*3, "dados/pfs_t.txt", 't');
  carrega_coo(foto, 25*2, "dados/pfs_f.txt", 'f');

  oex_add_pfs (pfs, 25, terreno, foto);

  oex_transfotopfs (oin_param, pfs);

  showm (pfs->foto, 25, 2); /*temp*/

  /*
   * Ajustamento
   */

  /*definicao dos pesos*/
  oex_AeqB (Pl, Cl, n*2, n*2); /*Pl = Cl*/
  oex_invM(Pl, n*2);           /*Pl = inv(Pl)*/
  oex_aM (var0, Pl, n*2, n*2); /*Pl = var*Pl*/

  oex_param2X (param, X);

  do {
    showm (X, u, 1); /*temp*/

    printf("* * * * * *\n");
    scanf("%d", &n0);
    if(n0 == 0)
      return 1;

    oex_designM (A, oin_param, pfs, param);
    oex_Mfecho (oin_param, pfs, param, W);

    /*
    oex_M_ascii ("dados/A.txt", A, n * 2, u);
    oex_M_ascii ("dados/W.txt", W, n * 2, 1);
    */

    oex_ABtt (A, n * 2, u, 1, Pl, n * 2, n * 2, 0, Aux);
    oex_ABtt (Aux, u, n * 2, 0, A, n * 2, u, 0, InvN);  
    oex_invM(InvN, u);
    oex_aM (-1.0, InvN, u, u);
    oex_ABtt (InvN, u, u, 0, A, n * 2, u, 1, Aux);
    oex_ABtt (Aux, u, n * 2, 0, Pl, n * 2, n * 2, 0, Aux2); 
    oex_ABtt (Aux2, u, n * 2, 0, W, n * 2, 1, 0, D);

    /*
    oex_M_ascii ("dados/D.txt", D, u, 1);
    oex_M_ascii ("dados/X.txt", X, u, 1);
    */

    oex_AmmB (0, X, D, u, 1, Aux3);

    oex_AeqB (X, Aux3, u, 1);

    oex_X2param (X, param);
    
  } while(oex_norm_M (D, u, 1) > 0.001);

  

  /*showm (Pl, n*2, n*3);*/

  /*
   * Libertar memoria
   */
  oex_free_M(L);
  oex_free_M(V);
  oex_free_M(X);
  oex_free_M(Cl);
  oex_free_M(Pl);
  oex_free_M(A);
  oex_free_M(W);
  oex_free_M(D);
  oex_free_M(Cx);
  oex_free_M(InvN);
  oex_free_M(Aux);
  oex_free_M(Aux2);

  oex_libertar_oin_param (oin_param);
  oex_libertar_param (param);
  oex_libertar_pfs (pfs);
 
  return 0;
}
