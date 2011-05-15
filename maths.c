/*
** maths.c for  in /goinfre/wolfer_c/momow/gasser_a-inim/src
**
** Made by charles wolfers
** Login   <wolfer_c@epita.fr>
**
** Started on  Tue Jul 11 23:51:58 2006 charles wolfers
** Last update Wed Jul 12 00:35:47 2006 charles wolfers
*/

#include <stdio.h>
#include <stdlib.h>
#include "maths.h"
#include "tab_hypot.h"
#include "tab_phasearg.h"
#include "tab_racine.h"
//#include "tab_arcsin.h"

/* long		arcsinw(long v, long mul_dec) */
/* { */
/*   long		v1 = 0; */
/*   long		abs_v = abs(v); */
/*   long		mul = (1 << mul_dec); */
/*   long		dec; */

/*   if (abs_v > mul) */
/*     fprintf(stderr, "ASIN\t%f\n", (float) v / mul); */
/*   else */
/*     { */
/*       if (abs_v == mul) */
/* 	v1 = (v > 0) ? 90 : -90; */
/*       else */
/*       { */
/* 	  //v1 = atanf(v / sqrt(1.0 - SQR(v))) / inrd; */
/* 	  dec = SMPL_ARCSIN_P2 - mul_dec; */
/* //	  printf("abs_v = %li, dec = %li\n", abs_v, dec); */
/* 	  if (dec < 0) // SMPL_ARCSIN < mul_dec */
/* 	      v1 = arcsin_tab[abs_v >> -dec]; */
/* 	  else // SMPL_ARCSIN > mul_dec */
/* 	      v1 = arcsin_tab[abs_v << dec]; */
/* 	  if (v < 0) */
/* 	      v1 = -v1; */
/*       } */
/*     } */
/*   return v1; */
/* } */

/* long		phasearg(long ip, long rp, long md) */
/* { */
/*   long		r; */

/*   if (md == 0) */
/*     return 0; */
/*   else */
/*     { */
/*       md = (ip << 16) / md; */
/*       r = (rp < 0) ? 180 - arcsinw(md, 16) : arcsinw(md, 16); */
/*       if (r > 180) */
/* 	r = r - 360; */
/*       else */
/* 	if (r < -180) */
/* 	  r = r + 360; */
/*     } */
/*   return r; */
/* } */

/* l'argument est ramene dans [0;360[ */
long		phasearg(long ip, long rp, long md)
{
    if (md == 0)
	return 0;
    if (abs(ip) > 255)
    {
	ip = (ip > 0) ? 255 : -255;
	fprintf(stderr, "TRUNC IM !!\n");
	fflush(stderr);
    }
    if (abs(rp) > 255)
    { 
	rp = (rp > 0) ? 255 : -255;
	fprintf(stderr, "TRUNC RE !!\n");
	fflush(stderr);
    }
    if (ip > 0)
	if (rp > 0)
	    return phasearg_tab[ip][rp];
	else
	    return 180 - phasearg_tab[ip][-rp];
    else
	if (rp > 0)
	    return 360 - phasearg_tab[-ip][rp];
	else
	    return 180 + phasearg_tab[-ip][-rp];
}

long		hypotw(long re, long im)
{
    long	abs_re = abs(re);
    long	abs_im = abs(im);

    if (abs_re > 255)
    {
	abs_re = 255;
	fprintf(stderr, "TRUNC RE !!\n");
	fflush(stderr);
    }
    if (abs_im > 255)
    { 
	abs_im = 255;
	fprintf(stderr, "TRUNC IM !!\n");
	fflush(stderr);
    }
    return hypot_tab[abs_re][abs_im];
}

long		racine(long carre)
{
    if (carre < 0)
    {
	fprintf(stderr, "sqrt(-x) !!\n");
	fflush(stderr);
	return 0;
    }
    if (carre >= SMPL_RACINE)
    { 
	fprintf(stderr, "TRUNC sqrt(%li) !!\n", carre);
	fflush(stderr);
	return racine_tab[SMPL_RACINE - 1];
    }
    return racine_tab[carre];
}
