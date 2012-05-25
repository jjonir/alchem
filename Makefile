CC = gcc
CFLAGS = -Wall -Wextra
OBJECTS = alchem.o workspace.o atom.o manipulator.o glyph.o

.PHONY: all
all: alchem-curses alchem-glut alchem-gl

alchem-curses: $(OBJECTS) curses_io.o
	$(CC) -o alchem-curses $(OBJECTS) curses_io.o $(CFLAGS) -lncurses

alchem-glut: $(OBJECTS) glut_io.o
	$(CC) -o alchem-glut $(OBJECTS) glut_io.o $(CFLAGS) -lglut -lGL -lGLU

alchem-gl: $(OBJECTS) gl_io.o
	$(CC) -o alchem-gl $(OBJECTS) gl_io.o $(CFLAGS) -lGL

.PHONY: clean
clean:
	rm -f alchem-curses alchem-glut alchem-gl $(OBJECTS) curses_io.o glut_io.o gl_io.o
