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
#include <stdlib.h>
#include <stdio.h>
#include "graber.h"
#include "exit_error.h"
#include "sfcol.h"
#include "v4l2uvc.h"
#include "seeker.h"

unsigned long nbr_grab = 0;

void	graber(struct s_graber_arg *graber_arg)
{
  struct vdIn		*videoIn = graber_arg->videoIn;
  unsigned long		img_size;
  int			count = 10;
    
  while (run)
    {
      if (uvcGrab(videoIn, 1) < 0)
	{
	  fprintf(stderr, "Error grabbing\n");
	  close_v4l2(videoIn);
	  free(videoIn);
	  exit(V4L2UVC_GRAB_ERROR);
	}

      if (count)
	{
	  --count;
	  videoIn->getPict = 0;
	  continue;
	}
      img_size = videoIn->buf.bytesused + DHT_SIZE;
      push_seeker_img(videoIn->tmpbuffer, img_size);
      videoIn->getPict = 0;

      ++nbr_grab;
    }
}
