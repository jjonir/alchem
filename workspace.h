#ifndef _WORKSPACE_H
#define _WORKSPACE_H

#include "space.h"

struct atom;
struct manipulator;
struct glyph;

struct workspace {
	struct position size;
	struct item *items;
	struct position pos;
};

enum item_type {
	ITEM_NONE = 0,
	ITEM_ATOM,
	ITEM_MANIPULATOR,
	ITEM_GLYPH
};

struct item {
	enum item_type type;
	void *item;
#if 0
	union {
		struct atom *atom;
		struct manipulator *manipulator;
		struct glyph *glyph;
	} item;
#endif
};

struct workspace *new_workspace(int width, int height, int depth);
int add_atom(struct workspace *w, struct atom *a, struct position pos);
int add_manipulator(struct workspace *w, struct manipulator *m, struct position pos);
int add_glyph(struct workspace *w, struct glyph *g, struct position pos);
struct atom *remove_atom(struct workspace *w, struct position pos);
struct manipulator *remove_manipulator(struct workspace *w, struct position pos);
struct glyph *remove_glyph(struct workspace *w, struct position pos);
struct atom *atom_at(struct workspace *w, struct position pos);
struct manipulator *manipulator_at(struct workspace *w, struct position pos);
struct glyph *glyph_at(struct workspace *w, struct position pos);
int move_manipulator(struct workspace *w, struct position from, struct position to);

#endif
