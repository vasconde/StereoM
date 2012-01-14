/*
 * Implementacao da ABMATCHING
 * Nome: abmatching.c
 * Autor: vasco conde
 */

/*
 * BIBLIOTECAS AUXILIARES
 */
#include <stdio.h>   /* c padrao */
#include <stdlib.h>  /* c padrao */
#include <math.h>
#include <assert.h> /*assert(codition)*/

#include "photojpeglib.h"

#include "abmatching.h"


double abm_media (unsigned char **im, int height, int width)
{
  int i, j;
  double media = 0;
  int n_px = height * width;

  for(i=0; i < height; i++)
    {
      for(j=0; j < width; j++)
	media += (double)im[i][j];
    }
  media /= n_px;
  
  return media;
}

double abm_desvio_padrao (unsigned char **im, int height, int width)
{
  int i, j;
  double media;
  double despad = 0;
  int n_px = height * width;

  media = abm_media(im, height, width);

  for(i=0; i < height; i++)
    {
      for(j=0; j < width; j++)
	despad += pow(((double)im[i][j] - media),2);
    }
  despad /= (n_px - 1);
  despad = sqrt(despad);
  
  return despad;
}

double abm_covariancia (unsigned char **im1, unsigned char **im2, int height, int width)
{
  int i, j;
  double media1, media2;
  double cov = 0;
  int n_px = height * width;

  media1 = abm_media(im1, height, width);
  media2 = abm_media(im2, height, width);

  for(i=0; i < height; i++)
    for(j=0; j < width; j++)
      cov += ((double)im1[i][j] - media1) * ((double)im2[i][j] - media2);
  cov /= (n_px - 1);

  return cov;
}

double abm_coef_correlacao (unsigned char **im1, unsigned char **im2, int height, int width)
{
  double despad1, despad2;
  double cov;
  
  despad1 = abm_desvio_padrao(im1, height, width);
  despad2 = abm_desvio_padrao(im2, height, width);

  cov = abm_covariancia (im1, im2, height, width);

  return cov / (despad1 * despad2);
}

unsigned char **abm_alocar_sub_matrix (unsigned char **m, int height, int width, int hi, int hf, int wi, int wf)
{
  int i;
  int height_sub = hf-hi+1;
  unsigned char **sub = (unsigned char **) calloc (height_sub, sizeof(unsigned char *));
  
  for(i = 0; i < height_sub; i++)
    sub[i] = &m[hi+i][wi];
    
  return sub;
}

void abm_libertar_sub_matrix (unsigned char **sub)
{
  free(sub);
}

void abm_cross_correlation (unsigned char **im, int him, int wim, unsigned char **t, int ht, int wt)
{
  int i, j;
  unsigned char **window;
  int halfw = (wt-1)/2;
  int halfh = (ht-1)/2;
  double cc;

  for(i = halfh; i < him - halfh; i++)
    for(j = halfw; j < wim - halfw; j++)
      {
	window = abm_alocar_sub_matrix (im, him, wim, i-halfh, i+halfh, 
					j-halfw, j+halfw);
	cc = abm_coef_correlacao (window, t, ht, wt);
	abm_libertar_sub_matrix(window);
	if (cc >= 0.99)
	  printf("ENCONTROU: cc = %lf\n          H: %d   W: %d\n", cc, i, j);
      }
}

/*
 * DES:   Image matching restrigido ah epipolar
 * PARAM: imagem, h da imagem, w da imagem,
 *        template, h do template, w do template,
 *        parametros da recta epipolar, intervalo em +/- em y,
 *        limite minimo em x e limite maximo em x da janela de procura
 */
void abm_cross_correlation_epi (unsigned char **im, int him, int wim, 
				unsigned char **t, int ht, int wt, 
				double *epil, int ty, int x_min, int x_max)
{
  int i, j; /*ciclos*/
  int inicio_x, fim_x;

  unsigned char **window; /*janela de matching*/
  int halfw = (wt-1)/2;   /*metade da dimensao do template em w*/
  int halfh = (ht-1)/2;   /*metade da dimensao do template em h*/

  double cc;              /*coeficiente de correlacao instatantaneo*/
  double cc_max = 0;      /*coeficiente de correlacao maximo*/
  int coo_cc_max[2];   /*coordenadas do cc maximo*/

  double y;               /*y da epipolar para um dado x*/
  int int_y;              /*o y anterior mas passado a inteiro*/
  
  /*garantir que os limites para x
    nao excedem a dimensao da imagem*/
  assert(x_min >= 0 && x_max <= wim);

  /*define os limites da busca em x
    tendo em conta do tamanho do template*/
  if (x_min-halfw >= 0)
    inicio_x = x_min;
  else
    inicio_x = x_min + halfw;

  if (x_max + halfw <= wim)
    fim_x = x_max;
  else
    fim_x = x_max - halfw;

  /*ciclo para percorrer a janela de procura e encontrar
    a posicao para a qual o cc eh maximo*/
  for(j = inicio_x; j < fim_x; j++)
    {
      /*recta epipolar
	ax+by+c=0; y = (-c-ax)/b*/
      y = (-epil[2]-epil[0]*(double)j)/epil[1];
      int_y = (int)y;

      /*verificar se a epipolar neste ponto
	nao sai dos limites superiror e inferiror 
	da imagem da imagem*/
      if((int_y-ty)-halfh < 0 || int_y+ty+halfh > him)
	;

      else 
       for(i = int_y-ty; i <= int_y+ty; i++)
	 {
	   /*cria a janela de matching*/
	   window = abm_alocar_sub_matrix (im, him, wim, i-halfh, i+halfh, 
					   j-halfw, j+halfw);
	   
	   /*determina o coeficiente de correlacao entre a 
	     a janela de matching e o template*/
	   cc = abm_coef_correlacao (window, t, ht, wt);

	   /*actualizacao do cc maximo*/
	   if (cc > cc_max)
	     {
	       cc_max = cc;
	       coo_cc_max[0] = i;
	       coo_cc_max[1] = j;
	     }
 
	   /*destroi a janela de matching*/
	   abm_libertar_sub_matrix(window);

	 }
    }

  /*resultado*/
  if (cc_max > 0.90)
      printf("\nENCONTROU: cc = %lf\n          H: %d   W: %d\n", cc_max, coo_cc_max[0], coo_cc_max[1]);
}

