CC           = gcc
CFLAGS       = -O2 -pthread -std=c99
#CFLAGS       = -O0 -g

#set the following variables for custom libraries and/or other objects
EXTOBJS      =
LIBS         = -lncurses
LIBPATHS     =
INCLUDEPATHS =

PROGRAM     = final
OBJS        = test.o server.o client.o

$(PROGRAM):$(OBJS)
	$(CC) -o $(PROGRAM) $(LIBPATHS) $(CFLAGS) $(OBJS) $(EXTOBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(TUNEFLAGS) $(INCLUDEPATHS) -c $<

clean:
	rm -f $(OBJS) $(PROGRAM) *~

