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
#include <sys/ioctl.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include "exit_error.h"
#include "v4l2uvc.h"
#include "image_format.h"
#include "graber.h"
#include "seeker.h"
#include "server.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))
#define READ_METHOD 0
#define MMAP_METHOD 1

int	run = 1;

static void	sigcatch()
{
  run = 0;
  dispose_server();
  fprintf(stderr, "Exiting...\n");
}

static void		exec_threads(struct vdIn	*videoIn,
				     char		*movie_filename,
				     unsigned short	server_port)
{
  pthread_t		thread_graber;
  pthread_t		thread_seeker;
  pthread_t		thread_server;
  struct s_graber_arg	graber_arg = { videoIn };
  struct s_seeker_arg	seeker_arg = { videoIn, movie_filename };
  struct s_server_arg	server_arg = { server_port };

  init_graber_comm(videoIn->width, videoIn->height);

  pthread_create(&thread_graber, NULL, (void *) graber, &graber_arg);
  pthread_create(&thread_seeker, NULL, (void *) seeker, &seeker_arg);
  pthread_create(&thread_server, NULL, (void *) server, &server_arg);

  pthread_join(thread_graber, NULL);
  pthread_join(thread_seeker, NULL);
  /* pthread_join(thread_server, NULL); */
  pthread_cancel(thread_server);

  free_graber_comm();
}

static void		display_usage(char *argv[], FILE *out_stream)
{
  fprintf(out_stream, 
	  "Usage: %s "
	  "[-h] [-d video_device] [-s widthxheight] [-p server_port] "
	  "[-o movie.avi]\n",
	  argv[0]);
}

static void		display_help(char *argv[])
{
  display_usage(argv, stdout);
  printf("-h              => this help message\n");
  printf("-d video_device => to indicate what video device you wish to use\n");
  printf("-s widthxheight => to indicate the width and height pixel sizes\n");
  printf("-p server_port  => to set the port what the server will use\n");
  printf("-o movie.avi    => to ask images to be saved into a movie file (MJPEG)\n");
}

int			main(int argc, char *argv[])
{
  int			opt;
  int			width = 640;
  int			height = 480;
  int			format = V4L2_PIX_FMT_MJPEG;
  char			*video_device = strdup("/dev/video0");
  char			*movie_filename = NULL;
  unsigned short	server_port = 7070;
  struct vdIn		*videoIn;

  videoIn = (struct vdIn *) calloc(1, sizeof (struct vdIn));
  if (!videoIn)
    exit(VDIN_ALLOC_ERROR);

  while ((opt = getopt(argc, argv, "hd:s:p:o:")) != -1)
    switch (opt)
      {
      case 'h':
	display_help(argv);
	exit(EXIT_SUCCESS);
      case 'd':
	free(video_device);
	video_device = strdup(optarg);
	break;
      case 's':
	set_dim(&width, &height, optarg);
	break;
      case 'p':
	server_port = atoi(optarg);
	break;
      case 'o':
	movie_filename = strdup(optarg);
	break;
      default:
	display_usage(argv, stderr);
	exit(EXIT_FAILURE);
      }

  signal(SIGINT, sigcatch);
  signal(SIGQUIT, sigcatch);
  signal(SIGKILL, sigcatch);
  signal(SIGTERM, sigcatch);
  signal(SIGABRT, sigcatch);
  signal(SIGTRAP, sigcatch);

  if (init_videoIn(videoIn, video_device, width, height, format, 
		   MMAP_METHOD) < 0)
    exit(V4L2UVC_INIT_ERROR);

  /* Reset all camera controls */
  v4l2ResetControl(videoIn, V4L2_CID_BRIGHTNESS);
  v4l2ResetControl(videoIn, V4L2_CID_CONTRAST);
  v4l2ResetControl(videoIn, V4L2_CID_SATURATION);
  v4l2ResetControl(videoIn, V4L2_CID_GAIN);

  /* Setup Camera Parameters */
  fprintf(stderr, "Camera brightness level is %d\n",
	  v4l2GetControl (videoIn, V4L2_CID_BRIGHTNESS));
  fprintf(stderr, "Camera contrast level is %d\n",
	  v4l2GetControl (videoIn, V4L2_CID_CONTRAST));
  fprintf(stderr, "Camera saturation level is %d\n",
	  v4l2GetControl (videoIn, V4L2_CID_SATURATION));
  fprintf(stderr, "Camera gain level is %d\n",
	  v4l2GetControl (videoIn, V4L2_CID_GAIN));

  exec_threads(videoIn, movie_filename, server_port);

  close_v4l2(videoIn);
  free(videoIn);
  free(video_device);
  if (movie_filename)
    free(movie_filename);

  return EXIT_SUCCESS;
}
