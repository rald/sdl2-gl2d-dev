# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O3
# Automatically fetch SDL2 include and linker paths
LIBS = $(shell pkg-config --cflags --libs sdl2)

# Target executable name
TARGET = game

# Source files
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC) gl2d.h
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run
