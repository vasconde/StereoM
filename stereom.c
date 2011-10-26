/*
 * StereoM
 * Nome: stereom.c
 * Autor: vasco conde
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "imagenslib.h"

double im_media (unsigned char **im, int height, int width)
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

double im_desvio_padrao (unsigned char **im, int height, int width)
{
  int i, j;
  double media;
  double despad = 0;
  int n_px = height * width;

  media = im_media(im, height, width);

  for(i=0; i < height; i++)
    {
      for(j=0; j < width; j++)
	despad += pow(((double)im[i][j] - media),2);
    }
  despad /= (n_px - 1);
  despad = sqrt(despad);
  
  return despad;
}

double im_covariancia (unsigned char **im1, unsigned char **im2, int height, int width)
{
  int i, j;
  double media1, media2;
  double cov = 0;
  int n_px = height * width;

  media1 = im_media(im1, height, width);
  media2 = im_media(im2, height, width);

  for(i=0; i < height; i++)
    for(j=0; j < width; j++)
      cov += ((double)im1[i][j] - media1) * ((double)im2[i][j] - media2);
  cov /= (n_px - 1);

  return cov;
}

double coef_correlacao (unsigned char **im1, unsigned char **im2, int height, int width)
{
  double despad1, despad2;
  double cov;
  
  despad1 = im_desvio_padrao(im1, height, width);
  despad2 = im_desvio_padrao(im2, height, width);

  cov = im_covariancia (im1, im2, height, width);

  return cov / (despad1 * despad2);
}

unsigned char **alocar_sub_matrix (unsigned char **m, int height, int width, int hi, int hf, int wi, int wf)
{
  int i;
  int height_sub = hf-hi+1;
  unsigned char **sub = (unsigned char **) calloc (height_sub, sizeof(unsigned char *));
  
  for(i = 0; i < height_sub; i++)
    sub[i] = &m[hi+i][wi];
    
  return sub;
}

void libertar_sub_matrix (unsigned char **sub)
{
  free(sub);
}

void cross_correlation (unsigned char **im, int him, int wim, unsigned char **t, int ht, int wt)
{
  int i, j;
  unsigned char **window;
  double cc;

  for(i = 0; i <= him - ht; i++)
    for(j = 0; j <= wim - wt; j++)
      {
	window = alocar_sub_matrix (im, him, wim, i, i+ht-1, j, j+wt-1);
	cc = coef_correlacao (window, t, ht, wt);
	libertar_sub_matrix(window);
	if (cc >= 0.99)
	  printf("ENCONTROU: cc = %lf\n          H: %d   W: %d\n", cc, i, j);
      }
}

int main (void)
{
  char infilename[20];
  char infilenametemplate[20];
  p_foto foto1 = NULL;
  p_foto template = NULL;
  
  unsigned char **area_busca;

  int height, width;
  
  printf("Ficheiro JPEG: ");
  scanf("%s", infilename);
  printf("Template JPEG: ");
  scanf("%s", infilenametemplate);
  /* infilename = "test.jpg"; */
  
  foto1 = read_jpeg_file( infilename );
  template = read_jpeg_file( infilenametemplate );

  if( foto1 != NULL ) 
    {
      printf("INFO: Processamento JPEG completo\n");
    }
  else
    {
      printf("FALHA: Processamento JPEG\n");
      return -1;
    }

  height = height_foto(foto1);
  width = width_foto(foto1);  

  area_busca = alocar_sub_matrix (componente_foto (foto1, 'R'), height, width, 0+1, 6+1, 0+1, 6+1);

  img_ascii ("dados/R.txt", componente_foto (foto1, 'R'), height, width);
  img_ascii ("dados/G.txt", componente_foto (foto1, 'G'), height, width);
  img_ascii ("dados/B.txt", componente_foto (foto1, 'B'), height, width);

  img_ascii ("dados/sub.txt", area_busca, 7, 7);

  printf("MEDIA: %lf\n", im_media(componente_foto (foto1, 'R'), height, width));
  printf("DESVIO PADRAO: %lf\n", im_desvio_padrao(componente_foto (foto1, 'R'), height, width));
  printf("COVARIANCIA: %lf\n", im_covariancia(componente_foto (foto1, 'R'),componente_foto (foto1, 'R'), height, width));
  printf("COEF. CORR.: %lf\n", coef_correlacao(componente_foto (foto1, 'R'),componente_foto (foto1, 'R'), height, width));

  cross_correlation (componente_foto (foto1, 'R'), height, width, 
		     componente_foto (template, 'R'), height_foto(template), width_foto(template));

  

  libertar_foto(foto1);

  printf("INFO: Fim de Programa\n");
  return 0;

}
