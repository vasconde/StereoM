/*
 * Interface da MEASTEREO
 * Nome: meastereo.h
 * Autor: vasco conde
 */

#ifndef _MEASTEREO
#define _MEASTEREO

typedef struct ms_param_photo *p_ms_param_photo;

double *photo2terrain (p_ms_param_photo ph1, double* meas1, 
		       p_ms_param_photo ph2, double* meas2);

#endif
