/*
 * Interface da ABMATCHING
 * Nome: abmatching.h
 * Autor: vasco conde
 */

#ifndef _ABMATCHING
#define _ABMATCHING

double abm_media (unsigned char **im, int height, int width);

double abm_desvio_padrao (unsigned char **im, int height, int width);

double abm_covariancia (unsigned char **im1, unsigned char **im2, 
			int height, int width);

double abm_coef_correlacao (unsigned char **im1, unsigned char **im2, 
			    int height, int width);

unsigned char **abm_alocar_sub_matrix (unsigned char **m, int height, 
				       int width, int hi, int hf, int wi, 
				       int wf);

void abm_libertar_sub_matrix (unsigned char **sub);

void abm_cross_correlation (unsigned char **im, int him, int wim, 
			    unsigned char **t, int ht, int wt);

int abm_cross_correlation_epi (unsigned char **im, int him, int wim, 
			       unsigned char **t, int ht, int wt, 
			       double *FM, int *fl, 
			       int ty, int x_min, int x_max,
			       double cc_min, 
			       int coo_final[], double *cc_final);

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
