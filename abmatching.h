/*
 * Interface da ABMATCHING
 * Nome: abmatching.h
 * Autor: vasco conde
 *
 * Des: Modulo dedicado ah correspondecia automatica
 * de imagens, utilizando o algoritmo de correspondecia
 * automatica de imagens baseado na area 
 * e com restricao definida pela recta epipolar
 */

#ifndef _ABMATCHING
#define _ABMATCHING

/* Calculo da media dos valor de intensidade de uma imagem 
 * ou sub imagem
 * entrada: matriz de valores de intensidade, altura e comprimento
 * saida: media
 */
double abm_media (unsigned char **im, int height, int width);

/* Calculo do desvio padrao dos valor de intensidade de uma imagem 
 * ou sub imagem
 * entrada: matriz de valores de intensidade, altura e comprimento
 * saida: desvio padrao
 */
double abm_desvio_padrao (unsigned char **im, int height, int width);

/* Calculo da covariancia entre os valor de intensidade de duas imagem 
 * ou sub imagens
 * entrada: matriz de valores de intensidade im1,
 * matriz de valores de intensidade im2,
 * altura e comprimento
 * saida: covariancia
 */
double abm_covariancia (unsigned char **im1, unsigned char **im2, 
			int height, int width);

/* Calculo do coeficiente de correlacao entre 
 * os valor de intensidade de duas imagem 
 * ou sub imagens
 * entrada: matriz de valores de intensidade im1,
 * matriz de valores de intensidade im2,
 * altura e comprimento
 * saida: coeficiente de correlacao
 */
double abm_coef_correlacao (unsigned char **im1, unsigned char **im2, 
			    int height, int width);

/* Retorna uma sub imagem de uma imagem maior
 * entrada: matriz de valores de intensidade,
 * altura e comprimento dessa matriz,
 * localizacao do inicio e fim da sub matriz
 * saida: sub matriz
 */
unsigned char **abm_alocar_sub_matrix (unsigned char **m, int height, 
				       int width, int hi, int hf, int wi, 
				       int wf);

/*liberta memoria alocada para a submatriz*/
void abm_libertar_sub_matrix (unsigned char **sub);

/* Procura numa imagem (im) o template (t) 
 * atraves de toda a imagem (im)
 * Entrada: imagem, template e
 * respectivas dimensoes
 * Saida: apresentacao na consola dos 
 * pontos e respctivo cc se for superior a 0.99
 */
void abm_cross_correlation (unsigned char **im, int him, int wim, 
			    unsigned char **t, int ht, int wt);

/*
 * DES:   Image matching restrigido ah epipolar
 * PARAM: imagem, h da imagem, w da imagem,
 *        template, h do template, w do template,
 *        matriz fundamenta, coo do ponto a procurar,
 *        (im esquerda), margem em y (epi), limite
 *        minimo e maximo para a janela de procura,
 *        coeficiente de correlacao minimo,
 *        array para guardar coordenadas resultado,
 *        e ponteiro para cc respectivo
 * SAIDA: Retorna 1 se detectou alguma correspondecia
 *        0 em caso contrario, o resultado fica
 *        guardado nas variaveis com esse destino
 */
int abm_cross_correlation_epi (unsigned char **im, int him, int wim, 
			       unsigned char **t, int ht, int wt, 
			       double *FM, int *fl, 
			       int ty, int x_min, int x_max,
			       double cc_min, 
			       int coo_final[], double *cc_final);

/*
 * DES:   Image matching restrigido ah epipolar que procura
 *        todos os pontos homologos para uma area dada
 * PARAM: imagem esquerda e respectivas dimensoes
 *        imagem direita e respectivas dimensoes
 *        dimensao do template, matriz fundamenta,
 *        margem para y (epi), limites para a janela
 *        para a qual se vao procurar os pontos homologos
 *        (esquerda), limites em x para a janela de procura
 *        (direita), coeficiente de correlacao minimo,
 *        ponteiro para numero de pontos homologos detectados,
 *        ponteiro para resultado.
 * SAIDA: retorna 1 se detectar algum ponto homologo e o
 *        resultado fica armazenado nas variaveis para isso
 *        destinadas
 *
 *        resultado:
 *        |i_l |j_l | i_r | j_r | cc |
 */
int abm_cross_correlation_epi_area (unsigned char **iml, int himl, int wiml,
				    unsigned char **imr, int himr, int wimr,
				    int dim_template,
				    double *FM, int ty, 
				    int x_min_l, int x_max_l,
				    int y_min_l, int y_max_l,
				    int x_min_r, int x_max_r,
				    double cc_min, 
				    int *n, double *resultado);

#endif
