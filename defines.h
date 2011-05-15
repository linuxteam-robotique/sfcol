/*
** defines.h for  in /goinfre/wolfer_c/momow/gasser_a-inim/src
**
** Made by charles wolfers
** Login   <wolfer_c@epita.fr>
**
** Started on  Wed Jul 12 14:07:42 2006 charles wolfers
** Last update Fri Jul 14 13:59:37 2006 charles wolfers
*/

#ifndef DEFINES_H_
# define DEFINES_H_

# define EXIT_NBR_ARG	1
# define EXIT_FILE_NF	2
# define EXIT_BAD_COLR	3
# define EXIT_IMG_HEAD	10
# define EXIT_IMG_DIM	11
# define EXIT_IMG_PAL	12
# define EXIT_READ_IMG	13

# define UC unsigned char

/* Definition des images */
/* # define X_LEN_IMG	env->imgw */
/* # define Y_LEN_IMG	env->imgh */
/* # define TOTAL_NBR_PIX_IMG (X_LEN_IMG * Y_LEN_IMG) */

/* Definition des limites des images */
/* # define X_MAX_IMG	(X_LEN_IMG - 1) */
/* # define Y_MAX_IMG	(Y_LEN_IMG - 1) */
/* # define ZERO		0 */
/* # define ONE		1 */

/* # define NBR_STEPS_IMAGES	1 */

# define UC2F(R, G, B, FR, FG, FB)	\
  FR = R / 255.0;			\
  FG = G / 255.0;			\
  FB = B / 255.0;

# define F2UC(FR, FG, FB, R, G, B)	\
  R = rint(FR * 255.0);			\
  G = rint(FG * 255.0);			\
  B = rint(FB * 255.0);

# define RGB2COLOR(R, G, B, Dest)	\
  Dest->r = R;				\
  Dest->g = G;				\
  Dest->b = B;

# define COLOR2RGB(Src, R, G, B)	\
  R = Src->r;				\
  G = Src->g;				\
  B = Src->b;

# define HLS2COLOR(H, L, S, Dest)	\
  Dest->h = H;				\
  Dest->l = L;				\
  Dest->s = S;

/* enum		e_proc */
/* { */
/* 		p_erosion, */
/* 		p_dilatation, */
/* 		p_ouverture, */
/* 		p_fermeture, */
/* 		p_moyenne, */
/* 		p_median, */
/* 		p_sobel, */
/* 		p_sort_by_size, */
/* 		p_unchamp, */
/* 		p_unchamp_sort */
/* }; */

struct			s_dim
{
    unsigned long	width;
    unsigned long	height;
};

/* struct		s_colr_rgb */
/* { */
/*   UC		r; */
/*   UC		g; */
/*   UC		b; */
/* } __attribute__ ((packed)); */

struct		s_colr_rgb
{
  UC		b;
  UC		g;
  UC		r;
} __attribute__ ((packed));

struct		s_colr_hls
{
  double	h;
  double	l;
  double	s;
} __attribute__ ((packed));

struct			s_ptr_uc
{
  UC			**img;
  UC			*img_beg;
  UC			*img_end;
};

struct			s_ptr_rgb
{
  struct s_colr_rgb	**img;
  struct s_colr_rgb	*img_beg;
  struct s_colr_rgb	*img_end;
};

struct			s_ptr_hls
{
  struct s_colr_hls	**img;
  struct s_colr_hls	*img_beg;
  struct s_colr_hls	*img_end;
};

extern char	color_name[][10];

/*!
** \brief The cars' color
*/
enum		e_color
{
		unused,
		white,
		red,       //   0 deg
		brown,	   //  30 deg
		orange,	   //  30 deg (+ lumineux que marron)
		yellow,	   //  60 deg
		green,	   // 120 deg
		cyan,	   // 180 deg
		blue,	   // 240 deg
		sky_blue,  // 240 deg (+ lumineux que bleu)
		magenta,   // 300 deg
		pink,	   // 300 deg (+ lumineux que magenta)
		black,
		unknown,
		considered
};

/* struct			s_pix1 */
/* { */
/*   unsigned char		r; */
/*   unsigned char		g; */
/*   unsigned char		b; */
/* } __attribute__ ((packed)); */

/* struct			s_pix2 */
/* { */
/*   unsigned short	r; */
/*   unsigned char		g; */
/*   unsigned char		b; */
/* } __attribute__ ((packed)); */

/*!
** \brief filename: image filename
**        car: car color
**        near_car: car color of the near car
**        isgray: 0 for a gray levels image, 1 for color image
**        imgh: image height
**        imgw: imahe weight
**        max_gray: colors number
**        sz_img: size of filename image
**        sz_pix: size of pixel of filename image
**        img: filename image
**        nbr_min, nbr_max, threshold, num_img
**        num_img_r1, num_img_g1, num_img_b1,
**        num_img_r2, num_img_g2, num_img_b2 : temporaries variables
**        r1, g1, b1, r2, g2, b2: temporary images
*/
/* struct			s_env */
/* { */
/*   char			*filename; */
/*   enum e_color		car; */
/*   enum e_color		near_car; */
/*   int			isgray; */
/*   long			imgh; */
/*   long			imgw; */
/*   long			max_gray; */
/*   unsigned long		sz_img; */
/*   unsigned char		sz_pix; */
/*   struct s_ptr_rgb	img; */
/*   unsigned short	nbr_min; */
/*   unsigned short	nbr_max; */
/*   unsigned char		threshold; */
/*   unsigned char		num_img; */
/*   unsigned char		num_img_r1; */
/*   unsigned char		num_img_g1; */
/*   unsigned char		num_img_b1; */
/*   unsigned char		num_img_r2; */
/*   unsigned char		num_img_g2; */
/*   unsigned char		num_img_b2; */
/*   struct s_ptr_uc	r1; */
/*   struct s_ptr_uc	b1; */
/*   struct s_ptr_uc	g1; */
/*   struct s_ptr_uc	r2; */
/*   struct s_ptr_uc	b2; */
/*   struct s_ptr_uc	g2; */
/*   struct s_ptr_uc	steps[NBR_STEPS_IMAGES]; */
/*   struct s_ptr_rgb	steps_colr[NBR_STEPS_IMAGES]; */
/*   struct s_object_list	*object_list; */
/* }; */

#endif /* !DEFINES_H_ */
