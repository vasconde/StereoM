/*
 * StereoM
 * Nome: stereom.c
 * Autor: vasco conde
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "abmatching.h"
#include "photojpeglib.h"


int op;

char *file_l_c_i_l = "";
char *file_l_c_i_r = "";
char *file_l_c_t_l = "";
char *file_l_c_t_r = "";

char *file_l_c_i_c = "";



char *file_image_l = "";
char *file_image_r = "";

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
  do {
    apresentar_dados();
    printf("\n1. Alterar\n");
    printf("2. Carregar para a memoria\n");
    scanf("%d");
    while (op != );
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

int main (void)
{

  dados_dis[0] = 0; /*ficheiro coordenadas img esquerda*/
  dados_dis[1] = 0; /*ficheiro coordenadas img direita*/
  dados_dis[2] = 0; /*ficheiro coordenadas terr esquerda*/
  dados_dis[3] = 0; /*ficheiro coordenadas terr direita*/
  dados_dis[4] = 0; /*ficheiro coordenadas img comuns*/
  dados_dis[5] = 0; /*ficheiro imagem esquerda*/
  dados_dis[6] = 0; /*ficheiro imagem direita*/
  
  char infilename[20];
  char infilenametemplate[20];
  p_ph_photo foto1 = NULL;
  p_ph_photo template = NULL;

  int height, width;

  double epil[] = {-100,1,-0.6254};
  int ty = 3;

  do
    {
      op = display_main_menu ();
      
      switch(op)
	{
	case 1:
	  carregar_dados ();
	  break;
	case 6:
	  sobre_stereom ();
	  break;
	}

    } while (op != 7);

  return 0;

  printf("Ficheiro JPEG: ");
  scanf("%s", infilename);
  printf("Template JPEG: ");
  scanf("%s", infilenametemplate);
  /* infilename = "test.jpg"; */
  
  foto1 = ph_read_jpeg_file( infilename );
  template = ph_read_jpeg_file( infilenametemplate );

  if( foto1 != NULL ) 
    {
      printf("INFO: Processamento JPEG completo\n");
    }
  else
    {
      printf("FALHA: Processamento JPEG\n");
      return -1;
    }

  height = ph_height_photo(foto1);
  width = ph_width_photo(foto1);  

  /*abm_cross_correlation (ph_componente_photo (foto1, 'R'), height, width, 
			 ph_componente_photo (template, 'R'), ph_height_photo(template), 
			 ph_width_photo(template));*/

  abm_cross_correlation_epi (ph_componente_photo (foto1, 'R'), height, width, 
			     ph_componente_photo (template, 'R'), ph_height_photo(template), 
			     ph_width_photo(template), 
			     epil, ty);

  

  ph_libertar_photo(foto1);

  printf("INFO: Fim de Programa\n");
  return 0;

}
