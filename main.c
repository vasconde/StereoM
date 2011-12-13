/*
 * StereoM
 * Nome: stereom.c
 * Autor: vasco conde
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "matrixlib.h"

#include "abmatching.h"

#include "photojpeglib.h"

#include "oex.h"


int op;

char file_l_c_i_l[50] = "\0";
char file_l_c_i_r[50] = "dados/par/limpres/pfs_fr.txt";
char file_l_c_t_l[50] = "\0";
char file_l_c_t_r[50] = "dados/par/limpres/pfs_tr.txt";

char file_l_c_i_c[50] = "\0";

char file_image_l[50] = "dados/par/L_E00001_rect.jpg";
char file_image_r[50] = "dados/par/R_E00003_rect.jpg";

int dados_dis[7];

int display_main_menu ()
{
  int op;
  do 
    {
      printf("\n+--- STEREOM APP ---+\n\n");
      printf(" 1. Carregar dados\n");
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
  printf("  Lista de coordenadas\n");
  printf("    Imagem da imagem esquerda: %s\n", file_l_c_i_l);
  printf("    Imagem da imagem direita: %s\n", file_l_c_i_r);
  printf("    Terreno da imagem esquerda: %s\n", file_l_c_t_l);
  printf("    Terreno da imagem direita: %s\n", file_l_c_t_r);
  printf("\n*** Matriz Fundamental ***\n");
  printf("  Lista de coordenadas imagem comuns: %s\n", file_l_c_i_c);
  printf("\n*** Estereorestituicao automatica ***\n");  
  printf("  Imagem esquerda: %s\n", file_image_l);
  printf("  Imagem direita: %s\n", file_image_r);
}

void carregar_dados ()
{
  int o;
  do {
    apresentar_dados();
    printf("\n1. Alterar\n");
    printf("2. Carregar para a memoria\n");
    printf("3. Concluido\n");
    printf("Opcao: ");
    scanf("%d", &o);
    switch (o)
      {
      case 1:
	printf("\n*** Orientacao Externa ***\n");
	printf("  Lista de coordenadas\n");
	printf("    Imagem da imagem esquerda: ");
	scanf("%s", file_l_c_i_l);
	printf("    Imagem da imagem direita: ");
	scanf("%s", file_l_c_i_r);
	printf("    Terreno da imagem esquerda: ");
	scanf("%s", file_l_c_t_l);
	printf("    Terreno da imagem direita:");
	scanf("%s", file_l_c_t_r);
	printf("\n*** Matriz Fundamental ***\n");
	printf("  Lista de coordenadas imagem comuns: ");
	scanf("%s", file_l_c_i_c);
	printf("\n*** Estereorestituicao automatica ***\n");  
	printf("  Imagem esquerda: ");
	scanf("%s", file_image_l);
	printf("  Imagem direita: ");
	scanf("%s", file_image_r);
	break;
      case 2:
	/*alocar e carregar memoria*/
	break;
      default:
	break;
      }
  } while (o != 3);
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

void orientacao_externa (char *ffoto, char *fterreno, int n)
{
  /*
  char * ffoto = "dados/par/limpres/pfs_fr.txt";
  char * fterreno = "dados/par/limpres/pfs_tr.txt";
  */

  /*int n = 14;*/

  p_oex_oin_param oin_param = oex_alocar_oin_param ();
  p_oex_param param = oex_alocar_param ();
  p_oex_pfs pfs = oex_alocar_pfs ();

  double terreno[n*3];
  double foto[n*2];

  /*
   * Carregar dados
   */
  /*oex_add_oin_param (oin_param, (2527.01371+2515.01405)/2.0, 1041.17490, 782.66840);*/
  oex_add_oin_param (oin_param, (2524.980712085470259+2512.961041523109998)/2, 1043.777785082072114, 782.329224502696547);

  oex_add_param (param, 3, 1, 9, 0.0, 0.0, 0.0, 1);
  
  oex_carrega_coo(terreno, n*3, fterreno, 't');
  oex_carrega_coo(foto, n*2, ffoto, 'f');
  
  oex_add_pfs (pfs, n, terreno, foto);

  oex_compute (n, oin_param, param, pfs);

  oex_libertar_oin_param (oin_param);
  oex_libertar_param (param);
  oex_libertar_pfs (pfs);

}

void matriz_fundamental()
{

}

int estereorestituicao ()
{
  p_ph_photo foto_l = NULL;
  p_ph_photo foto_r = NULL;

  int height_l, width_l;
  int height_r, width_r;

  double epil[] = {-100,1,-0.6254};
  int ty = 3;


  foto_l = ph_read_jpeg_file(file_image_l);
  foto_r = ph_read_jpeg_file(file_image_r);
  
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
	  carregar_dados ();
	  break;
	case 2:

	  break;
	case 3:
	  orientacao_externa (file_l_c_i_r, file_l_c_t_r, 14);
	  break;
	case 4:
	  
	  matriz_fundamental();
	  break;
	case 5:

	  estereorestituicao ();
	  break;
	case 6:
	  
	  sobre_stereom ();
	  break;
	}

    } while (op != 7);

  return 0;
}
