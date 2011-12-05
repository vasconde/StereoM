/*
 * Interface da MEASTEREO
 * Nome: meastereo.h
 * Autor: vasco conde
 */

#ifndef _MEASTEREO
#define _MEASTEREO

typedef struct ms_param_photo *p_ms_param_photo;

p_ms_param_photo ms_alocar_param_photo (double x, double y, double f, 
					double X, double Y, double Z,
					double omega, double phi, double kappa);

void ms_free_param_photo (p_ms_param_photo param);

void ms_M_rot (double *R, double omega, double phi, double kappa);

double ms_ND (int op, p_ms_param_photo ph, double* meas);

double ms_kxy (int op, p_ms_param_photo ph, double* meas);

void ms_firstAprox (p_ms_param_photo ph1, double* meas1, 
		    p_ms_param_photo ph2, double* meas2, double* res);

void ms_photo2terrain (p_ms_param_photo ph1, double* meas1, 
		       p_ms_param_photo ph2, double* meas2, double* res);

#endif
