/*
 * StereoM
 * Nome: stereom.c
 * Autor: vasco conde
 */

/*C PADRAO*/
#include <stdio.h>
#include <stdio_ext.h> /*fpurge para limpar o buffer*/
#include <stdlib.h>
#include <math.h>
#include <assert.h> /*assert(codition)*/

/*CBLAS*/
#include <cblas.h>

/*MODULOS STEREOM*/
#include "matrixlib.h"
#include "abmatching.h"
#include "photojpeglib.h"
#include "oex.h"
#include "epipolar.h"

/*PRE PROCESSADOR*/
#define DEBUG

/*CONSTANTES*/
#define MAX_FILE_NAME 75

/*MACROS*/
#define POS(i,j,n) (j+i*n)

int op;

/*
 * NOMES DOS FICHEIROS DOS DADOS DE ENTRADA (POR DEFEITO)
 */

/* ORIENTACAO INTERNA ESQUERDA*/
char file_param_in_l[MAX_FILE_NAME]= "dados/dados/oin/oin_l.txt"; /*file parameters internal left*/
int info_oin_l[] = {1,0}; /*nome sim | em memoria nao*/

/* ORIENTACAO INTERNA DIREITA*/
char file_param_in_r[MAX_FILE_NAME]= "dados/dados/oin/oin_r.txt"; /*file parameters internal right*/
int info_oin_r[] = {1,0}; /*nome sim | em memoria nao*/

/* ORIENTACAO EXTERNA ESQUERDA*/
char file_l_c_i_l[MAX_FILE_NAME] = "dados/dados/oex/pfs_fl.txt"; /*file list coordinates image left*/
char file_l_c_t_l[MAX_FILE_NAME] = "dados/dados/oex/pfs_tl.txt"; /*file list coordinates terrain left*/
int l_c_l = 19; /*list coordinates left*/
char file_param_ex_ini_l[MAX_FILE_NAME]= "dados/dados/oex/oex_l.txt"; /*file parameters external initial left*/
int info_oex_l[] = {1,0,1,0,1,0}; /*nome sim | em memoria nao | nome sim | carregado nao ...*/

/* ORIENTACAO EXTERNA DIREITA */
char file_l_c_i_r[MAX_FILE_NAME] = "dados/dados/oex/pfs_fr.txt"; /*file list coordinates image right*/
char file_l_c_t_r[MAX_FILE_NAME] = "dados/dados/oex/pfs_tr.txt"; /*file list coordinates terrain right*/
int l_c_r = 17; /*list coordinates right*/
char file_param_ex_ini_r[MAX_FILE_NAME]= "dados/dados/oex/oex_r.txt"; /*file parameters external initial right*/
int info_oex_r[] = {1,0,1,0,1,0}; /*nome sim | em memoria nao | nome sim | em memoria nao ...*/

/* MATRIZ FUNDAMENTAL */
char file_l_c_i_l_c[MAX_FILE_NAME] = "dados/dados/matrix/left.txt"; /*file list coordinates image left common*/
char file_l_c_i_r_c[MAX_FILE_NAME] = "dados/dados/matrix/right.txt"; /*file list coordinates image right common*/
int l_c_i_c = 22; /*list coordinates image common*/
int info_fm[] = {1,0,1,0}; /*nome sim | em memoria nao | nome sim | carregado nao*/

/* IMAGENS DIREITA E ESQUERDA  */
char file_image_l[MAX_FILE_NAME] = "dados/dados/imagens/L_E00001_rect.jpg"; /*left image*/
char file_image_r[MAX_FILE_NAME] = "dados/dados/imagens/R_E00003_rect.jpg"; /*right image*/
int info_im[] = {1,0,1,0}; /*nome sim | em memoria nao | nome sim | carregado nao*/

int *info_dados[] = {info_oin_l, info_oin_r, info_oex_l, info_oex_r, info_fm, info_im};


/*apresenta na consola o menu principal*/
int display_main_menu ()
{
  int op;
  do 
    {
      printf("\n+--- STEREOM APP ---+\n\n");
      printf(" 1. Ficheiros de dados\n");
      printf(" 2. Orientacao Externa\n");
      printf(" 3. Matriz Fundamental\n");
      printf(" 4. Correspondencia automatica\n");
      printf(" 5. Restituicao\n");
      printf(" 6. Sobre StereoM\n");
      printf(" 7. Sair\n");
      printf("\nopcao: ");
      scanf("%d", &op);
    } while (op < 1 || op > 7);

  return op;
}

void apresentar_dados()
{
  printf("\n(0) *** Orientacao Interna ***\n");
  printf("        Parametros:\n");
  printf("          Esquerda: %s\n", file_param_in_l);
  printf("          Direita: %s\n", file_param_in_r);
  printf("\n(1) *** Orientacao Externa ***\n");
  printf("        Esquerda (%d):\n", l_c_l);
  printf("          Cordenadas imagem: %s\n", file_l_c_i_l);
  printf("          Coordenadas terreno: %s\n", file_l_c_t_l);
  printf("          Parametros iniciais: %s\n", file_param_ex_ini_l);
  printf("        Direita (%d):\n", l_c_r);
  printf("          Coordenadas imagem: %s\n", file_l_c_i_r);
  printf("          Coordenadas terreno: %s\n", file_l_c_t_r);
  printf("          Parametros iniciais: %s\n", file_param_ex_ini_r);
  printf("\n(2) *** Matriz Fundamental ***\n");
  printf("      Lista de coordenadas imagem comuns (%d):\n", l_c_i_c);
  printf("        Esquerda: %s\n", file_l_c_i_l_c);
  printf("        Direita: %s\n", file_l_c_i_r_c);
  printf("\n(3) *** Estereorestituicao automatica ***\n");  
  printf("        Imagem esquerda: %s\n", file_image_l);
  printf("        Imagem direita: %s\n", file_image_r);
}

void modifica_dados ()
{
  int seccao;     /*seccao*/
  int op_e_d; /*opecao */
  printf("Seccao (0,1,2 ou 3): ");
  
  scanf("%d", &seccao);
  assert(seccao >= 0 && seccao <= 3);

  switch (seccao)
    {
    case 0:
      printf("\n(0) *** Orientacao Interna ***\n");
      printf("            Parametros\n");
      printf("1. Esquerda\n2. Direita\nopcao: ");

      scanf("%d", &op_e_d);
      assert(op_e_d == 1 || op_e_d == 2);

      switch (op_e_d)
	{
	case 1:
	 
	  printf("   da imagem esquerda: ");
	  scanf("%s", file_param_in_l);
	  /*reinicia info*/
	  info_dados[0][0] = 1;
	  info_dados[0][1] = 0;

	  break;

	case 2:
	  printf("    da imagem direita: ");
	  scanf("%s", file_param_in_r);
	  /*reinicia info*/
	  info_dados[1][0] = 1;
	  info_dados[1][1] = 0;

	  break;
	}
      break;

    case 1:
      printf("\n(1) *** Orientacao Externa ***\n");
      printf("            Lista de coordenadas e parametros\n");
      printf("1. Esquerda\n2. Direita\nopcao: ");  

      scanf("%d", &op_e_d);
      assert(op_e_d == 1 || op_e_d == 2);

      switch (op_e_d)
	{
	case 1:
	 
	  printf("    Imagem da imagem esquerda: ");
	  scanf("%s", file_l_c_i_l);
	  /*reinicia info*/
	  info_dados[2][0] = 1;
	  info_dados[2][1] = 0;
	  
	  printf("    Terreno da imagem esquerda: ");
	  scanf("%s", file_l_c_t_l);
	  /*reinicia info*/
	  info_dados[2][2] = 1;
	  info_dados[2][3] = 0;

	  printf("numero de pontos: ");
	  scanf("%d", &l_c_l);

	  printf("    Parametros iniciais imagem esquerda: ");
	  scanf("%s", file_param_ex_ini_l);
	  /*reinicia info*/
	  info_dados[2][4] = 1;
	  info_dados[2][5] = 0;
	  break;

	case 2:
	  printf("    Imagem da imagem direita: ");
	  scanf("%s", file_l_c_i_r);
	  info_dados[3][0] = 1;
	  info_dados[3][1] = 0;

	  printf("    Terreno da imagem direita:");
	  scanf("%s", file_l_c_t_r);
	  info_dados[3][2] = 1;
	  info_dados[3][3] = 0;

	  printf("    numero de pontos: ");
	  scanf("%d", &l_c_r);

	  printf("    Parametros iniciais imagem direita: ");
	  scanf("%s", file_param_ex_ini_r);
	  /*reinicia info*/
	  info_dados[3][4] = 1;
	  info_dados[3][5] = 0;
	  break;
	}
      
      break;
    case 2:

      printf("\n(2) *** Matriz Fundamental ***\n");      
      printf("  Lista de coordenadas imagem comuns:\n");

      printf("    Esquerda: ");
      scanf("%s", file_l_c_i_l_c);
      /*reinicia info*/
      info_dados[4][0] = 1;
      info_dados[4][1] = 0;

      printf("    Direita: ");
      scanf("%s", file_l_c_i_r_c);
      /*reinicia info*/
      info_dados[4][2] = 1;
      info_dados[4][3] = 0;

      printf("    numero de pontos: ");
      scanf("%d", &l_c_i_c);

      break;
    case 3:

      printf("\n(3) *** Estereorestituicao automatica ***\n");

      printf("  Imagem esquerda: ");
      scanf("%s", file_image_l);
      /*reinicia info*/
      info_dados[5][0] = 1;
      info_dados[5][1] = 0;

      printf("  Imagem direita: ");
      scanf("%s", file_image_r);
      /*reinicia info*/
      info_dados[5][2] = 1;
      info_dados[5][3] = 0;

      break;
    }
}

void ficheiros_dados ()
{
  int o;
  do {

    apresentar_dados();

    printf("\n1. Alterar\n");
    printf("2. Concluido\n");
    printf("Opcao: ");
    
    scanf("%d", &o);
    assert(o == 1 || o == 2);

    switch (o)
      {
      case 1:
	modifica_dados ();
	break;
      }
  } while (o != 2);
}

/*
 * Apresenta na consola a informacao relativa
 * ao stereom
 */
void sobre_stereom ()
{
  char c[1];
  printf("\n+--- SOBRE ---+\n\n");
  printf("Nome:      StereoM\n");
  printf("Versao:    1.0\n");
  printf("Autor:     Vasco Conde\n");
  printf("Descricao: Estereorestituicao automatica\n");
  scanf("%s", c);
}


void orientacao_externa_foto (char *ffoto, char *fterreno, int n, p_oex_oin_param oin_param, p_oex_param param)
{
  /*
  char * ffoto = "dados/par/limpres/pfs_fr.txt";
  char * fterreno = "dados/par/limpres/pfs_tr.txt";
  */

  /*int n = 14;*/

  p_oex_pfs pfs = oex_alocar_pfs ();

  double terreno[n*3];
  double foto[n*2];

  /*
   * Carregar dados
   */
  /*oex_add_oin_param (oin_param, (2527.01371+2515.01405)/2.0, 1041.17490, 782.66840);*/
  
  oex_carrega_coo(terreno, n*3, fterreno, 't');
  oex_carrega_coo(foto, n*2, ffoto, 'f');
  
  oex_add_pfs (pfs, n, terreno, foto);

  oex_compute (n, oin_param, param, pfs);

  oex_libertar_pfs (pfs);

}


void orientacao_externa (char l_or_r, p_oex_oin_param oin_param, p_oex_param oex_param)
{
  double xo, yo, f;
  double Xo, Yo, Zo, omega, phi, kappa;

  FILE *foin;
  FILE *foex;
 
  char *f_lista_coo_t;
  char *f_lista_coo_i;
  int npfs;

  assert(l_or_r == 'l' || l_or_r == 'r');

  switch (l_or_r)
    {
    case 'l':

      if (!info_dados[0][1])
	{
	  foin = fopen(file_param_in_l, "r");
	  if (foin == NULL) 
	    printf("ERRO: Na leitura do ficheiro %s\n", file_param_in_l);
	    	  
	  /*carrega os parametros da orientacao interna*/
	  fscanf(foin, "%lf %lf %lf", &xo, &yo, &f); /*le do ficheiro*/
	  oex_add_oin_param (oin_param, f, xo, yo);       /*carrega a estrutura*/
	  
	  
	  fclose(foin);

	  info_dados[0][1] = 1;
	}
      
      if (!info_dados[2][5])
	{
	  foex = fopen(file_param_ex_ini_l, "r");
	  if (foex == NULL) 
	    printf("ERRO: Na leitura do ficheiro %s\n", file_param_ex_ini_l);
	    
	  
	  /*carrega os parametros de orientacao externa*/
	  fscanf(foex, "%lf %lf %lf %lf %lf %lf", &Xo, &Yo, &Zo, &omega, &phi, &kappa); /*le do ficheiro*/
	  oex_add_param (oex_param, Xo, Yo, Zo, omega, phi, kappa, 1);/*carrega a estrutura*/

	  fclose(foex);
	  
	  info_dados[2][5] = 1;
	}

      f_lista_coo_i = file_l_c_i_l;
      f_lista_coo_t = file_l_c_t_l;
      npfs = l_c_l;

      break;
      
    case 'r':

      if (!info_dados[1][1])
	{
	  foin = fopen(file_param_in_r, "r");
	  if (foin == NULL) 
	    printf("ERRO: Na leitura do ficheiro %s\n", file_param_in_r);
	  
	  /*carrega os parametros da orientacao interna*/
	  fscanf(foin, "%lf %lf %lf", &xo, &yo, &f); /*le do ficheiro*/
	  oex_add_oin_param (oin_param, f, xo, yo);       /*carrega a estrutura*/


	  fclose(foin);
	  info_dados[1][1] = 1;
	}

      if(!info_dados[3][5])
	{	  
	  foex = fopen(file_param_ex_ini_r, "r");
	  if (foex == NULL) 
	    printf("ERRO: Na leitura do ficheiro %s\n", file_param_ex_ini_r);
	  
	  
	  /*carrega os parametros de orientacao externa*/
	  fscanf(foex, "%lf %lf %lf %lf %lf %lf", &Xo, &Yo, &Zo, &omega, &phi, &kappa); /*le do ficheiro*/
	  oex_add_param (oex_param, Xo, Yo, Zo, omega, phi, kappa, 1);/*carrega a estrutura*/

	  fclose(foex);
	  info_dados[3][5] = 1;

 	}

      f_lista_coo_i = file_l_c_i_r;
      f_lista_coo_t = file_l_c_t_r;
      npfs = l_c_r;

      break;
    }

  

  /*executa o calculo da orientacao interna*/
  orientacao_externa_foto (f_lista_coo_i, f_lista_coo_t, npfs, oin_param, oex_param);

}

void matriz_fundamental(double *FM)
{
  int np = l_c_i_c; /*numero de pontos comuns*/

  double *left = ml_alocar_M (np, 2);
  double *right = ml_alocar_M (np, 2);

  ep_readm (file_l_c_i_l_c, left, np, 2);
  ep_readm (file_l_c_i_r_c, right, np, 2);

  ep_fundamentalMatrix (np, left, right, FM);

  ml_free_M (left);
  ml_free_M (right);
}

/*
 * DIS:   A procura eh feita para um ponto
 * PARAM: matriz fundamental; componente R,G,B; 
          dimensao do template;
          margem em y para a epipolar
	  limite minimo da janela de procura em x
	  limite maximo da janela de procura em x
 */
int correspondencia_auto_pontual (int *fl, double *FM, char componente, 
				  int dim_template, 
				  int margem_y, 
				  int x_min, int x_max,
				  double *homologos, int *n_homologos)
{
  p_ph_photo foto_l = NULL;
  p_ph_photo foto_r = NULL;
  
  int height_l, width_l; /*dimensoes da imagem esquerda*/
  int height_r, width_r; /*dimensoes da imagem direita*/

  int fr[2];      /*coordenadas foto do ponto encontrado*/
  double cc;      /*coeficiente de corelacao final*/
  
  unsigned char **template; /*template*/


  
  /*carregamento da imagem esquerda*/
  foto_l = ph_read_jpeg_file(file_image_l);
  if( foto_l != NULL ) 
    printf("INFO: Processamento JPEG completo\n");
  else
    {
      printf("FALHA: Processamento JPEG: %s\n", file_image_l);
      return -1;
    }

  /*carregamento da imagem direita*/  
  foto_r = ph_read_jpeg_file(file_image_r); 
  if( foto_r != NULL ) 
    printf("INFO: Processamento JPEG completo\n");
  else
    {
      printf("FALHA: Processamento JPEG: %s\n", file_image_r);
      return -1;
    }  

  /*levantamento das dimensoes das imagens*/
  height_l = ph_height_photo(foto_l);
  width_l = ph_width_photo(foto_l); 
  height_r = ph_height_photo(foto_r);
  width_r = ph_width_photo(foto_r);  


  


  /*geracao do template*/
  template = abm_alocar_sub_matrix (ph_componente_photo (foto_l, componente), 
				    height_l, width_l, fl[0]-dim_template, 
				    fl[0]+dim_template, 
				    fl[1]-dim_template, fl[1]+dim_template);
  
  /*cross correlation restringida pela epipolar*/
  if (abm_cross_correlation_epi (ph_componente_photo (foto_r, componente), 
				 height_r, width_r, 
				 template, 
				 2*dim_template+1, 2*dim_template+1, 
				 FM, fl, 
				 margem_y, x_min, x_max, 0.90,
				 fr, &cc))
    { 
      

      homologos[POS(0,0,5)] = fl[0];
      homologos[POS(0,1,5)] = fl[1];

      homologos[POS(0,2,5)] = fr[0];
      homologos[POS(0,3,5)] = fr[1];

      homologos[POS(0,4,5)] = cc;

      *n_homologos = 1;

      printf("\nENCONTROU: cc = %lf\n          H: %d   W: %d\n", 
	     cc, fr[0], fr[1]);
    }

  else
    printf ("\nNAO ENCONTROU\n");
  
  /*liberta a memoria alocada para as imagens*/
  ph_libertar_photo(foto_l);
  ph_libertar_photo(foto_r);

  abm_libertar_sub_matrix(template);
  
  return 0;
}


/*
 * DIS:   A procura eh feita para uma janela de procura
 * PARAM: matriz fundamental; componente R,G,B; 
          dimensao do template;
          margem em y para a epipolar
	  limite minimo da janela de procura em x
	  limite maximo da janela de procura em x
 */
int correspondencia_auto_area (double *FM, char componente, 
			       int dim_template, 
			       int margem_y, 
			       int x_min_l, int x_max_l,
			       int y_min_l, int y_max_l,
			       int x_min_r, int x_max_r,
			       double cc_min,
			       double *homologos, int *n_homologos)
{

  p_ph_photo foto_l = NULL;
  p_ph_photo foto_r = NULL;
  
  int height_l, width_l; /*dimensoes da imagem esquerda*/
  int height_r, width_r; /*dimensoes da imagem direita*/  

  /*carregamento da imagem esquerda*/
  foto_l = ph_read_jpeg_file(file_image_l);
  if( foto_l != NULL ) 
    printf("INFO: Processamento JPEG completo\n");
  else
    {
      printf("FALHA: Processamento JPEG: %s\n", file_image_l);
      return -1;
    }

  /*carregamento da imagem direita*/  
  foto_r = ph_read_jpeg_file(file_image_r); 
  if( foto_r != NULL ) 
    printf("INFO: Processamento JPEG completo\n");
  else
    {
      printf("FALHA: Processamento JPEG: %s\n", file_image_r);
      return -1;
    }  

  /*levantamento das dimensoes das imagens*/
  height_l = ph_height_photo(foto_l);
  width_l = ph_width_photo(foto_l); 
  height_r = ph_height_photo(foto_r);
  width_r = ph_width_photo(foto_r);  


  
  /**/
  if (abm_cross_correlation_epi_area (ph_componente_photo (foto_l, componente), 
				      height_l, width_l,
				      ph_componente_photo (foto_r, componente), 
				      height_r, width_r,
				      dim_template,
				      FM, margem_y, 
				      x_min_l, x_max_l,
				      y_min_l, y_max_l,
				      x_min_r, x_max_r,
				      cc_min, 
				      n_homologos, homologos))
    {
      printf("\nEncontrou %d pontos homologos\n", *n_homologos);
    }




  return 0;
}



/*
 *
 */
double *correspondencia_auto (double *FM, int *n_homologos)
{
  int p_or_a;       /*pontual - 1 | area - 2*/
  char comp;        /*R, G, B*/
  int margem_y;     /*margem em y*/
  int dim_template; /*dimensao do template*/
  int x_min_l, x_max_l; /*limite minimo e maximo em x da janela de procura*/
  int y_min_l, y_max_l; /*limite minimo e maximo em y da janela de procura*/
  int x_min_r, x_max_r;
  double cc_min;    /*valor minimo para o coeficiente de correlacao*/

  double *homologos = NULL;

  int fl[2];

  do
    {
      printf("\n1. Pontual\n");
      printf("2. Area\n");
      printf("3. Concluido\n");
      printf("Opcao: ");

      scanf("%d", &p_or_a);
      __fpurge(stdin); /*limpar o buffer*/
      assert(p_or_a >= 1 && p_or_a <= 3);

      if(p_or_a != 3) /*se a opcao nao for concluido*/
	{
	  /*pede a componente em que vai trabalhar*/
	  printf("\nComponete (R,G,B): "); 
	  __fpurge(stdin);               /*limpa o buffer*/
	  scanf("%c", &comp);           
	  assert(comp == 'R' || comp == 'G' || comp == 'B');

	  /* pede a dimensao do template*/
	  printf("\nDimensao do template (ex. 5): "); 
	  __fpurge(stdin);               /*limpa o buffer*/
	  scanf("%d", &dim_template);           
	  assert(dim_template >= 0 && dim_template <= 30);

	  /* pede a margem em y em torno da epipolar*/
	  printf("\nMargem em y (ex. 15): "); 
	  __fpurge(stdin);               /*limpa o buffer*/
	  scanf("%d", &margem_y);           
	  assert(margem_y >= 0 && margem_y <= 30);

	  /* Limites para a janela de procura em x*/
	  printf("\nLimite minimo da janela de procura em x (ex. 50): "); 
	  __fpurge(stdin);               /*limpa o buffer*/
	  scanf("%d", &x_min_r);
	  printf("Limite maximo da janela de procura em x (ex. 100): ");
	  __fpurge(stdin);               /*limpa o buffer*/
	  scanf("%d", &x_max_r);
	  assert(x_max_r > x_min_r);

	}

      switch (p_or_a)
	{
	case 1:
	  assert(FM != NULL); /*tem de existir matriz fundamental*/

	  /*pedido ao utilizador das coordenadas do ponto a procurar*/
	  printf("\nCoordenadas imagem esqueda (h w): ");
	  scanf("%d %d", &fl[0], &fl[1]);

	  /*alocacao de memoria para guradar resultado*/
	  if (homologos == NULL)
	    {
	      homologos = ml_alocar_M(1, 5);
	      *n_homologos = 0;
	    }
	  else
	    {
	      ml_free_M(homologos);
	      homologos = ml_alocar_M(1, 5);
	      *n_homologos = 0;
	    }

	  correspondencia_auto_pontual (fl, FM, comp, dim_template, 
					margem_y, x_min_r, x_max_r,
					homologos, n_homologos);



	  break;
	case 2:
	  assert(FM != NULL); /*tem de existir matriz fundamental*/

	  /* Limites para a janela de procura em x*/
	  printf("\nLimite minimo em x (ex. 50): "); 
	  __fpurge(stdin);               /*limpa o buffer*/
	  scanf("%d", &x_min_l);
	  printf("Limite maximo x (ex. 100): ");
	  __fpurge(stdin);               /*limpa o buffer*/
	  scanf("%d", &x_max_l);
	  assert(x_max_l > x_min_l);

	  /* Limites para a janela de procura em y*/
	  printf("\nLimite minimo em y (ex. 50): "); 
	  __fpurge(stdin);               /*limpa o buffer*/
	  scanf("%d", &y_min_l);
	  printf("Limite maximo y (ex. 100): ");
	  __fpurge(stdin);               /*limpa o buffer*/
	  scanf("%d", &y_max_l);
	  assert(y_max_l > y_min_l);

	  printf("\nLimite minimo para coeficiente de correlacao (ex. 0.95): ");
	  __fpurge(stdin);               /*limpa o buffer*/
	  scanf("%lf", &cc_min);
	  assert(cc_min >= 0 && cc_min <= 1);

	  /*alocacao de memoria para guradar resultado*/
	  if (homologos == NULL)
	    homologos = ml_alocar_M ((x_max_l-x_min_l)*(y_max_l-y_min_l), 5);
	  else
	    {
	      ml_free_M(homologos);
	      homologos = ml_alocar_M ((x_max_l-x_min_l)*(y_max_l-y_min_l), 5);
	    }
	  
	  correspondencia_auto_area(FM, comp, dim_template, 
				    margem_y, x_min_l, x_max_l, 
				    y_min_l, y_max_l,
				    x_min_r, x_max_r,
				    cc_min,
				    homologos, n_homologos);

	  

	  break;
	}

    } while (p_or_a != 3);

  return homologos;
}

/*TEMP*/
void escreve_ascii (double *res, int n)
{
  int i;
  
  FILE *f = fopen("res.txt", "w");

  for(i = 0; i < n; i++)
    {
      fprintf(f, "%6.1lf  %6.1lf  %6.1lf  %6.1lf  %6.3lf\n", 
	      res[POS(i,0,5)],
	      res[POS(i,1,5)],
	      res[POS(i,2,5)],
	      res[POS(i,3,5)],
	      res[POS(i,4,5)]);
    }
  
  fclose(f);
}

/* MAIN */

int main (void)
{
  double *FM = NULL;
  int o; /*utilizado para opcoes*/

  /*orientacoes para a foto da esquerda*/
  p_oex_oin_param oin_param_l = NULL;
  p_oex_param oex_param_l = NULL;
  
  /*orientacoes para a foto da direita*/
  p_oex_oin_param oin_param_r = NULL;
  p_oex_param oex_param_r = NULL;

  /*lista das coordenadas dos pontos homologos detctados*/
  double *homologos = NULL;
  int n_homologos = 0;


  do
    {
      /*system("clear");*/

      op = display_main_menu ();
      
      switch(op)
	{

	case 1: /* 1. Ficheiros de dados */
	  ficheiros_dados ();
	  break;

	case 2: /* 2. Orientacao Externa */
	  
	  do
	    {
	      printf("\n1. Esquerda\n");
	      printf("2. Direita\n");
	      printf("3. Concluido\n");
	      printf("Opcao: ");
	      
	      scanf("%d", &o);
	      assert(o == 1 || o == 2 || o == 3);
	      
	      switch (o)
		{
		case 1:
		  if (oin_param_l == NULL)
		    oin_param_l = oex_alocar_oin_param ();
		  if (oex_param_l == NULL)
		    oex_param_l = oex_alocar_param ();

		  orientacao_externa ('l', oin_param_l, oex_param_l);
		  break;
		 
		case 2:
		  if (oin_param_r == NULL)
		    oin_param_r = oex_alocar_oin_param ();
		  if (oex_param_r == NULL)
		    oex_param_r = oex_alocar_param ();

		  orientacao_externa ('r', oin_param_r, oex_param_r);
		  break;
		}
	    } while (o != 3);
  
	  
	  break;

	case 3: /* 3. Matriz Fundamental */

	  if (FM == NULL) 
	    FM = ml_alocar_M (3, 3); /* alocagem */

	  matriz_fundamental(FM); /*calculo da matriz fundamental*/

#ifdef DEBUG
	  /*impressao da matriz fundamental*/
	  printf("\n");
	  oex_showm (FM, 3, 3);
#endif /*DEBUG*/  

	  break;

	case 4: /* 4. Correspondencia automatica */

	  /*para o caso deste passo ja ter sido executado antes*/
	  if (homologos != NULL) 
	    {
	      ml_free_M(homologos);
	      n_homologos = 0;
	    }

	  homologos = correspondencia_auto (FM, &n_homologos);

#ifdef DEBUG
	  escreve_ascii (homologos, n_homologos);
#endif /*DEBUG*/

	  break;

	case 5: /* 5. Restituicao  */

	  break;

	case 6: /* 6. Sobre StereoM */
	  
	  sobre_stereom ();
	  break;

	}

    } while (op != 7);

  /*Libertacao de memoria*/

  if (oin_param_l != NULL) oex_libertar_oin_param (oin_param_l);
  if (oex_param_l != NULL) oex_libertar_param (oex_param_l);
  if (oin_param_r != NULL) oex_libertar_oin_param (oin_param_r);
  if (oex_param_r != NULL) oex_libertar_param (oex_param_r);
  if (FM != NULL) free(FM);
  
  if (homologos != NULL) ml_free_M(homologos);

  /* fim */
  return 0;
}
