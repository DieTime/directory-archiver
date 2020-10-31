CC       = gcc
CFLAGS   = -Wall -fsanitize=address
SOURCES  = main.c src/archiver.c
INCLUDES = include/archiver.h
TARGET   = ./archiver

VALGRIND = valgrind --tool=memcheck --leak-check=full
ARCHIVE_NAME = archive.tar
UNPACK_DIR = unpacked
PACK_DIR = .

all: build

build: $(SOURCES) $(INCLUDES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(INCLUDES)

memcheck-pack: build
	$(VALGRIND) $(TARGET) --pack $(PACK_DIR) $(ARCHIVE_NAME)

memcheck-unpack: build
	$(VALGRIND) $(TARGET) --unpack $(ARCHIVE_NAME) $(UNPACK_DIR)

memcheck: memcheck-pack memcheck-unpack

clean:
	rm -f $(TARGET)
	rm -f $(ARCHIVE_NAME)
	rm -rf $(UNPACK_DIR)
