TARGET = MossIM
default: $(TARGET)

BUILD_DIR = obj

SOURCES = main.c \
src/bbst.c \
src/client.c \
src/moss.c \
src/server.c

INCLUDES = -Iinclude


CC = gcc
OPT = -O1
CFLAGS = $(INCLUDES) $(OPT) -Wall
LIBS = -lncurses -lpthread


DEBUG = 0
ifeq ($(DEBUG), 1)
	CFLAGS += -gdwarf
endif


#Cargo cult for right now, actually learn what this does
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(SOURCES)))

$(BUILD_DIR)/%.o: %.c
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $@


clean:
	-rm -f $(BUILD_DIR)/*
	-rm $(TARGET)
