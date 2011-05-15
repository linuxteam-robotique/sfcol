/*
** frees.c for  in /goinfre/wolfer_c/momow/gasser_a-inim/src
**
** Made by charles wolfers
** Login   <wolfer_c@epita.fr>
**
** Started on  Tue Jul 11 21:23:27 2006 charles wolfers
** Last update Tue Jul 11 21:35:41 2006 charles wolfers
*/

#include "frees.h"

#define PTRSIZE		sizeof (void *)

struct			s_dim
{
  int			sizemat;
  int			sizeptr;
  int			sizelin;
  unsigned		mats;
  unsigned		mate;
  unsigned		lin;
  int			m;
  int			i;
};

union			u_ptruns
{
  void			*ptr;
  unsigned		uns;
};

/*!
** Convert address of unsigned number to void pointer
**
** @param uns number to convert
**
** @return void pointer with the uns address
*/
static void		*UnsToPtr(unsigned uns)
{
  union u_ptruns	pu;

  pu.uns = uns;
  return pu.ptr;
}

/*!
** Convert address of void pointer to unsigned value
**
** @param ptr void pointer to convert
**
** @return address of ptr void pointer
*/
static unsigned		PtrToUns(void *ptr)
{
  union u_ptruns	pu;

  pu.ptr = ptr;
  return pu.uns;
}

/*!
** Allocate nbmat matrix of h x w elements of size bytes and
** set each bytes with zero (calloc);
**
** @param h height of matrix
** @param w weight of matrix
** @param size size in byte(s) of an element of matrix
** @param nbmat matrix number
*/
void			allocs_matrix_cont(int	h,
					   int	w,
					   int	size,
					   int	nbmat,
					   ...)
{
  struct s_dim		d;
  va_list		ap;
  void			**ptr;
  void			**pmat;
  void			**pmatb;
  void			**pmate;

  d.sizemat = (d.sizeptr = h * PTRSIZE) + h * (d.sizelin = w * size);
  d.mats = PtrToUns(calloc(d.sizemat * nbmat, 1));
  va_start(ap, nbmat);
  for (d.m = 0; d.m < nbmat; d.m++, *pmate = UnsToPtr(d.mats += d.sizemat))
    {
      pmat = va_arg(ap, void **);
      *pmat = UnsToPtr(d.mate = d.mats);
      pmatb = va_arg(ap, void **);
      *pmatb = UnsToPtr(d.lin = d.mate + d.sizeptr);
      for (d.i = 0; d.i < h; d.i++, d.mate += PTRSIZE, d.lin += d.sizelin)
	{
	  ptr = UnsToPtr(d.mate);
	  *ptr = UnsToPtr(d.lin);
	}
      pmate = va_arg(ap, void **);
    }
  va_end(ap);
}

/*!
** Call free() on each pointer in arguments
**
** @param nbptr number of pointer
*/
void			frees(int nbptr, ...)
{
  int			i;
  va_list		ap;
  void			*ptr;

  va_start(ap, nbptr);
  for (i = 0; i < nbptr; i++)
    {
      ptr = va_arg(ap, void *);
      free(ptr);
    }
  va_end(ap);
}
