CC           = gcc
CFLAGS       = -std=c99 -pthread
#CFLAGS       = -O2 -gdwarf -S

#set the following variables for custom libraries and/or other objects
LIBS         = -lncurses

PROGRAM     = MossIM
OBJS = main.o moss.o aes.o client.o server.o bbst.o

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $(CFLAGS) $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(PROGRAM) *~
