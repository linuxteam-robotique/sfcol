/*
** maths.h for  in /goinfre/wolfer_c/momow/gasser_a-inim/src
**
** Made by charles wolfers
** Login   <wolfer_c@epita.fr>
**
** Started on  Wed Jul 12 14:41:01 2006 charles wolfers
** Last update Wed Jul 12 14:56:16 2006 charles wolfers
*/

#ifndef MATHS_H_
# define MATHS_H_

# include <math.h>
# include <stdio.h>

# define SQR(V)	(V * V)

/* Variables double precision (8 octets) pour les fonctions mathematiques */

/*  1.0 / sqrt( 3.0)   */
static const float sqrt31 = 0.577350269189625764509148780501957;

/*  2.0 / sqrt( 3.0 )  */
static const float sqrt32 = 1.15470053837925152901829756100391;

/*  sqrt( 3.0 ) / 2.0  */
static const float sqrt3s2 = 0.866025403784438646763723170752936;

/*  PI / 180.0         */
static const float inrd = M_PI / 180.0;


/* long		arcsinw(long v, long mul_dec); */

long		phasearg(long ip, long rp, long md);

long		hypotw(long re, long im);

long		racine(long carre);

#endif /* !MATHS_H_ */
