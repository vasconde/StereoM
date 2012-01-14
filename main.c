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
int l_c_i_c = 20; /*list coordinates image common*/
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
      printf(" 4. Estereorestituicao automatica\n");
      printf(" 5. Sobre StereoM\n");
      printf(" 6. Sair\n");
      printf("\nopcao: ");
      scanf("%d", &op);
    } while (op < 1 || op > 6);

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
int estereorestituicao_pontual (double *FM, char componente, int dim_template, 
				int margem_y, int x_min, int x_max)
{
  p_ph_photo foto_l = NULL;
  p_ph_photo foto_r = NULL;
  
  int height_l, width_l; /*dimensoes da imagem esquerda*/
  int height_r, width_r; /*dimensoes da imagem direita*/
  
  double epil[3]; /*parametros da recta epipolar*/
  int fl [2];     /*coordenas foto do ponto a procurar*/
  int ty = 3;
  
  int i,j; /*para ciclos*/
  
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

  /*
  printf("%d\n", ph_componente_photo (foto_r, 'R')[578][1263]);
  printf("%d\n", ph_componente_photo (foto_r, 'R')[1027][1587]);
  printf("%d\n", ph_componente_photo (foto_r, 'R')[751][363]);
  printf("%d\n", ph_componente_photo (foto_r, 'R')[1195][1607]);
  */

  /*return -1;*/

  /*levantamento das dimensoes das imagens*/
  height_l = ph_height_photo(foto_l);
  width_l = ph_width_photo(foto_l); 
  height_r = ph_height_photo(foto_r);
  width_r = ph_width_photo(foto_r);  

  /*pedido ao utilizador das coordenadas do ponto a procurar*/
  printf("Coordenadas imagem esqueda: ");
  scanf("%d %d", &fl[0], &fl[1]);
  
  /* calculo dos parametros da recta epipolar 
   * atencao atencao para a epipolar n entra l,c mas c,l*/
  ep_lepipolar (FM, fl[1], fl[0], &epil[0], &epil[1], &epil[2]);

#ifdef DEBUG
  /*imprime os parametros*/
  printf("\na = %lf\nb = %lf\nc = %lf\n",epil[0],epil[1],epil[2]);
#endif /*DEBUG*/  

  /*geracao do template*/
  template = abm_alocar_sub_matrix (ph_componente_photo (foto_l, componente), 
				  height_l, width_l, fl[0]-dim_template, fl[0]+dim_template, 
				  fl[1]-dim_template, fl[1]+dim_template);
  
  /*cross correlation restringida pela epipolar*/
  abm_cross_correlation_epi (ph_componente_photo (foto_r, componente), 
			     height_r, width_r, 
			     template, 2*dim_template+1, 2*dim_template+1, epil, 
			     margem_y, x_min, x_max);
  

  /*abm_cross_correlation (ph_componente_photo (foto1, 'R'), height, width, 
    ph_componente_photo (template, 'R'), ph_height_photo(template), 
    ph_width_photo(template));*/
  
  
  /*
    abm_cross_correlation_epi (ph_componente_photo (foto1, 'R'), height, width, 
			     ph_componente_photo (template, 'R'), ph_height_photo(template), 
			     ph_width_photo(template), 
			     epil, ty);
  */
  
  ph_libertar_photo(foto_l);
  ph_libertar_photo(foto_r);
  
  return 0;
}

void estereorestituicao_area ()
{

}

/*
 *
 */
int estereorestituicao_auto (double *FM)
{
  int p_or_a;       /*pontual - 1 | area - 2*/
  char comp;        /*R, G, B*/
  int margem_y;     /*margem em y*/
  int dim_template; /*dimensao do template*/
  int x_min, x_max; /*limite minimo e maximo em x da janela de procura*/

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
	  printf("\nDimensao do template (ex. 10): "); 
	  __fpurge(stdin);               /*limpa o buffer*/
	  scanf("%d", &dim_template);           
	  assert(dim_template >= 0 && dim_template <= 30);

	  /* pede a margem em y em torno da epipolar*/
	  printf("\nMargem em y (ex. 15): "); 
	  __fpurge(stdin);               /*limpa o buffer*/
	  scanf("%d", &margem_y);           
	  assert(margem_y >= 0 && margem_y <= 30);

	  /* pede a margem em y em torno da epipolar*/
	  printf("\nLimite minimo da janela de procura em x (ex. 50): "); 
	  __fpurge(stdin);               /*limpa o buffer*/
	  scanf("%d", &x_min);
	  printf("Limite maximo da janela de procura em x (ex. 100): ");
	  __fpurge(stdin);               /*limpa o buffer*/
	  scanf("%d", &x_max);
	  assert(x_max > x_min);
	}

      switch (p_or_a)
	{
	case 1:
	  assert(FM != NULL); /*tem de existir matriz fundamental*/

	  estereorestituicao_pontual (FM, comp, dim_template, 
				      margem_y, x_min, x_max);

	  break;
	case 2:
	  assert(FM != NULL); /*tem de existir matriz fundamental*/

	  estereorestituicao_area();

	  break;
	}

    } while (p_or_a != 3);

  return 0;
}

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

	  if (FM == NULL) FM = ml_alocar_M (3, 3); /* alocagem */

	  matriz_fundamental(FM); /*calculo da matriz fundamental*/

#ifdef DEBUG
	  /*impressao da matriz fundamental*/
	  printf("\n");
	  oex_showm (FM, 3, 3);
#endif /*DEBUG*/  

	  break;

	case 4: /* 4. Estereorestituicao automatica */

	  estereorestituicao_auto (FM);
	  break;

	case 5: /* 5. Sobre StereoM */
	  
	  sobre_stereom ();
	  break;

	}

    } while (op != 6);

  /*Libertacao de memoria*/

  if (oin_param_l != NULL) oex_libertar_oin_param (oin_param_l);
  if (oex_param_l != NULL) oex_libertar_param (oex_param_l);
  if (oin_param_r != NULL) oex_libertar_oin_param (oin_param_r);
  if (oex_param_r != NULL) oex_libertar_param (oex_param_r);
  if (FM != NULL) free(FM);
  

  return 0;
}
