/*
 * StereoM
 * Nome: stereom.c
 * Autor: vasco conde
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <cblas.h>

#include "matrixlib.h"

#include "abmatching.h"

#include "photojpeglib.h"

#include "oex.h"

#include "epipolar.h"


int op;

char file_l_c_i_l[50] = "\0";
char file_l_c_t_l[50] = "\0";
int l_c_l = 0;

char file_l_c_i_r[50] = "dados/par/limpres/pfs_fr.txt";
char file_l_c_t_r[50] = "dados/par/limpres/pfs_tr.txt";
int l_c_r = 14;

char file_l_c_i_l_c[50] = "dados/epipolar/left.txt";
char file_l_c_i_r_c[50] = "dados/epipolar/right.txt";
int l_c_i_c = 12;

char file_image_l[50] = "dados/par/L_E00001_rect.jpg";
char file_image_r[50] = "dados/par/R_E00003_rect.jpg";

int dados_dis[7];



int display_main_menu ()
{
  int op;
  do 
    {
      printf("\n+--- STEREOM APP ---+\n\n");
      printf(" 1. Ficheiros de dados\n");
      printf(" 2. Salvar dados\n");
      printf(" 3. Orientacao Externa\n");
      printf(" 4. Matriz Fundamental\n");
      printf(" 5. Estereorestituicao automatica\n");
      printf(" 6. Sobre StereoM\n");
      printf(" 7. Sair\n");
      printf("\nopcao: ");
      scanf("%d", &op);
    } while (op < 1 || op > 7);

  return op;
}

void apresentar_dados()
{
  printf("\n*** Orientacao Externa ***\n");
  printf("  Lista de coordenadas:\n");
  printf("    Esquerda (%d):\n", l_c_l);
  printf("      Imagem: %s\n", file_l_c_i_l);
  printf("      Terreno: %s\n", file_l_c_t_l);
  printf("    Direita (%d):\n", l_c_r);
  printf("      Imagem: %s\n", file_l_c_i_r);
  printf("      Terreno: %s\n", file_l_c_t_r);
  printf("\n*** Matriz Fundamental ***\n");
  printf("  Lista de coordenadas imagem comuns (%d):\n", l_c_i_c);
  printf("    Esquerda: %s\n", file_l_c_i_l_c);
  printf("    Direita: %s\n", file_l_c_i_r_c);
  printf("\n*** Estereorestituicao automatica ***\n");  
  printf("  Imagem esquerda: %s\n", file_image_l);
  printf("  Imagem direita: %s\n", file_image_r);
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
    switch (o)
      {
      case 1:
	printf("\n*** Orientacao Externa ***\n");
	printf("  Lista de coordenadas\n");
	printf("    Imagem da imagem esquerda: ");
	printf("ficheiro: ");
	scanf("%s", file_l_c_i_l);  
	printf("    Terreno da imagem esquerda: ");
	scanf("%s", file_l_c_t_l);
	printf("numero de pontos: ");
	scanf("%d", &l_c_l);
	printf("    Imagem da imagem direita: ");
	scanf("%s", file_l_c_i_r);
	printf("    Terreno da imagem direita:");
	scanf("%s", file_l_c_t_r);
	printf("    numero de pontos: ");
	scanf("%d", &l_c_r);
	printf("\n*** Matriz Fundamental ***\n");
	printf("  Lista de coordenadas imagem comuns: ");
	printf("    Esquerda:");
	scanf("%s", file_l_c_i_l_c);
	printf("    Direita:");
	scanf("%s", file_l_c_i_r_c);
	printf("    numero de pontos: ");
	scanf("%d", &l_c_i_c);
	printf("\n*** Estereorestituicao automatica ***\n");  
	printf("  Imagem esquerda: ");
	scanf("%s", file_image_l);
	printf("  Imagem direita: ");
	scanf("%s", file_image_r);
	break;
      default:
	break;
      }
  } while (o != 2);
}

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

 void libertar_memoria_toda ()
 {
   
 }

void orientacao_externa (char *ffoto, char *fterreno, int n, p_oex_oin_param oin_param, p_oex_param param)
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
  int np = l_c_i_c;

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

int main (void)
{
  double *FM = NULL;

  p_oex_oin_param oin_param = NULL;
  p_oex_param oex_param = NULL;

  dados_dis[0] = 0; /*ficheiro coordenadas img esquerda*/
  dados_dis[1] = 0; /*ficheiro coordenadas img direita*/
  dados_dis[2] = 0; /*ficheiro coordenadas terr esquerda*/
  dados_dis[3] = 0; /*ficheiro coordenadas terr direita*/
  dados_dis[4] = 0; /*ficheiro coordenadas img comuns*/
  dados_dis[5] = 0; /*ficheiro imagem esquerda*/
  dados_dis[6] = 0; /*ficheiro imagem direita*/
  
  do
    {
      /*system("clear");*/

      op = display_main_menu ();
      
      switch(op)
	{
	case 1:
	  ficheiros_dados ();
	  break;
	case 2:

	  break;
	case 3:
	  /*falta pedir os parametros iniciais e separar por direita e esquerda*/
	  if (oin_param == NULL)
	    oin_param = oex_alocar_oin_param ();
	  if (oex_param == NULL)
	    oex_param = oex_alocar_param ();

	  oex_add_oin_param (oin_param, (2524.980712085470259+2512.961041523109998)/2, 1043.777785082072114, 782.329224502696547);
	  
	  oex_add_param (oex_param, 3, 1, 9, 0.0, 0.0, 0.0, 1);

	  orientacao_externa (file_l_c_i_r, file_l_c_t_r, 14, oin_param, oex_param);
	  break;
	case 4:
	  if (FM == NULL) FM = ml_alocar_M (3, 3);
	  matriz_fundamental(FM);
	  oex_showm (FM, 3, 3);
	  break;
	case 5:

	  estereorestituicao_auto (FM, 10);
	  break;
	case 6:
	  
	  sobre_stereom ();
	  break;
	}

    } while (op != 7);


  if (oin_param != NULL) oex_libertar_oin_param (oin_param);
  if (oex_param != NULL) oex_libertar_param (oex_param);
  if (FM != NULL) free(FM);
  

  return 0;
}
