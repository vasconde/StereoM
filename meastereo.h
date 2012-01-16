/*
 * Interface da MEASTEREO
 * Nome: meastereo.h
 * Autor: vasco conde
 *
 * Des: Modulo dedicado ah determinacao de 
 * coordenadas objecto a partir de coordenadas
 * imagem de pontos homologos em pares de imagens
 * em condicoes de estereoscopia a partir das equacoes
 * de colinearida.
 */

#ifndef _MEASTEREO
#define _MEASTEREO

/* ponteiro para
 * estrutura de dados para parametros 
 * de orientacao interna e externa
 */
typedef struct ms_param_photo *p_ms_param_photo;

/*alocacao de memoria para 
 *a estrutura dos parametros
 */
p_ms_param_photo ms_alocar_param_photo (double x, double y, double f, 
					double X, double Y, double Z, int op,
					double omega, double phi, double kappa);

/*libertacao de memoria para 
 *a estrutura dos parametros
 */
void ms_free_param_photo (p_ms_param_photo param);

/* Calcula a matriz de rotacao */
void ms_M_rot (int op, double *R, double omega, double phi, double kappa);

/* Determina o valor do numerador (Nx ou Ny)
 * ou denominador (D) das equacoes de 
 * colinearida
 */
double ms_ND (int op, p_ms_param_photo ph, double* meas);

/* Determina valor de Nx/D ou Ny/D
 * das equacoes de colinearidade
 */
double ms_kxy (int op, p_ms_param_photo ph, double* meas);

/* Determina uma primeira aproximacao para as coordenadas
 * objecto, para utilizacao no ajustamento
 * entrada: parametros e coordenadas imagem
 * saida: 1a aproximacao as coordenadas terreno
 */
void ms_firstAprox (p_ms_param_photo ph1, double* meas1, 
		    p_ms_param_photo ph2, double* meas2, double* res);

/* Determinacao das coordenadas objecto de um ponto medido
 * em duas imagens recorrendo a um ajustamento por minimos
 * quadrados das equacoes de colinearidade
 * Entrada: parametros de orientacao da imagem 1 e coordenadas
 * imagem do ponto na imagem 1;
 * parametros de orientacao da imagem 2 e coordenadas
 * imagem do ponto na imagem 2;
 * ponteiro para receber a proposta
 */
void ms_photo2terrain (p_ms_param_photo ph1, double* meas1, 
		       p_ms_param_photo ph2, double* meas2, double* res);

#endif
