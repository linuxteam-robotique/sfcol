/*
** rgb_to_objects.h for rgb_to_objects in /home/aurelien/momow/gasser_a-inim
**
** Made by aurelien
** Login   <gasser_a@epita.fr>
**
** Started on  Fri Jul 14 02:44:54 2006 aurelien
** Last update Fri Jul 14 03:29:44 2006 aurelien
*/

#ifndef RGB_TO_OBJECTS_H_
# define RGB_TO_OBJECTS_H_

#include "defines.h"
#include "objects.h"

void			rgb_to_objects(struct s_object_list	*object_list,
				       struct s_ptr_uc		*src,
				       struct s_ptr_rgb		*rgb,
				       struct s_dim		dim
/*				       UC                       *picture,
				       SDL_Surface              *pscreen
*/    );

//void			memset_skip(struct s_ptr_uc *src, int c, unsigned long imgw);
void			memset_speed(unsigned long *s, unsigned char c, size_t n);
void			prepare_seek(struct s_dim	*dim,
				     unsigned long	div_dec,
				     struct s_ptr_rgb	*rgb);

#endif /* !RGB_TO_OBJECTS_H_ */
