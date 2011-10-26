/*
 * Implementacao da IMAGENSLIB
 * Nome: imagenslib.c
 * Autor: vasco conde
 */

/*
 * BIBLIOTECAS AUXILIARES
 */
#include <stdio.h>   /* c padrao */
#include <stdlib.h>  /* c padrao */

#include <jpeglib.h> /* operacoes sobre o formato jpeg  */

#include "imagenslib.h"

struct foto
{
  int height;
  int width;

  unsigned char **R;
  unsigned char **G;
  unsigned char **B;

};

struct par_foto
{
  p_foto foto1;
  p_foto foto2;
};

p_foto read_jpeg_file(char *filename)
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

  p_foto foto1;

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

  foto1 = alocar_foto(height,width);
  
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
	    foto1->R[cinfo.output_scanline-1][j] = raw_image[location-1];
	  else if (c == 1)
	    foto1->G[cinfo.output_scanline-1][j] = raw_image[location-1];
	  else
	    foto1->B[cinfo.output_scanline-1][j++] = raw_image[location-1];
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
  return foto1;
}

/* alocacao de de memoria para R G B e retorno de um elemento pFoto*/
/*p_foto alocar_foto (int height)*/

/* m * n */
unsigned char **alocar_mb (int m, int n)
{
  unsigned char **v;  /* ponteiro para a matriz */
  int i;    /* variavel auxiliar      */

  if (m < 1 || n < 1) /* verifica parametros recebidos */
    { 
      printf ("** Erro: Parametro invalido **\n");
      return (NULL);
    }
  /* aloca as linhas da matriz */
  v = (unsigned char **) calloc (m, sizeof(unsigned char *));
  if (v == NULL) 
    {
      printf ("** Erro: Memoria Insuficiente **");
      return (NULL);
    }
  /* aloca as colunas da matriz */
  for ( i = 0; i < m; i++ ) 
    {
      v[i] = (unsigned char*) calloc (n, sizeof(unsigned char));
      if (v[i] == NULL) 
	{
	  printf ("** Erro: Memoria Insuficiente **");
	  return (NULL);
	}
    }
  return (v); /* retorna o ponteiro para a matriz */
}

unsigned char **libertar_mb (int m, int n, unsigned char **img)
{
  int  i;  /* variavel auxiliar */
  if (img == NULL) 
    return (NULL);
  if (m < 1 || n < 1) {  /* verifica parametros recebidos */
    printf ("** Erro: Parametro invalido **\n");
    return (img);
  }
  for (i=0; i<m; i++) 
    free (img[i]); /* libera as linhas da matriz */
  free (img);      /* libera a matriz */
  return (NULL); /* retorna um ponteiro nulo */
}

p_foto alocar_foto (int height, int width)
{
  p_foto foto_alocada = (p_foto)malloc(sizeof(struct foto));

  unsigned char **R = alocar_mb(height,width);
  unsigned char **G = alocar_mb(height,width);
  unsigned char **B = alocar_mb(height,width);

  foto_alocada->R = R;
  foto_alocada->G = G;
  foto_alocada->B = B;

  foto_alocada->height = height;
  foto_alocada->width = width;

  return foto_alocada;  
}

void libertar_foto (p_foto foto_alocada)
{
  int height = foto_alocada->height;
  int width = foto_alocada->width;

  libertar_mb(height, width, foto_alocada->R);
  libertar_mb(height, width, foto_alocada->G);
  libertar_mb(height, width, foto_alocada->B);

  free(foto_alocada);
} 

p_par_foto alocar_par_foto (p_foto foto1, p_foto foto2)
{
  p_par_foto par_foto_alocada = (p_par_foto)malloc(sizeof(struct par_foto));

  par_foto_alocada->foto1 = foto1;
  par_foto_alocada->foto2 = foto2;

  return par_foto_alocada;
}

void libertar_par_foto (p_par_foto par)
{
  libertar_foto (par->foto1);
  libertar_foto (par->foto2);

  free(par);
}

unsigned char **componente_foto (p_foto foto, char nome_comp)
{
  unsigned char **componente = NULL;
  switch(nome_comp)
    {
    case 'R':
      componente = foto->R;
      break;
    case 'G':
      componente = foto->G;
      break;
    case 'B':
      componente = foto->B;
      break;
    }
  return componente;
}


int height_foto (p_foto foto)
{
  return foto->height;
}

int width_foto (p_foto foto)
{
  return foto->width;
}

void img_ascii (char *filename, unsigned char **img, int height, int width)
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
