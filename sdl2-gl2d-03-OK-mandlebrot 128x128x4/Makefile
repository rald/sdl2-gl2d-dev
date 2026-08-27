CC = gcc
# -Ofast turns on aggressive architecture optimizations just like modern high-performance ports
CFLAGS = -Wall -Wextra -Ofast -march=native
LIBS = $(shell pkg-config --cflags --libs sdl2)

TARGET = game
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC) gl2d.h
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run
