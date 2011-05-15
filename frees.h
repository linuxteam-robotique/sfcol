/*
** frees.h for  in /goinfre/wolfer_c/momow/gasser_a-inim/src
**
** Made by charles wolfers
** Login   <wolfer_c@epita.fr>
**
** Started on  Wed Jul 12 14:29:37 2006 charles wolfers
** Last update Wed Jul 12 14:29:38 2006 charles wolfers
*/

#ifndef FREES_H_
# define FREES_H_

# include <stdlib.h>
# include <stdarg.h>

void			allocs_matrix_cont(int	h,
					   int	w,
					   int	size,
					   int	nbmat,
					   ...);

void			frees(int nbptr, ...);

#endif /* !FREES_H_ */
