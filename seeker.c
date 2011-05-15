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

#include <pthread.h>
/* #include <mjpegtools/avilib.h> */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "v4l2uvc.h"
#include "seeker.h"
#include "exit_error.h"
#include "sfcol.h"
#include "frees.h"
#include "graber.h"
#include "jpeg_to_rgb.h"
#include "defines.h"
#include "ms_time.h"
#include "objects.h"
#include "server.h"
#include "rgb_to_objects.h"
#include "ramdisk.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define JPG_FILENAME	"sfcol.jpg"

/* Source code handling image_lock mutex */

static pthread_mutex_t	init_graber_comm_done;
static pthread_mutex_t	images_lock;
static int		processed_img = 0;
static unsigned char	*images[2] = { NULL, NULL };
static unsigned long	images_size[2] = { 0, 0 };

/* This function MUST be call by the main program (not a thread) */
void		init_graber_comm(int width, int height)
{
  int		img_max_size;
  
  img_max_size = width * height * 4;
  images[0] = (unsigned char *) malloc(img_max_size);
  images[1] = (unsigned char *) malloc(img_max_size);

  pthread_mutex_init(&init_graber_comm_done, NULL);
  pthread_mutex_init(&images_lock, NULL);

  pthread_mutex_lock(&init_graber_comm_done);
  pthread_mutex_unlock(&images_lock);
}

/* This function MUST be call by the main program (not a thread) */
void		free_graber_comm(void)
{
  pthread_mutex_destroy(&init_graber_comm_done);
  pthread_mutex_destroy(&images_lock);
}

void		push_seeker_img(unsigned char *img, unsigned long img_size)
{
  int		new_img;

  pthread_mutex_lock(&images_lock);
  new_img = (processed_img) ? 0 : 1;
  memcpy(images[new_img], img, img_size);
  images_size[new_img] = img_size;
  pthread_mutex_unlock(&images_lock); 
  pthread_mutex_unlock(&init_graber_comm_done);
}

inline static void	pop_seeker_img_begin(unsigned char **img, 
					     unsigned long *img_size)
{
  pthread_mutex_lock(&images_lock);
  processed_img = (processed_img) ? 0 : 1;
  *img = images[processed_img];
  *img_size = images_size[processed_img];
}

inline static void	pop_seeker_img_end(void)
{
  pthread_mutex_unlock(&images_lock);
}

static void	wait_for_next_image(void)
{
  pthread_mutex_lock(&images_lock);
  processed_img = (processed_img) ? 0 : 1;
  pthread_mutex_unlock(&images_lock);
  pthread_mutex_lock(&init_graber_comm_done);
}

/* End of source code handling image_lock mutex */

void		seeker(struct s_seeker_arg	*seeker_arg)
{
  struct vdIn		*videoIn = seeker_arg->videoIn;
  /* char			*movie_filename = seeker_arg->movie_filename; */
  /* avi_t			*fvid; */
  unsigned long		nbr_img = 0;
  unsigned char		*img;
  unsigned long		img_size;
  int			f;
  struct s_ptr_rgb	rgb;
  struct s_ptr_uc	colr;
  double		start_tim = ms_time();
  double		tim;
  struct s_object_list	*object_list;
  struct s_dim		new_dim = { videoIn->width, videoIn->height };

  allocs_matrix_cont(videoIn->height, videoIn->width, 
		     sizeof (struct s_colr_rgb), 1,
		     &(rgb.img), &(rgb.img_beg), &(rgb.img_end));

  allocs_matrix_cont(videoIn->height, videoIn->width, sizeof (UC), 1,
		     &(colr.img), &(colr.img_beg), &(colr.img_end));

  object_list = malloc(sizeof (struct s_object_list));
  if (!object_list)
    {
      fprintf(stderr, 
	      "\033[41mOut of memory on object_list allocation!!!\033[0m\n");
      run = 0;
      frees(2, rgb.img, colr.img);
      return;
    }

  wait_for_next_image();
  wait_for_next_image();
  /* Here: We are sure the two images were gotten */

  /* if (movie_filename) */
  /*   { */
  /*     fvid = AVI_open_output_file(movie_filename); */
  /*     if (!fvid) */
  /* 	{ */
  /* 	  perror("Create movie"); */
  /* 	  close_v4l2(videoIn); */
  /* 	  free(videoIn); */
  /* 	  exit(SEEKER_CREAT_MOVIE_ERROR); */
  /* 	} */
  /*     AVI_set_video(fvid, videoIn->width, videoIn->height, 20, "MJPG"); */
  /*   } */

  while (run)
    {
      pop_seeker_img_begin(&img, &img_size);

      printf("Processing image #%lu\n", nbr_img); fflush(stdout);

      /* if (movie_filename) */
      /* 	AVI_write_frame(fvid, (char *) img, img_size, nbr_img); */

      send_image(img, img_size, nbr_img);

      f = open(JPG_TEMP_PATH JPG_FILENAME, 
	       O_CREAT | O_TRUNC | O_WRONLY, 
	       S_IRUSR | S_IWUSR);
      write(f, img, img_size);
      close(f);

      jpeg_to_rgb(JPG_TEMP_PATH JPG_FILENAME, rgb.img_beg);

      pop_seeker_img_end();

      img_size = videoIn->width * videoIn->height;
      memset((unsigned long *) colr.img_beg, unknown, img_size);

      /* f = open("image.ppm", O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR); */
      /* write(f, "P6\n640 480\n255\n", 3+8+4); */
      /* write(f, rgb.img_beg, 640*480*3); */
      /* close(f); */

      rgb_to_objects(object_list, &colr, &rgb, new_dim);
      send_info(object_list);

      printf("]\n");
      fflush(stdout);

      ++nbr_img;
    }

  /* if (movie_filename) */
  /*   AVI_close(fvid); */

  frees(2, rgb.img, colr.img);

  tim = ms_time() - start_tim;
  printf("%lu images traitees durant %lf secondes (%lu images lues)\n"
	 "%lf images par seconde ; %lf ms par image\n",
	 nbr_img, (tim / 1000.0), nbr_grab,
	 nbr_img / (tim / 1000.0), tim / nbr_img);
  fflush(stdout);
}
