#include "workspace.h"
#include <stdlib.h>
#include <stdint.h>

void *remove_item(struct workspace *w, struct position pos, enum item_type t);

struct workspace *new_workspace(uint16_t width, uint16_t height, uint16_t depth)
{
	struct workspace *w;
	uint64_t size;

	w = (struct workspace *)malloc(sizeof(struct workspace));
	w->width = width;
	w->height = height;
	w->depth = depth;
	size = width * height * depth;
	w->items = (struct item *)calloc(size, sizeof(struct item));
	SET_POS(w->pos, 0, 0, 0);

	return w;
}

void add_atom(struct workspace *w, atom_t *a, struct position pos)
{
	struct item *i = &w->items[pos.x][pos.y][pos.z];
	if (i->type == 0) {
		i->type = ITEM_ATOM;
		i->item.atom = a;
	}
}

void add_manipulator(struct workspace *w, struct manipulator *m, struct position pos)
{
	struct item *i = &w->items[pos.x][pos.y][pos.z];
	if (i->type == 0) {
		i->type = ITEM_MANIPULATOR;
		i->item.manipulator = m;
	}
}

void add_glyph(struct workspace *w, struct glyph *g, struct position pos)
{
	struct item *i = &w->items[pos.x][pos.y][pos.z];
	if (i->type == 0) {
		i->type = ITEM_GLYPH;
		i->item.glyph = g;
	}
}

void *remove_item(struct workspace *w, struct position pos, enum item_type t)
{
	void *ret;
	struct item *i;

	i = &w->items[pos.x][pos.y][pos.z];
	if (i->type == t) {
		ret = (void *)i->item.atom;
		i->type = ITEM_NONE;
		i->item.atom = NULL;
	} else {
		ret = NULL;
	}

	return ret;
}

struct atom *remove_atom(struct workspace *w, struct position pos)
{
	struct atom *a;
	a = (struct atom *)remove_item(w, pos, ITEM_ATOM);
	return a;
}

struct manipulator *remove_manipulator(struct workspace *w, struct position pos)
{
	struct manipulator *m;
	m = (struct manipulator *)remove_item(w, pos, ITEM_MANIPULATOR);
	return m;
}

struct glyph *remove_glyph(struct workspace *w, struct position pos)
{
	struct atom *g;
	g = (struct glyph *)remove_item(w, pos, ITEM_GLYPH);
	return g;
}
