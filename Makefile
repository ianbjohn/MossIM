CC           = gcc
CFLAGS       = -std=c99
#CFLAGS       = -O2 -gdwarf -S

#set the following variables for custom libraries and/or other objects
LIBS         = -lncurses

PROGRAM     = MossIM
INCLUDEPATHS = -I./include
#clean this up with wildcards
OBJSDIR = ./objs
OBJS = $(wildcard $(OBJSDIR)/*.o)

$(PROGRAM): $(OBJS)
	$(CC) -o $(CFLAGS)

clean:
	rm -f $(OBJS) $(PROGRAM) *~
