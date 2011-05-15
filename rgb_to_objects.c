/*
** Rgb_to_objects.c for rgb_to_objects in /home/aurelien/momow/gasser_a-inim
**
** Made by aurelien
** Login   <gasser_a@epita.fr>
**
** Started on  Fri Jul 14 02:44:32 2006 aurelien
** Last update Fri Jul 14 23:33:18 2006 charles wolfers
*/

/* #include <SDL/SDL.h> */
/* #include <SDL/SDL_thread.h> */
/* #include <SDL/SDL_timer.h> */

#include <stdio.h>
#include <stdlib.h>
#include "rgb_to_objects.h"
#include "defines.h"
#include "objects.h"
#include "maths.h"

#define SIZE_OBJ_MIN	500
#define SIZE_OBJ_MAX	8000
/* #define SKIP_PIX_LENGTH	8//5	/\* Nombre de ligne/col par pixel de recherche d'objet *\/ */
#define SKIP_PIX_LENGTH	0	/* Nombre de ligne/col a sauter pendant la recherche d'objet */


#define in_image(src, pix) (pix >= src->img_beg && pix < src->img_end)
#define pix_to_x(src, pix, imgw) ((pix - src->img_beg) % imgw)
#define pix_to_y(src, pix, imgw) ((pix - src->img_beg) / imgw)

//static struct s_object	*objects[MAX_OBJECTS];

#define SAT_MIN_RED	25
#define SAT_MIN_GREEN	30
#define SAT_MIN_BLUE	60

#define SAT_MIN_0	SAT_MIN_RED//25
#define SAT_MIN_30	((SAT_MIN_0 + SAT_MIN_60) >> 1)//26
#define SAT_MIN_60	((SAT_MIN_RED + SAT_MIN_GREEN) >> 1)//27
#define SAT_MIN_120	SAT_MIN_GREEN//30
#define SAT_MIN_180	((SAT_MIN_GREEN + SAT_MIN_BLUE) >> 1)//45
#define SAT_MIN_240	SAT_MIN_BLUE//60
#define SAT_MIN_300	((SAT_MIN_BLUE + SAT_MIN_RED) >> 1)//42

#define SAT_MIN		SAT_MIN_RED

inline enum e_color	rgbToColor(struct s_colr_rgb	*src_rgb)
{
  long			re, im;
  long			r, g, b;
  long			h, l, s;
  long			md;

  COLOR2RGB(src_rgb, r, g, b);
  re = r - ((g + b) >> 1);
  im = (g - b) * sqrt3s2;
  /* im = ((g - b) * 14529495) >> 24; //APPROX SANS FLOTTANT :S */
  md = hypotw(re, im);
  h = phasearg(im, re, md); /* Hue calcul: Hue/360 */
  l = r + g + b; /* Luminosity calcul: Lum/765  (255*3 = 765) */

  //s = racine(SQR(r) + SQR(g) + SQR(b) - SQR(l));
  s = racine(SQR(r) + SQR(g) + SQR(b) - SQR(l) / 3);
  //max(Sat) ~= 208.21 max trouve par l'essai: 185


  if ((h >= 30) && (h < 90) && (l >= 200) /*&& (s >= 10)*/)
    return yellow; //60 deg
  else
    if (((h < 30) || (h >= 270)) && (s >= 15))
      return red; //0 deg
  else
    if ((h >= 90) && (h < 200) && (s >= 10))
      return green; //120 deg
  else
    if ((h >= 200) && (h <= 270) && (s >= 15))
      return blue; //240 deg
  else
    if ((h >= 20) && (h < 40) && (l < 383) && (s >= 30))
      return brown; //30 deg
  else
    if ((h >= 20) && (h < 45) && (l >= 383) /*&& (l < 574)*/ && (s >= 30))
      return orange; //30 deg (+ lumineux que marron)
  else
    if ((h >= 150) && (h < 210) /*&& (l >= 64) && (l < 574)*/ && (s >= 45))
      return cyan; //180 deg
  else
    if ((h >= 210) && (h <= 270) && (l >= 383) /*&& (l < 574)*/ && (s >= 60))
      return sky_blue; //240 deg
  else
    if ((h >= 270) && (h < 330) /*&& (l >= 64)*/ && (l < 574) && (s >= 42))
      return magenta; //300 deg
  else
    if ((h >= 270) && (h < 330) && (l >= 574) /*&& (l < 701)*/ && (s >= 42))
      return pink; //300 deg (+ lumineux que magenta)
  else
    {
      if (s < 30)
	{
	  if (l >= 665)
	    return white;
	  if (l <= 100)
	    return black;
	}
      return unknown;
    }
}

/*!
** Calculate the center of the object
** with the computed data
**
** @param object the object
*/
inline void			compute_center(struct s_object *object)
{
  unsigned long			j = 0;
  register unsigned long	dx = 0, dy = 0;
  register unsigned long	x, y, maxx, maxy, minx, miny;

  minx = miny = 0xFFFFFFFF;
  maxx = maxy = 0;
  for (j = 0; j < object->cpt; ++j)
    {
      x = object->coord[j][0];
      y = object->coord[j][1];
      if (x > maxx)
	maxx = x;
      if (x < minx)
	minx = x;
      if (y > maxy)
	maxy = y;
      if (y < miny)
	miny = y;
      dx += x;
      dy += y;
    }
  object->x = dx / object->cpt;
  object->y = dy / object->cpt;
  object->minx = minx;
  object->miny = miny;
  object->maxx = maxx;
  object->maxy = maxy;
}

/*!
** Test the object with all the sorts
*/
inline int		is_valid(struct s_object	*object,
				 struct s_dim		*dim)
{
    unsigned long	dx;
    unsigned long	dy;
    unsigned long	r;

    dx = object->maxx - object->minx;
    dy = object->maxy - object->miny;

    /* Check too big size */
    if ((dx >= (dim->width >> 1)) || (dy >= (dim->height >> 1)))
      return 0;

    switch (object->color)
      {
      case white:
	return 1;
	break;
      case black:
	return 1;
        break;
      case red:
	return 1;
	break;
      }

    return 0;
}

#define NotePixel(pimg, xc, yc)					\
{								\
    *pimg = unused;						\
    object->coord[object->cpt][0] = xc;				\
    object->coord[object->cpt][1] = yc;				\
    object->cpt++;						\
}

#define NotePixel1(xc, yc)					\
{								\
    object->coord[object->cpt][0] = xc;				\
    object->coord[object->cpt][1] = yc;				\
    object->cpt++;						\
}

#define NotePixel2(xc, yc)					\
{								\
    *coord = xc;						\
    *(coord + 1) = yc;						\
    cpt++;							\
    coord += 2;							\
}

/* #define NotePixel2(xc, yc)					\ */
/* {								\ */
/*     *coord = xc;						\ */
/*     *(coord + 1) = yc;						\ */
/*     cpt++;							\ */
/*     coord += 2;							\ */
/* *(picture + (yc * dim.width + xc) * 3) = 255;		\ */
/* *(picture + (yc * dim.width + xc) * 3 + 1) = 255;	\ */
/* *(picture + (yc * dim.width + xc) * 3 + 2) = 255;	\ */
/* } */

/*!
** Find an object from a pixel and fill the object structure
**
** @param src the source image
** @param pix on of the pixels of the objects
** @param object the object to fill
**
** If the pixel is of the color of the object, add it to the object
** Test if the pixels around the current pixel are of the same color.
** If yes, recursion
*/
static unsigned long	find_object(struct s_ptr_uc	*src,
				    UC			*pix,
				    struct s_object	*object,
				    unsigned long	imgw)
{
  UC			*x_img, *pimg, *y_img;
  unsigned long		x, y, xc, yc, x1_prev, x2_prev, x_ret, old_cpt;
  enum e_color		color = *pix;

  x = pix_to_x(src, pix, imgw);
  y = pix_to_y(src, pix, imgw);

  object->coord[object->cpt][0] = x;
  object->coord[object->cpt][1] = y;
  object->cpt++;

  /* Parcours de la premiere ligne */
  x_img = pix + imgw - x; /* Calcul le x juste apres la fin de la ligne */
  for (pimg = pix + 1, xc = x + 1; pimg < x_img; pimg++, xc++)
      if (*pimg == color)
	  NotePixel(pimg, xc, y)
      else
	  break;
  x_ret = xc - x;
  if (object->cpt >= (NBR_PIX_MAX - imgw))
  {
      fprintf(stderr, "OBJECT RESEARCH STOPPED!!!\n");
      fflush(stderr);
      return x_ret;
  }

  yc = y;
  y_img = pix;
  x1_prev = x;
  x2_prev = xc - 1;
  /* Pour chaque ligne en dessous de la premiere faire ... */
  do
  {
      old_cpt = object->cpt;
      yc++;
      y_img += imgw;

      /* Parcours vers la gauche */
      x_img = y_img - x; /* Calcul le x au debut de la ligne yimg */
      for (pimg = y_img, xc = x; pimg >= x_img; pimg--, xc--)
	  if (*pimg == color)
	      NotePixel(pimg, xc, yc)
	  else
	      /* On cherche la couleur 1 pixel plus loin que sur la ligne d'au-dessus */
	      if (xc < x1_prev)
		  break;
      x1_prev = xc + 1;

      /* Parcours vers la droite */
      x_img = y_img + imgw - x; /* Calcul le x juste apres la fin de la ligne yimg */
      for (pimg = y_img + 1, xc = x + 1; pimg < x_img; pimg++, xc++)
	  if (*pimg == color)
	      NotePixel(pimg, xc, yc)
	  else
	      /* On cherche la couleur 1 pixel plus loin que sur la ligne d'au-dessus */
	      if (xc > x2_prev)
		  break;
      x2_prev = xc - 1;

      if (object->cpt >= (NBR_PIX_MAX - imgw))
      {
	  fprintf(stderr, "OBJECT RESEARCH STOPPED!!!\n");
	  fflush(stderr);
	  return x_ret;
      }
  } while (old_cpt < object->cpt);

  return x_ret;
}


inline unsigned long	find_object2(struct s_ptr_uc	*src, /* color image */
				     UC			*pix, /* pixel color */
				     struct s_ptr_rgb	*rgb, /* RGB image */
				     struct s_colr_rgb	*crgb, /* current RGB pixel */
				     struct s_object	*object,
				     struct s_dim	dim)
{
  struct s_colr_rgb	*x_img, *pimg, *y_img;
  UC			*cpimg, *y_cimg;
  unsigned long		x, y, xc, yc, x1_prev, x2_prev, x_ret, cpt = 0, old_cpt;
  unsigned long		*coord = (unsigned long *) object->coord;
  enum e_color		color = *pix, c;

  x = pix_to_x(src, pix, dim.width);
  y = pix_to_y(src, pix, dim.width);

  NotePixel2(x, y)

  /* Parcours de la premiere ligne */
  x_img = crgb + dim.width - x; /* Calcul le x juste apres la fin de la ligne */
  for (pimg = crgb + 1, cpimg = pix + 1, xc = x + 1; pimg < x_img; pimg++, cpimg++, xc++)
  {
      if (*cpimg != unknown)
	  break;
      c = rgbToColor(pimg);
      if (c == color)
      {
	  *cpimg = c;
	  NotePixel2(xc, y)
	  if (cpt >= NBR_PIX_MAX)
	    break;
      }
      else
	  break;
  }
  x_ret = xc - x;
  if (object->cpt >= NBR_PIX_MAX)
  {
      fprintf(stderr, "OBJECT RESEARCH STOPPED!!!\n");
      fflush(stderr);
      object->cpt = cpt;
      return x_ret;
  }

  yc = y;
  y_img = crgb;
  y_cimg = pix;
  x1_prev = x;
  x2_prev = xc - 1;
  /* Pour chaque ligne en dessous de la premiere faire ... */
  do
  {
/*       old_cpt = cpt; */
      yc++;
      y_img += dim.width;
      y_cimg += dim.width;

      /* Parcours vers la gauche */
      x_img = y_img - x; /* Calcul le x au debut de la ligne yimg */
      for (pimg = y_img, cpimg = y_cimg, xc = x, old_cpt = cpt; 
	   pimg >= x_img; 
	   pimg--, cpimg--, xc--)
      {
	  if (*cpimg != unknown)
	      break;
	  c = rgbToColor(pimg);
	  if (c == color)
	  {
	      *cpimg = c;
	      NotePixel2(xc, yc)
	      if (cpt >= NBR_PIX_MAX)
		break;
	  }
	  else
	      /* On cherche la couleur aussi loin que sur la ligne d'au-dessus */
	      if (xc <= x1_prev)
		  break;
      }
      x1_prev = (pimg < x_img) ? xc + 1 : xc;
      if (cpt - old_cpt == 0)
	break;

      if (cpt >= NBR_PIX_MAX)
      {
	  fprintf(stderr, "OBJECT RESEARCH STOPPED (TOO MUCH PIXELS)!!!\n");
	  fflush(stderr);
	  object->cpt = cpt;
	  return x_ret;
      }

      /* Parcours vers la droite */
      x_img = y_img + dim.width - x; /* Calcul le x juste apres la fin de la ligne yimg */
      for (pimg = y_img + 1, cpimg = y_cimg + 1, xc = x + 1, old_cpt = cpt; 
	   pimg < x_img; 
	   pimg++, cpimg++, xc++)
      {
	  if (*cpimg != unknown)
	      break;
	  c = rgbToColor(pimg);
	  if (c == color)
	  {
	      *cpimg = c;
	      NotePixel2(xc, yc)
	      if (cpt >= NBR_PIX_MAX)
		break;
	  }
	  else
	      /* On cherche la couleur aussi loin que sur la ligne d'au-dessus */
	      if (xc >= x2_prev)
		  break;
      }
      x2_prev = (pimg >= x_img) ? xc - 1 : xc;
      if (cpt - old_cpt == 0)
	break;

      //Si le nombre de pixel colorie est moins de 2 fois moins grand que la longueur de la ligne
/*       if (((cpt - old_cpt) << 1) < (x2_prev - x1_prev)) */
/*       { */
/* //	  write(1, "H", 1); */
/* 	  object->cpt = cpt; */
/* 	  return x_ret; */
/*       } */
      //Dans la mesure du possible, il faut choper tout les pixels de l'objet pour
      //ne pas le morceler en plusieurs objets, cependant des qu'on est assez bas
      //sur l'image, rien n'empeche d'approximer sa recherche

      if (cpt >= NBR_PIX_MAX)
      {
	  fprintf(stderr, "OBJECT RESEARCH STOPPED (TOO MUCH PIXELS)!!!\n");
	  fflush(stderr);
	  object->cpt = cpt;
	  return x_ret;
      }
  } while (yc < dim.height);
  
/*   if ((object->cpt >= SIZE_OBJ_MIN) /\*&& (object->cpt <= SIZE_OBJ_MAX)*\/) */
/*       printf("%c %i\n", (old_cpt == object->cpt) ? '=' : ' ', yc); */
  object->cpt = cpt;

  return x_ret;
}

/* inline void		replace_considered(struct s_ptr_uc	*colr, */
/* 					   enum e_color		color) */
/* { */
/*   UC			*c; */

/*   for (c = colr->img_beg; c < colr->img_end; c++) */
/*     if (((enum e_color) *c) == considered) */
/*       *c = color; */
/* } */

inline int		is_color_ok(long		x,
				    long		y,
				    struct s_ptr_uc	*colr,
				    struct s_ptr_rgb	*rgb,
				    enum e_color	color,
				    struct s_dim	*dim)
{
  return ((x >= 0) &&
	  (x < dim->width) &&
	  (y >= 0) &&
	  (y < dim->height) &&
	  (colr->img[y][x] == unknown) &&
	  (rgbToColor(rgb->img[y] + x) == color));
}

/* search_around() assume *pix = unknown before to be called */
void			search_around(struct s_ptr_uc	*colr,
				      UC		*pix,
				      struct s_ptr_rgb	*rgb,
				      struct s_object	*object,
				      enum e_color	color,
				      struct s_dim	*dim)
{
  unsigned long		x, y;

  x = pix_to_x(colr, pix, dim->width);
  y = pix_to_y(colr, pix, dim->width);
  NotePixel1(x, y)
  *pix = color;

  /* Search around pix pixel by trigonometric sens */

  /* (x + 1 ; y) */
  if (is_color_ok(x + 1, y, colr, rgb, color, dim))
    search_around(colr, colr->img[y] + x + 1, rgb, object, color, dim);

  /* (x + 1 ; y - 1) */
  if (is_color_ok(x + 1, y - 1, colr, rgb, color, dim))
    search_around(colr, colr->img[y - 1] + x + 1, rgb, object, color, dim);

  /* (x ; y - 1) */
  if (is_color_ok(x, y - 1, colr, rgb, color, dim))
    search_around(colr, colr->img[y - 1] + x, rgb, object, color, dim);

  /* (x - 1 ; y - 1) */
  if (is_color_ok(x - 1, y - 1, colr, rgb, color, dim))
    search_around(colr, colr->img[y - 1] + x - 1, rgb, object, color, dim);

  /* (x - 1 ; y) */
  if (is_color_ok(x - 1, y, colr, rgb, color, dim))
    search_around(colr, colr->img[y] + x - 1, rgb, object, color, dim);

  /* (x - 1 ; y + 1) */
  if (is_color_ok(x - 1, y + 1, colr, rgb, color, dim))
    search_around(colr, colr->img[y + 1] + x - 1, rgb, object, color, dim);

  /* (x ; y + 1) */
  if (is_color_ok(x, y + 1, colr, rgb, color, dim))
    search_around(colr, colr->img[y + 1] + x, rgb, object, color, dim);

  /* (x + 1 ; y + 1) */
  if (is_color_ok(x + 1, y + 1, colr, rgb, color, dim))
    search_around(colr, colr->img[y + 1] + x + 1, rgb, object, color, dim);
}

inline unsigned long	find_object3(struct s_ptr_uc	*colr, /* color image */
				     UC			*pix, /* pixel color */
				     struct s_ptr_rgb	*rgb, /* RGB image */
				     struct s_colr_rgb	*crgb, /* current RGB pixel */
				     struct s_object	*object,
				     struct s_dim	*dim)
{
  unsigned long		x, y;
  enum e_color		color = *pix;

  *pix = unknown;
  search_around(colr, pix, rgb, object, color, dim);
}

/* set only usefull memory part */
/* void			memset_skip(struct s_ptr_uc *src, int c, unsigned long imgw) */
/* { */
/*     UC			*pix; */
/*     unsigned long	col = 0; */
/*     unsigned long	imgw3 = imgw * (SKIP_PIX_LENGTH - 1); */

/*     for (pix = src->img_beg; pix < src->img_end; pix += (col) ? SKIP_PIX_LENGTH : imgw3) */
/*     { */
/*       col += SKIP_PIX_LENGTH; */
/*       if (col == imgw) */
/* 	col = 0; */
/*       *pix = unknown; */
/*     } */
/* } */

/* void			memset_speed(unsigned long *s, unsigned char c, size_t n) */
/* { */
/*     unsigned long	cw = (c << 24) | (c << 16) | (c << 8) | c; */

/*     for (; n; n -= sizeof (unsigned long), s++) */
/* 	*s = cw; */
/* } */

/* Re-write rgb image with new dim */
/* void			prepare_seek(struct s_dim	*dim, */
/* 				     unsigned long	div_dec, */
/* 				     struct s_ptr_rgb	*rgb) */
/* { */
/*     struct s_colr_rgb	*pr, *pr0, *prend; */
/*     unsigned long	img_sz, imgw, col = 0; */
/*     unsigned long	skip_col = (1 << div_dec), skip_lin; */

/*     imgw = dim->width; */
/*     skip_lin = (skip_col - 1) * imgw; */

/*     img_sz = (dim->height * imgw) >> (div_dec + 1); */
/*     prend = rgb->img_beg + img_sz; */

/*     for (pr = pr0 = rgb->img_beg; pr < prend; pr++, pr0 += (col) ? skip_col : skip_lin)  */
/*     { */
/* 	col += skip_col; */
/* 	if (col == imgw) */
/* 	    col = 0; */
/* 	pr->r = pr0->r; */
/* 	pr->g = pr0->g; */
/* 	pr->b = pr0->b; */
/*     } */
/* } */

/*!
** Generate an object tab from an image
**
** @param env the environment
** @param src the source image
*/
void			rgb_to_objects(struct s_object_list	*object_list,
				       struct s_ptr_uc		*colr,
				       struct s_ptr_rgb		*rgb,
				       struct s_dim		dim)
{
  UC			*pix;
  struct s_colr_rgb	*crgb;
  struct s_object	*obj;
  unsigned long		count = 0;
  enum e_color		color;

  for (pix = colr->img_beg, crgb = rgb->img_beg; 
       (pix < colr->img_end) && (count < MAX_OBJECTS);
       pix++, crgb++)
    {
      if (*pix != unknown)
	continue;
      color = rgbToColor(crgb);
      *pix = color;
      if ((color == red) || (color == blue) || 
	  (color == yellow) || (color == black))
	{
          /* *pix = color; */
	  obj = object_list->objects + count;
	  obj->color = color;
	  obj->cpt = 0;
/* 	  find_object2(colr, pix, rgb, crgb, obj, dim); */
	  find_object3(colr, pix, rgb, crgb, obj, &dim);
	  if ((obj->cpt < SIZE_OBJ_MIN) /*|| (obj->cpt > SIZE_OBJ_MAX)*/)
	      continue;
	  compute_center(obj);
	  /* if (is_valid(obj, &dim)) */
	      count++;
	}
    }

  object_list->count = count;
}
