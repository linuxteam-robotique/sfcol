CC = gcc
PROG = sfcol
CFLAGS = -W -Wall -g
LDFLAGS = -lv4l2 -ljpeg -lpthread
#-llavfile
#-lmjpegutils

OBJSERVER = sfcol.o image_format.o v4l2uvc.o graber.o seeker.o \
	    frees.o jpeg_to_rgb.o ms_time.o rgb_to_objects.o \
	    maths.o server.o
#utils.o encoder.o huffman.o quant.o marker.o

all:	$(PROG)

clean:
	@echo "Cleaning up directory."
	rm -f *.a *.o $(PROG) core *~ log errlog

$(PROG): $(OBJSERVER)
	$(CC) $(LDFLAGS) -o $@ $(OBJSERVER)

sfcol.o:	sfcol.c sfcol.h
image_format.o:	image_format.c image_format.h
v4l2uvc.o:	v4l2uvc.c v4l2uvc.h
graber.o:	graber.c graber.h
seeker.o:	seeker.c seeker.h
frees.o:	frees.c frees.h
jpeg_to_rgb.o:	jpeg_to_rgb.c jpeg_to_rgb.h
ms_time.o:	ms_time.c ms_time.h
rgb_to_objets.o:	rgb_to_objects.c rgb_to_objects.h objects.h
maths.o:	maths.c maths.h tab_hypot.h tab_phasearg.h tab_racine.h
server.o:	server.c server.h color_name.h
# utils.o:	utils.c utils.h defines.h jdatatype.h encoder.h
# encoder.o:	encoder.c encoder.h huffman.h quant.h marker.h
# huffman.o:	huffman.c huffman.h
# quant.o:	quant.c quant.h
# marker.o:	marker.c marker.h

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
