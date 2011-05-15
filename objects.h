/*
** objects.h for objects in /home/aurelien/momow/gasser_a-inim
**
** Made by aurelien
** Login   <gasser_a@epita.fr>
**
** Started on  Fri Jul 14 02:55:27 2006 aurelien
** Last update Fri Jul 14 04:32:33 2006 aurelien
*/

#ifndef OBJECTS_H_
# define OBJECTS_H_

# include "defines.h"

# define MAX_OBJECTS	32
# define NBR_PIX_MAX	(200 * 200)

struct			s_object
{
  unsigned long		coord[NBR_PIX_MAX][2];
  enum e_color		color;
  unsigned long		cpt;
  unsigned long		x;
  unsigned long		y;
  unsigned long		minx;
  unsigned long		maxx;
  unsigned long		miny;
  unsigned long		maxy;
};

struct			s_object_list
{
  struct s_object	objects[MAX_OBJECTS];
  unsigned long		count;
};

#endif /* !OBJECTS_H_ */
