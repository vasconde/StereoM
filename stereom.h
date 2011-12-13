/*
 * Interface da STEREOM
 * Nome: stereom.h
 * Autor: vasco conde
 *
 */

#ifndef _STEREOM
#define _STEREOM

void st_stereo_matching (p_ph_photo foto_l, p_ph_photo foto_r, int sub_l, int sub_r,
			 double *epil);

#endif
