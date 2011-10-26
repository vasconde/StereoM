/*
 * Interface da IMAGENSLIB
 * Nome: imagenslib.h
 * Autor: vasco conde
 */

#ifndef _IMAGENSLIB
#define _IMAGENSLIB

/*** Definicao do Tipo ponteiro para ***/
typedef struct foto *p_foto;

typedef struct par_foto *p_par_foto;

/*** Prototipos dos subprogramas ***/

p_foto read_jpeg_file(char *filename); /* descomprecao da imagem jpeg para raw_image */

/* ALOCAR e LIBERTAR */
unsigned char **alocar_mb (int m, int n); /* alocacao de memoria para uma matriz height x width */

unsigned char **libertar_mb (int m, int n, unsigned char **img); /* libertacao de memoria para uma matriz imagem */

p_foto alocar_foto (int height, int width); /* alocacao de mamoria para um estrutura foto */

void libertar_foto (p_foto foto_alocada);

unsigned char **componente_foto (p_foto foto, char nome_comp); /* retorna a matriz R, G, ou B da foto */

int height_foto (p_foto foto);

int width_foto (p_foto foto);

void libertar_par_foto (p_par_foto par);

p_par_foto alocar_par_foto (p_foto foto1, p_foto foto2);

/* IO */
void img_ascii (char *filename, unsigned char **img, int height, int width); /* passa a imagem para ascii  */

#endif
