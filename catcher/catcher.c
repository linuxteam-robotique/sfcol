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

#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "../objects.h"
#include "../color_name.h"

static int		run = 1;

static void		catch_sig(/*int sig*/)
{
  run = 0;
  fprintf(stderr, "Signal receved => end client\n");
  fflush(stderr);
}

static void		display_usage(char *argv[], FILE *out_stream)
{
  fprintf(out_stream, 
	  "Usage: %s [-h] [-w hostname:server_port] ",
	  argv[0]);
}

static void		display_help(char *argv[])
{
  display_usage(argv, stdout);
  printf("-h                      => this help message\n");
  printf("-w hostname:port => connect to _hostname_ on _port_ server port\n");
}

static void		get_ip_port(char		**hostname, 
				    unsigned short	*port, 
				    char		*hostname_ip)
{
  char			*semicolon;

  semicolon = index(hostname_ip, ':');
  if ((semicolon == NULL) || (*semicolon != ':'))
    exit(2);
  *semicolon = '\0';
  *hostname = strdup(hostname_ip);
  *port = atoi(semicolon + 1);
}

static int		connect_server(char *hostname, unsigned short port)
{
  int			sock;
  int			r;
  struct sockaddr_in	address;
  socklen_t		addr_len;

  sock = socket(AF_INET, SOCK_STREAM, 0);

  memset(&address, 0, sizeof (address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(hostname);
  address.sin_port = htons(port);
  addr_len = sizeof (address);

  r = connect(sock, (struct sockaddr *) &address, addr_len);
  if (r == -1)
    {
      perror("connect");
      exit(3);
    }

  return sock;
}

static void		catch_objects(char *hostname, unsigned short port)
{
  int			sock;
  int			data_to_send = 1; /* coord */
  unsigned long		objno;
  struct s_object_list	*ol; /* Avoid allocation on stack: too big size! */

  ol = malloc(sizeof (struct s_object_list));
  if (!ol)
    {
      perror("malloc");
      exit(4);
    }

  printf("Connect to '%s' on port #%i\n", hostname, port);
  fflush(stdout);

  signal(SIGPIPE, catch_sig);
  signal(SIGINT, catch_sig);

  sock = connect_server(hostname, port);
  write(sock, &data_to_send, 4);

  while (run)
    {
      memset(ol, 0, sizeof (struct s_object_list));

      read(sock, &(ol->count), 4);
      printf("Object count: %lu\n", ol->count);
      fflush(stdout);
      /* write(sock, &(ol->count), 4); */

      if (ol->count > MAX_OBJECTS)
	ol->count = MAX_OBJECTS;
      for (objno = 0; objno < ol->count; ++objno)
  	{
  	  read(sock, &(ol->objects[objno].color), 1);
  	  read(sock, &(ol->objects[objno].x), 4);
  	  read(sock, &(ol->objects[objno].y), 4);
  	  printf("%s\t=> (%lu ; %lu)\n",
  		 color_name[ol->objects[objno].color],
  		 ol->objects[objno].y,
  		 ol->objects[objno].x);
  	  fflush(stdout);
  	}
    }

  free(ol);

  shutdown(sock, SHUT_RDWR);
  close(sock);
}

int			main(int argc, char *argv[])
{
  int			opt;
  char			*hostname = strdup("127.0.0.1");
  unsigned short	port = 7070;

  while ((opt = getopt(argc, argv, "hw:")) != -1)
    switch (opt)
      {
      case 'h':
	display_help(argv);
	break;
      case 'w':
	free(hostname);
	get_ip_port(&hostname, &port, optarg);
	break;
      default:
	display_usage(argv, stderr);
	return EXIT_FAILURE;
      }
  catch_objects(hostname, port);
  free(hostname);
  return EXIT_SUCCESS;
}
