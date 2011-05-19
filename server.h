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

#ifndef SERVER_H_
# define SERVER_H_

# include "objects.h"

struct			s_server_arg
{
  unsigned short	server_port;
};

void	dispose_server(void);

void	send_info(struct s_object_list *object_list);
void	send_image(unsigned char *img, unsigned long img_size, unsigned long nbr_img);

void	server(struct s_server_arg *server_arg);

#endif /* !SERVER_H_ */
