CC = gcc
CFLAGS = -Wall
OBJECTS = alchem.o workspace.o atom.o manipulator.o glyph.o

.PHONY: all
all: alchem-curses alchem-gl

alchem-gl: $(OBJECTS) gl_io.o alchem.c
	$(CC) -o alchem-gl $(OBJECTS) gl_io.o $(CFLAGS) -lglut

alchem-curses: $(OBJECTS) curses_io.o alchem.c
	$(CC) -o alchem-curses $(OBJECTS) curses_io.o $(CFLAGS) -lncurses

.PHONY: clean
clean:
	rm -f alchem-curses alchem-gl $(OBJECTS) gl_io.o curses_io.o
