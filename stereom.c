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

int main (void)
{
  char infilename[20];
  char infilenametemplate[20];
  p_ph_photo foto1 = NULL;
  p_ph_photo template = NULL;

  int height, width;

  double epil[] = {0,1,-267};
  int ty = 3;
  
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
