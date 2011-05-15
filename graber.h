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

#ifndef GRABER_H_
# define GRABER_H_

struct		s_graber_arg
{
  struct vdIn	*videoIn;
};

extern unsigned long nbr_grab;

void	graber(struct s_graber_arg *graber_arg);

#endif /* !GRABER_H_ */
