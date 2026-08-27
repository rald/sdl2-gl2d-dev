# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O3
# Fetch SDL2 flags and include math library (-lm)
LIBS = $(shell pkg-config --cflags --libs sdl2) -lm

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
