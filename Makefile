CC = gcc
WARNINGS = -Wall -Wextra -Wsign-conversion
ifeq ($(LOG), true)
CFLAGS = $(WARNINGS) -DENABLE_LOG
else
CFLAGS = $(WARNINGS)
endif
RM = rm -f

ENV ?= linux

ifeq ($(ENV), mingw)
CURSES = -lpdcurses
GLUT = -lfreeglut
GLU = -lglu32
GL = -lopengl32
EXE_SUFFIX = .exe
endif
ifeq ($(ENV), linux)
CURSES = -lncurses
GLUT = -lglut
GLU = -lGLU
GL = -lGL
endif

OBJECTS = alchem.o workspace.o atom.o manipulator.o glyph.o
CURSES_OBJECTS = $(OBJECTS) curses_io.o
GLUT_OBJECTS = $(OBJECTS) glut_io.o
GL_OBJECTS = $(OBJECTS) gl_io.o
ALL_OBJECTS = $(OBJECTS) curses_io.o glut_io.o gl_io.o

CURSES_EXE = alchem-curses$(EXE_SUFFIX)
GLUT_EXE = alchem-glut$(EXE_SUFFIX)
GL_EXE = alchem-gl$(EXE_SUFFIX)
EXES = $(CURSES_EXE) $(GLUT_EXE) $(GL_EXE)

.PHONY: all clean

all: $(EXES)

$(CURSES_EXE): $(CURSES_OBJECTS)
	@echo LD $@
	$(CC) $(CFLAGS) -o $@ $(CURSES_OBJECTS) $(CURSES)

$(GLUT_EXE): $(GLUT_OBJECTS)
	@echo LD $@
	$(CC) $(CFLAGS) -o $@ $(GLUT_OBJECTS) $(GLUT) $(GLU) $(GL)

$(GL_EXE): $(GL_OBJECTS)
	@echo LD $@
	$(CC) $(CFLAGS) -o $@ $(GL_OBJECTS) $(GL)

%.o: %.c
	@echo CC $@
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(EXES) $(ALL_OBJECTS)
