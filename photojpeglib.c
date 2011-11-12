/*
 * Implementacao da PHOTOJPEGLIB
 * Nome: photojpeglib.c
 * Autor: vasco conde
 */

/*
 * BIBLIOTECAS AUXILIARES
 */
#include <stdio.h>   /* c padrao */
#include <stdlib.h>  /* c padrao */

#include <jpeglib.h> /* operacoes sobre o formato jpeg  */

#include "photojpeglib.h"

struct ph_photo
{
  int height;
  int width;

  unsigned char **R;
  unsigned char **G;
  unsigned char **B;

};

struct ph_par_photo
{
  p_ph_photo photo1;
  p_ph_photo photo2;
};

p_ph_photo ph_read_jpeg_file(char *filename)
{
  unsigned char *raw_image = NULL; /* ponteiro para a imagem descompressa R,G,B,R,G,B... */
  int width;
  int height;

  /* estruturas standard para a leitura (descompressao)  */
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  /* estrutura de dados para guardar uma linha (scanline) */
  JSAMPROW row_pointer[1];

  /* leitura da ficheiro JPEG (binario) */
  FILE *infile = fopen( filename, "rb" );

  int c, j; /* temp */

  p_ph_photo photo1;

  unsigned long location = 0;
  int i = 0;
  
  if (!infile)
    {
      printf("ERRO: Abrir o ficheiro JPEG %s\n!", filename);
      return NULL;
    }

  /* here we set up the standard libjpeg error handler */
  cinfo.err = jpeg_std_error( &jerr );
  /* setup decompression process and source, then read JPEG header */
  jpeg_create_decompress( &cinfo );
  /* this makes the library read from infile */
  jpeg_stdio_src( &cinfo, infile );
  /* reading the image header which contains image information */
  jpeg_read_header( &cinfo, TRUE );

  width = cinfo.image_width;
  height = cinfo.image_height;

  photo1 = ph_alocar_photo(height,width);
  
  /* Apresentacao de alguns dados sobre o ficheiro */
  printf( "INFO: JPEG File Information: \n" );
  printf( "INFO: Image width and height: %d pixels and %d pixels.\n", cinfo.image_width, cinfo.image_height );
  printf( "INFO: Color components per pixel: %d.\n", cinfo.num_components );
  printf( "INFO: Color space: %d.\n", cinfo.jpeg_color_space );

  /* Inicio da descompressao do ficheiro JPEG */
  jpeg_start_decompress(&cinfo);

  /* Alocacao de memoria para receber os dados da imagem descomprimida */
  raw_image = (unsigned char*)malloc(cinfo.output_width*cinfo.output_height*cinfo.num_components );
  /* Alocacao de memoria para receber os dados para a scanline */
  row_pointer[0] = (unsigned char *)malloc( cinfo.output_width*cinfo.num_components );

  /* scanline a scanline a imagem eh passada para a variavel raw_image */
  while(cinfo.output_scanline < cinfo.image_height)
    {
      jpeg_read_scanlines( &cinfo, row_pointer, 1 );
      c = 0; /* temp */
      for( i=0, j=0; i< cinfo.image_width * cinfo.num_components; i++)
	{ 
	  if (c==3) c = 0; /* temp */
	  
	  raw_image[location++] = row_pointer[0][i];
	  
	  if (c == 0)
	    photo1->R[cinfo.output_scanline-1][j] = raw_image[location-1];
	  else if (c == 1)
	    photo1->G[cinfo.output_scanline-1][j] = raw_image[location-1];
	  else
	    photo1->B[cinfo.output_scanline-1][j++] = raw_image[location-1];
	  c++; /* temp */
	}
    }
 
  /* termina a descompressao, destroi os objectos, liberta a memoria e fecha os ficheiro  */
  jpeg_finish_decompress( &cinfo );
  jpeg_destroy_decompress( &cinfo );
  free( row_pointer[0] );
  free(raw_image);
  fclose( infile );

  /* termina com sucesso */
  return photo1;
}

/* alocacao de de memoria para R G B e retorno de um elemento pPhoto*/
/*p_photo alocar_photo (int height)*/

/* m * n */
unsigned char **ph_alocar_m (int h, int w)
{
  unsigned char **v;  /* ponteiro para a matriz */
  int i;    /* variavel auxiliar      */

  if (h < 1 || w < 1) /* verifica parametros recebidos */
    { 
      printf ("** Erro: Parametro invalido **\n");
      return (NULL);
    }
  /* aloca as linhas da matriz */
  v = (unsigned char **) calloc (h, sizeof(unsigned char *));
  if (v == NULL) 
    {
      printf ("** Erro: Memoria Insuficiente **");
      return (NULL);
    }
  /* aloca as colunas da matriz */
  for ( i = 0; i < h; i++ ) 
    {
      v[i] = (unsigned char*) calloc (w, sizeof(unsigned char));
      if (v[i] == NULL) 
	{
	  printf ("** Erro: Memoria Insuficiente **");
	  return (NULL);
	}
    }
  return (v); /* retorna o ponteiro para a matriz */
}

unsigned char **ph_libertar_m (int h, int w, unsigned char **img)
{
  int  i;  /* variavel auxiliar */
  if (img == NULL) 
    return (NULL);
  if (h < 1 || w < 1) {  /* verifica parametros recebidos */
    printf ("** Erro: Parametro invalido **\n");
    return (img);
  }
  for (i=0; i<h; i++) 
    free (img[i]); /* libera as linhas da matriz */
  free (img);      /* libera a matriz */
  return (NULL); /* retorna um ponteiro nulo */
}

p_ph_photo ph_alocar_photo (int height, int width)
{
  p_ph_photo photo_alocada = (p_ph_photo)malloc(sizeof(struct ph_photo));

  unsigned char **R = ph_alocar_m(height,width);
  unsigned char **G = ph_alocar_m(height,width);
  unsigned char **B = ph_alocar_m(height,width);

  photo_alocada->R = R;
  photo_alocada->G = G;
  photo_alocada->B = B;

  photo_alocada->height = height;
  photo_alocada->width = width;

  return photo_alocada;  
}

void ph_libertar_photo (p_ph_photo photo_alocada)
{
  int height = photo_alocada->height;
  int width = photo_alocada->width;

  ph_libertar_m(height, width, photo_alocada->R);
  ph_libertar_m(height, width, photo_alocada->G);
  ph_libertar_m(height, width, photo_alocada->B);

  free(photo_alocada);
} 

p_ph_par_photo ph_alocar_par_photo (p_ph_photo photo1, p_ph_photo photo2)
{
  p_ph_par_photo par_photo_alocada = (p_ph_par_photo)malloc(sizeof(struct ph_par_photo));

  par_photo_alocada->photo1 = photo1;
  par_photo_alocada->photo2 = photo2;

  return par_photo_alocada;
}

void ph_libertar_par_photo (p_ph_par_photo par)
{
  ph_libertar_photo (par->photo1);
  ph_libertar_photo (par->photo2);

  free(par);
}

unsigned char **ph_componente_photo (p_ph_photo photo, char nome_comp)
{
  unsigned char **componente = NULL;
  switch(nome_comp)
    {
    case 'R':
      componente = photo->R;
      break;
    case 'G':
      componente = photo->G;
      break;
    case 'B':
      componente = photo->B;
      break;
    }
  return componente;
}


int ph_height_photo (p_ph_photo photo)
{
  return photo->height;
}

int ph_width_photo (p_ph_photo photo)
{
  return photo->width;
}

void ph_img_ascii (char *filename, unsigned char **img, int height, int width)
{
  FILE *cR = fopen(filename, "w"); /* temp */
  int i,j;

  if (!cR)
    {
      printf("ERRO: Na escrita no ficheiro %s\n!", filename);
    }

  for(i=0; i < height; i++)
    {
      for(j = 0; j < width; j++)
	fprintf(cR,"%d ", img[i][j]);
      fprintf(cR,"\n");
    }
  fclose(cR);
}
