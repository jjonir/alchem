#ifndef _WORKSPACE_H
#define _WORKSPACE_H

#include <stdint.h>
#include "space.h"
#include "atom.h"
#include "manipulator.h"
#include "glyph.h"
#include "list.h"

#define GRID_SZ_X 10
#define GRID_SZ_Y 10
#define GRID_SZ_Z 10

typedef struct workspace {
	struct list_head atoms;
	struct list_head manipulators;
	struct list_head glyphs;
	position_t pos;
} workspace_t;

workspace_t *new_workspace(void);
void add_atom(workspace_t *w, atom_t *a);
void add_manipulator(workspace_t *w, manipulator_t *m);
void remove_manipulator(manipulator_t *m);
void add_glyph(workspace_t *w, glyph_t *g);
void remove_glyph(glyph_t *g);

#endif
