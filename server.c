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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "server.h"
#include "objects.h"
#include "color_name.h"
#include "sfcol.h"

enum	e_proto
{
  proto_sfcol2
};

static enum e_proto	protocol = proto_sfcol2;
static int		data_to_send = -1; /* 0 = jpeg ; 1 = coord */
static int		client_sockfd = -1;
static int		connect_active = 0;

static int		server_sockfd;


static void	catch_sigpipe(/*int sig*/)
{
  connect_active = 0;
  printf("Client has disconnected!\n");
  fflush(stdout);
}

void	dispose_server(void)
{
  connect_active = 0;
  shutdown(client_sockfd, SHUT_RDWR);
  close(client_sockfd);
  close(server_sockfd);
}

static void	send_info_proto_sfcol2(struct s_object_list *object_list)
{
  unsigned int		i;
  struct s_object	*object1;
  /* unsigned long		count = 0; */
  int			info_to_send = connect_active && (data_to_send == 1);

  if (info_to_send)
    {
      write(client_sockfd, &(object_list->count), 4);
      /* read(client_sockfd, &count, 4); */
    }
  for (i = 0; i < object_list->count; ++i)
    {
      object1 = object_list->objects + i;
      printf("%s\t=> (%lu ; %lu)  %lu pixels\n",
	     color_name[object1->color],
	     object1->y,
	     object1->x,
	     object1->cpt);
      if (info_to_send)
	{
	  write(client_sockfd, &(object1->color), 1);
	  write(client_sockfd, &(object1->x), 4);
	  write(client_sockfd, &(object1->y), 4);
	}
    }
  printf("%lu %i %i\n", object_list->count, data_to_send, client_sockfd);
}

void	send_info(struct s_object_list *object_list)
{
  switch (protocol)
    {
    case proto_sfcol2:
      send_info_proto_sfcol2(object_list);
      break;
    }
}

void	send_image(unsigned char *img, unsigned long img_size, unsigned long nbr_img)
{
  char	buf_read[1024];
  int	err;

  if (connect_active && (data_to_send == 0))
    {
      send(client_sockfd, &img_size, 4, 0);
      err = recv(client_sockfd, buf_read, 4, 0);
      if (err != -1)
	{
	  send(client_sockfd, img, img_size, 0);
	  err = recv(client_sockfd, buf_read, 4, 0);
	  send(client_sockfd, &nbr_img, 4, 0); 
	}
    }
}

void	server(struct s_server_arg *server_arg)
{
  int			server_len;
  int			new_client_sockfd;
  socklen_t		client_len;
  struct sockaddr_in	server_address;
  struct sockaddr_in	client_address;

  signal(SIGPIPE, catch_sigpipe);

  server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(server_arg->server_port);
  server_len = sizeof (server_address);
  /*     getsockname(server_sockfd,  */
  /* (struct sockaddr *) &server_address, */
  /* &server_len); // Get a free port number */
  bind(server_sockfd,
       (struct sockaddr *) &server_address,
       sizeof (server_address));
  printf("Server listen port %i\n", htons(server_address.sin_port));
  printf("Server @%08X\n", server_address.sin_addr.s_addr);

  listen(server_sockfd, 1);
  while (run)
    {
      client_len = sizeof (client_address);
      new_client_sockfd = accept(server_sockfd,
				 (struct sockaddr *) &client_address,
				 &client_len);
      if (client_sockfd != -1)
	close(client_sockfd);
      client_sockfd = new_client_sockfd;

      read(client_sockfd, &data_to_send, 4);

      connect_active = 1;
    }
  /* pthread_mutex_lock(&server_stop); //Lock */
  /* pthread_mutex_lock(&server_stop); //Stop ... wait unlock! */

  printf("Server: J'ai fini mon service !!! :D\n");
}
