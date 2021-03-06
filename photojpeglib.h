/*
 * Interface da PHOTOJPEGLIB
 * Des: Modulo para operar sobre imagens no formato JPEG.
 * Nome: photojpeglib.h
 * Autor: vasco conde
 */

#ifndef _PHOTOJPEGLIB
#define _PHOTOJPEGLIB

/*** Definicao do Tipo ponteiro para ***/
typedef struct ph_photo *p_ph_photo;

typedef struct ph_par_photo *p_ph_par_photo;

/*** Prototipos dos subprogramas ***/

/* 
 * descomprecao da imagem para uma estrutura ph_photo 
 * Parametros: caminho para ficheiro imagem JPEG
 * Retorno: Estrutura ph_photo preenchida com as tres componetes
 * RGB
 */
p_ph_photo ph_read_jpeg_file(char *filename); 

/* ALOCAR e LIBERTAR */
unsigned char **ph_alocar_m (int h, int w); /* alocacao de memoria para uma matriz height x width */

unsigned char **ph_libertar_m (int h, int w, unsigned char **img); /* libertacao de memoria para uma matriz imagem */

p_ph_photo ph_alocar_photo (int height, int width); /* alocacao de mamoria para um estrutura photo */

void ph_libertar_photo (p_ph_photo photo_alocada);

unsigned char **ph_componente_photo (p_ph_photo photo, char nome_comp); /* retorna a matriz R, G, ou B da photo */

/*retorna height*/
int ph_height_photo (p_ph_photo photo);

/*retorna width*/
int ph_width_photo (p_ph_photo photo);

/*gestao de memoria*/
void ph_libertar_par_photo (p_ph_par_photo par);

p_ph_par_photo ph_alocar_par_photo (p_ph_photo photo1, p_ph_photo photo2);

/* IO */
void ph_img_ascii (char *filename, unsigned char **img, int height, int width); /* passa a imagem para ascii  */

#endif
