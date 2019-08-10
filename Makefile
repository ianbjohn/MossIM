CC           = gcc
CFLAGS       = -pthread -std=c99
#CFLAGS       = -O2 -g

#set the following variables for custom libraries and/or other objects
EXTOBJS      =
LIBS         = -lncurses
LIBPATHS     =
INCLUDEPATHS =

PROGRAM     = final
OBJS        = test.o server.o client.o aes.o

$(PROGRAM):$(OBJS)
	$(CC) -o $(PROGRAM) $(LIBPATHS) $(CFLAGS) $(OBJS) $(EXTOBJS) $(LIBS)
aes_test: aes.o aes_test.o
	gcc -o aes_test -std=c99 -gdwarf aes.o aes_test.o

.c.o:
	$(CC) $(CFLAGS) $(TUNEFLAGS) $(INCLUDEPATHS) -c $<

clean:
	rm -f $(OBJS) $(PROGRAM) *~
	rm -f aes_test.o aes_test *~

