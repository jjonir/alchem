#ifndef _WORKSPACE_H
#define _WORKSPACE_H

#include <stdint.h>
#include "space.h"
#include "atom.h"
#include "manipulator.h"
#include "glyph.h"

typedef struct workspace {
	uint16_t width;
	uint16_t height;
	uint16_t depth;
	struct item *items;
	position_t pos;
} workspace_t;

enum item_type {
	ITEM_NONE = 0,
	ITEM_ATOM,
	ITEM_MANIPULATOR,
	ITEM_GLYPH
};

struct item {
	enum item_type type;
	union {
		struct atom *atom;
		struct manipulator *manipulator;
		struct glyph *glyph;
	} item;
};

struct workspace *new_workspace(uint16_t width, uint16_t height, uint16_t depth);
void add_atom(struct workspace *w, struct atom *a, struct position pos);
void add_manipulator(struct workspace *w, struct manipulator *m, struct position pos);
void add_glyph(struct workspace *w, struct glyph *g, struct position pos);
struct atom *remove_atom(struct workspace *w, struct position pos);
struct manipulator *remove_manipulator(struct workspace *w, struct position pos);
struct glyph *remove_glyph(struct workspace *w, struct position pos);

#endif
