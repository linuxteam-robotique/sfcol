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

#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <unistd.h>
#include "defines.h"

#define V(X) printf(X "\n");fflush(stdout);

struct		my_error_mgr
{
  struct jpeg_error_mgr	pub;           /* "public" fields */
  jmp_buf		setjmp_buffer; /* for return to caller */
};

static void	rgb2bgr(struct s_colr_rgb *row, unsigned long width)
{
  unsigned char	r;

  for (; width; --width, ++row)
    {
      r = row->r;
      row->r = row->b;
      row->b = r;
    }
}

int	jpeg_to_rgb(char *jpg_img, struct s_colr_rgb *rgb_img)
{
  struct jpeg_decompress_struct	cinfo;
  struct my_error_mgr		jerr;
  FILE				*infile;
  int				row_stride;
  struct s_colr_rgb		*row;

  if ((infile = fopen(jpg_img, "rb")) == NULL)
    {
      fprintf(stderr, "can't open %s\n", jpg_img);
      return -1;
    }

  cinfo.err = jpeg_std_error(&jerr.pub);
  jpeg_create_decompress(&cinfo);

  jpeg_stdio_src(&cinfo, infile);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);

  /* rgb->width = cinfo.output_width; */
  /* rgb->height = cinfo.output_height; */

  row_stride = cinfo.output_width * cinfo.output_components;
  while (cinfo.output_scanline < cinfo.output_height)
    {
      row = rgb_img + cinfo.output_scanline * cinfo.output_width;
      jpeg_read_scanlines(&cinfo, (JSAMPARRAY) &row, 1);
      rgb2bgr(row, cinfo.output_width);
    }
				      
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(infile);

  if (jerr.pub.num_warnings != 0)
    {
      fprintf(stderr, "Error during jpeg reading!\n");
      return -4;
    }

  return 0;
}
