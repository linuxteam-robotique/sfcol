CC = gcc
PROG = catcher
CFLAGS = -W -Wall -g
LDFLAGS =

all: $(PROG)

$(PROG): catcher.c
	$(CC) $< -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f *.a *.o core *~ log errlog

distclean: clean
	rm -f $(PROG)
