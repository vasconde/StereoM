/*
 * StereoM
 * Nome: stereom.c
 * Autor: vasco conde
 */

/*C PADRAO*/
#include <stdio.h>
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

/*CONSTANTES*/
#define MAX_FILE_NAME 75

int op;

/*
 * NOMES DOS FICHEIROS DOS DADOS DE ENTRADA
 */

char file_param_in_l[MAX_FILE_NAME]= "dados/dados/oin/oin_l.txt"; /*file parameters internal left*/
char file_param_in_r[MAX_FILE_NAME]= "dados/dados/oin/oin_r.txt"; /*file parameters internal right*/

char file_l_c_i_l[MAX_FILE_NAME] = "dados/dados/oex/pfs_fl.txt"; /*file list coordinates image left*/
char file_l_c_t_l[MAX_FILE_NAME] = "dados/dados/oex/pfs_tl.txt"; /*file list coordinates terrain left*/
int l_c_l = 24; /*list coordinates left*/
char file_param_ex_ini_l[MAX_FILE_NAME]= "dados/dados/oex/oex_l.txt"; /*file parameters external initial left*/

char file_l_c_i_r[MAX_FILE_NAME] = "dados/dados/oex/pfs_fr.txt"; /*file list coordinates image right*/
char file_l_c_t_r[MAX_FILE_NAME] = "dados/dados/oex/pfs_tr.txt"; /*file list coordinates terrain right*/
int l_c_r = 25; /*list coordinates right*/
char file_param_ex_ini_r[MAX_FILE_NAME]= "dados/dados/oex/oex_r.txt"; /*file parameters external initial right*/

char file_l_c_i_l_c[MAX_FILE_NAME] = "dados/dados/matrix/left.txt"; /*file list coordinates image left common*/
char file_l_c_i_r_c[MAX_FILE_NAME] = "dados/dados/matrix/right.txt"; /*file list coordinates image right common*/
int l_c_i_c = 12; /*list coordinates image common*/

char file_image_l[MAX_FILE_NAME] = "dados/dados/imagens/L_E00001_rect.jpg"; /*left image*/
char file_image_r[MAX_FILE_NAME] = "dados/dados/imagens/R_E00003_rect.jpg"; /*right image*/

int dados_dis[7];


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
  printf("Seccao (1,2 ou 3): ");
  
  scanf("%d", &seccao);
  assert(seccao >= 1 && seccao <= 3);

  switch (seccao)
    {
    case 1:
      printf("\n(1) *** Orientacao Externa ***\n");
      printf("            Lista de coordenadas\n");
      printf("1. Esquerda\n2. Direita\nopcao: ");
  
      scanf("%d", &op_e_d);
      assert(op_e_d == 1 || op_e_d == 2);

      switch (op_e_d)
	{
	case 1:
	 
	  printf("    Imagem da imagem esquerda: ");
	  scanf("%s", file_l_c_i_l);  
	  printf("    Terreno da imagem esquerda: ");
	  scanf("%s", file_l_c_t_l);
	  printf("numero de pontos: ");
	  scanf("%d", &l_c_l);
	  break;

	case 2:
	  printf("    Imagem da imagem direita: ");
	  scanf("%s", file_l_c_i_r);
	  printf("    Terreno da imagem direita:");
	  scanf("%s", file_l_c_t_r);
	  printf("    numero de pontos: ");
	  scanf("%d", &l_c_r);

	  break;
	}
      
      break;
    case 2:

      printf("\n(2) *** Matriz Fundamental ***\n");
      
      printf("  Lista de coordenadas imagem comuns:\n");
      printf("    Esquerda: ");
      scanf("%s", file_l_c_i_l_c);
      printf("    Direita: ");
      scanf("%s", file_l_c_i_r_c);
      printf("    numero de pontos: ");
      scanf("%d", &l_c_i_c);

      break;
    case 3:

      printf("\n(3) *** Estereorestituicao automatica ***\n");

      printf("  Imagem esquerda: ");
      scanf("%s", file_image_l);
      printf("  Imagem direita: ");
      scanf("%s", file_image_r);

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

int estereorestituicao_auto (double *FM, int tdim)
{
  p_ph_photo foto_l = NULL;
  p_ph_photo foto_r = NULL;

  int height_l, width_l;
  int height_r, width_r;

  double epil[3];
  int fl [2];
  int ty = 3;

  int i,j;

  unsigned char **window;

  foto_l = ph_read_jpeg_file(file_image_l);
  foto_r = ph_read_jpeg_file(file_image_r);

  /*
  printf("%d\n", ph_componente_photo (foto_r, 'R')[578][1263]);
  printf("%d\n", ph_componente_photo (foto_r, 'R')[1027][1587]);
  printf("%d\n", ph_componente_photo (foto_r, 'R')[751][363]);
  printf("%d\n", ph_componente_photo (foto_r, 'R')[1195][1607]);
  */

  /*return -1;*/

  if( foto_l != NULL ) 
    printf("INFO: Processamento JPEG completo\n");
  else
    {
      printf("FALHA: Processamento JPEG: %s\n", file_image_l);
      return -1;
    }

  if( foto_r != NULL ) 
    printf("INFO: Processamento JPEG completo\n");
  else
    {
      printf("FALHA: Processamento JPEG: %s\n", file_image_r);
      return -1;
    }
  
  height_l = ph_height_photo(foto_l);
  width_l = ph_width_photo(foto_l); 
 
  height_r = ph_height_photo(foto_r);
  width_r = ph_width_photo(foto_r);  

  /*TEMP*/
  printf("Coordenadas na imagem esqueda: ");
  scanf("%d %d", &fl[0], &fl[1]);
  
  /*atencao atencao para a epipolar n entra l,c mas c,l*/
  ep_lepipolar (FM, fl[1], fl[0], &epil[0], &epil[1], &epil[2]);

  printf("a = %lf\nb = %lf\nc = %lf\n",epil[0],epil[1],epil[2]);

  window = abm_alocar_sub_matrix (ph_componente_photo (foto_l, 'G'), height_l, width_l, fl[0]-tdim, fl[0]+tdim, 
					   fl[1]-tdim, fl[1]+tdim);

  abm_cross_correlation_epi (ph_componente_photo (foto_r, 'G'), height_r, width_r, 
			     window, 2*tdim+1, 2*tdim+1, epil, 15);

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

void orientacao_externa (char *nome_foin, char *nome_foex, 
			 char *f_lista_coo_t, char *f_lista_coo_i, int npfs, 
			 p_oex_oin_param oin_param, p_oex_param oex_param)
{
  double xo, yo, f;
  double Xo, Yo, Zo, omega, phi, kappa;

  FILE *foin = fopen(nome_foin, "r");
  FILE *foex = fopen(nome_foex, "r");

  if (foin == NULL) 
    {
      printf("ERRO: Na leitura do ficheiro %s\n", nome_foin);
    }
  if (foex == NULL) 
    {
      printf("ERRO: Na leitura do ficheiro %s\n", nome_foex);
    }

  /*carrega os parametros da orientacao interna*/
  fscanf(foin, "%lf %lf %lf", &xo, &yo, &f); /*le do ficheiro*/
  oex_add_oin_param (oin_param, f, xo, yo);       /*carrega a estrutura*/
  
  /*carrega os parametros de orientacao externa*/
  fscanf(foex, "%lf %lf %lf %lf %lf %lf", &Xo, &Yo, &Zo, &omega, &phi, &kappa); /*le do ficheiro*/
  oex_add_param (oex_param, Xo, Yo, Zo, omega, phi, kappa, 1);/*carrega a estrutura*/
  
  /*executa o calculo da orientacao interna*/
  orientacao_externa_foto (f_lista_coo_i, f_lista_coo_t, npfs, oin_param, oex_param);

  fclose(foin);
  fclose(foex);
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

  dados_dis[0] = 0; /* ficheiro coordenadas img esquerda    */
  dados_dis[1] = 0; /* ficheiro coordenadas img direita    */
  dados_dis[2] = 0; /* ficheiro coordenadas terr esquerda */
  dados_dis[3] = 0; /* ficheiro coordenadas terr direita */
  dados_dis[4] = 0; /* ficheiro coordenadas img comuns  */
  dados_dis[5] = 0; /* ficheiro imagem esquerda        */
  dados_dis[6] = 0; /* ficheiro imagem direita        */
  
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
		  orientacao_externa (file_param_in_l, file_param_ex_ini_l, 
				      file_l_c_t_l, file_l_c_i_l, l_c_l, 
				      oin_param_l, oex_param_l);
		  break;
		 
		case 2:
		  if (oin_param_r == NULL)
		    oin_param_r = oex_alocar_oin_param ();
		  if (oex_param_r == NULL)
		    oex_param_r = oex_alocar_param ();
		  orientacao_externa (file_param_in_r, file_param_ex_ini_r, 
				      file_l_c_t_r, file_l_c_i_r, l_c_r, 
				      oin_param_r, oex_param_r);
		  break;
		}
	    } while (o != 3);
  
	  
	  break;

	case 3: /* 3. Matriz Fundamental */
	  if (FM == NULL) FM = ml_alocar_M (3, 3);
	  matriz_fundamental(FM);
	  oex_showm (FM, 3, 3); 
	  break;

	case 4: /* 4. Estereorestituicao automatica */

	  estereorestituicao_auto (FM, 10);
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
