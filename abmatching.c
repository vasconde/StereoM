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

void abm_cross_correlation_epi (unsigned char **im, int him, int wim, 
				unsigned char **t, int ht, int wt, 
				double *epil, int ty)
{
  int i, j;
  unsigned char **window;
  int halfw = (wt-1)/2;
  int halfh = (ht-1)/2;
  double cc;
  double y;
  int int_y;
 
  for(j = halfw; j < wim - halfw; j++)
    {
      /*ax+by+c=0; y = (-c-ax)/b*/
      y = (-epil[2]-epil[0]*(double)j)/epil[1];
      int_y = (int)y;

      if((int_y-ty)-halfh < 0 || int_y+ty+halfh > him)
	;
      else 
       for(i = int_y-ty; i <= int_y+ty; i++)
	 {
	   window = abm_alocar_sub_matrix (im, him, wim, i-halfh, i+halfh, 
					   j-halfw, j+halfw);
	   cc = abm_coef_correlacao (window, t, ht, wt);
	   abm_libertar_sub_matrix(window);
	   if (cc >= 0.99)
	     printf("ENCONTROU: cc = %lf\n          H: %d   W: %d\n", cc, i, j);
	 }
    }
  
}

