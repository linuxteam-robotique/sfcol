/*
  sfcol - Search For Color
  Copyright (C) 2004 Charles Wolfers

  This file is part of sfcol.

  sfcol is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  sfcol is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with sfcol.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include "v4l2uvc.h"
#include "exit_error.h"

void		set_dim(int *width, int *height, char *dim)
{
  char		*mul_sign;
  
  *width = strtoul(dim, &mul_sign, 10);
  if (*mul_sign != 'x')
    exit(IMAGE_FORMAT_SET_DIM_ERROR);
  *height = strtoul(mul_sign + 1, NULL, 10);
}

