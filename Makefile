CC       = gcc
CFLAGS   = -Wall
SOURCES  = main.c src/archiver.c
INCLUDES = include/archiver.h
TARGET   = ./archiver

all: build

build: $(SOURCES) $(INCLUDES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(INCLUDES)

memcheck: build
	valgrind --tool=memcheck --leak-check=full $(TARGET)

clean:
	rm -f $(TARGET)
