/*
 * Interface da OEX
 * Nome: oex.h
 * Autor: vasco conde
 * 
 * Des: Conjunto de metodos para a determinacao dos parametros
 * de orientacao externa de uma imagem utilizando um ajustamento por minimos
 * quadrados e o modelo das equacoes de colinearidade
 *
 */

#ifndef _OEX
#define _OEX

/*Ponteiro para estrutura para albergar 
 *os parametros de o. interna*/
typedef struct oex_oin_param *p_oex_oin_param;

/*Ponteiro para estrutura para albergar 
 *os parametros de o. externa*/
typedef struct oex_param *p_oex_param;

/* Ponteiro para estrutura para albergar as listas
 * de coordenadas dos pfs 
 * no espaco terreno e imagem */
typedef struct oex_pfs *p_oex_pfs;


/** IO **/

/*mostrar matriz na consola*/
void oex_showm (double *M, int m, int n);

/*carregar matriz a partir de um ficheiro*/
void oex_readm (char *nfile, double *M, int m, int n);

/*carrega as coordenadas de uma lista ascii para uma matriz*/
void oex_carrega_coo(double *coo, int dim, char *nfile, char tipo);

/** FIM IO  **/

/** GESTAO DE MEMORIA 
 * o proprio nome eh suficiente para descrever a sua funcao
 **/
p_oex_oin_param oex_alocar_oin_param ();

p_oex_param oex_alocar_param ();

p_oex_pfs oex_alocar_pfs ();

void oex_libertar_oin_param (p_oex_oin_param e);

void oex_libertar_param (p_oex_param e);

void oex_libertar_pfs (p_oex_pfs e);

/*FIM GESTAO DA MEMORIA*/

/* 
 * Calcula a matriz de rotacao 
 */
void oex_M_rot (double *R, double omega, double phi, double kappa, int op);

/*
 * Adiciona os parametros de orientacao interna
 */
void oex_add_oin_param (p_oex_oin_param oin_param, double c, double xo, double yo);

/*
 * Adiciona os parametros de orientacao externa
 */
void oex_add_param (p_oex_param param, double Xo, double Yo, double Zo, double omega, double phi, double kappa, int opR);

/*
 * Adiciona pfs
 */
void oex_add_pfs (p_oex_pfs pfs, int n_pfs, double *terreno, double *foto);

/*
 * Modifica os parametros de orientacao externa
 */
void oex_set_param (p_oex_param param, double Xo, double Yo, double Zo, double omega, double phi, double kappa, int opR);

/*
 * recolhe os parametros de orientacao externa
 */
void oex_get_param (p_oex_param param, double* Xo, double* Yo, double* Zo, double* omega, double* phi, double* kappa);

/*
 * retorna as componetes Nx, Ny e D
 * das equacoes de colinearidade
 */
double oex_n_xy_d (int op, int i, p_oex_pfs pfs, p_oex_param param);

/*
 * Matriz A
 */
void oex_designM (double *A, p_oex_oin_param oin_param, p_oex_pfs pfs, p_oex_param param);

/*
 * Matriz de fecho
 */
void oex_Mfecho (p_oex_oin_param oin_param, p_oex_pfs pfs, p_oex_param param, double *W);

/*
 * Actualiza a matriz dos parametros X
 * com os valores da estrutura de dados param
 */
void oex_param2X (p_oex_param param, double *X);

/*
 * Actualiza os valores da estrutura de dados param
 * com os valor da matriz de parametros X
 */
void oex_X2param (double *X, p_oex_param param);

/*
 * Calcula a norma de uma matriz M
 */
double oex_norm_M (double *M, int m, int n);

/*
 * Transforma as coordenadas imagem
 * de: origen canto superior esquerdo e eixos (X >;Y v) 
 * para: origem centro da imagem e eixos (X >;Y ^) 
 */
void oex_transfotopfs (p_oex_oin_param oin_param, p_oex_pfs pfs);

/*escreve num ficheiro de texto a matriz M*/
void oex_M_ascii (char *filename, double *M, int m, int n);

/*
 * Calcula os parametros de orientacao externa recorrendo a um ajustamento por 
 * minimos quadrados.
 * Parametros: numero de pfs, parametros de orientacao interna, 
 * parametros de orientacao externa, pfs
 * retorno: parametros de orientacao externa
 */
void oex_compute (int n, p_oex_oin_param oin_param, p_oex_param param, p_oex_pfs pfs);

#endif
