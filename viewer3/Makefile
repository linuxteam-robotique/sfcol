CC = gcc
PROG = viewer
CFLAGS = -W -Wall -g
LDFLAGS = -ljpeg `sdl-config --libs`
#-llavfile

OBJS = viewer.o image_format.o v4l2uvc.o graber.o seeker.o \
       frees.o jpeg_to_rgb.o ms_time.o rgb_to_objects.o \
       maths.o 

all:	$(PROG)

clean:
	@echo "Cleaning up directory."
	rm -f *.a *.o $(PROG) core *~ log errlog

$(PROG): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

viewer.o:		viewer.c
	$(CC) -g `sdl-config --cflags` -c $< -I..

frees.o:		../frees.c ../frees.h
	$(CC) -g `sdl-config --cflags` -c $< -I..

maths.o:		../maths.c ../maths.h
	$(CC) -g `sdl-config --cflags` -c $< -I..

rgb_to_objects.o:	../rgb_to_objects.c ../rgb_to_objects.h ../objects.h
	$(CC) -g `sdl-config --cflags` -c $< -I..

image_format.o:		../image_format.c ../image_format.h
	$(CC) -g `sdl-config --cflags` -c $< -I..

v4l2uvc.o:		../v4l2uvc.c ../v4l2uvc.h
	$(CC) -g `sdl-config --cflags` -c $< -I..

graber.o:		../graber.c ../graber.h
	$(CC) -g `sdl-config --cflags` -c $< -I..

seeker.o:		../seeker.c ../seeker.h
	$(CC) -g `sdl-config --cflags` -c $< -I..

jpeg_to_rgb.o:		../jpeg_to_rgb.c ../jpeg_to_rgb.h
	$(CC) -g `sdl-config --cflags` -c $< -I..

ms_time.o:		../ms_time.c ../ms_time.h
	$(CC) -g `sdl-config --cflags` -c $< -I..
