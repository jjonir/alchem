#include "workspace.h"
#include "atom.h"
#include "glyph.h"
#include "manipulator.h"
#include <stdlib.h>
#include <stdint.h>

static struct item *item_at(struct workspace *w, struct position pos);
static void add_item(struct workspace *w, void *item, struct position pos, enum item_type t);
static void *remove_item(struct workspace *w, struct position pos, enum item_type t);

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

struct item *item_at(struct workspace *w, struct position pos)
{
	uint64_t index;

	index = pos.x + w->width * (pos.y + w->height * pos.z);
	return &w->items[index];
}

void add_item(struct workspace *w, void *item, struct position pos, enum item_type t)
{
	struct item *i =item_at(w, pos);
	if (i->type == 0) {
		i->type = t;
		i->item = item;
	}
}

void add_atom(struct workspace *w, atom_t *a, struct position pos)
{
	add_item(w, (void *)a, pos, ITEM_ATOM);
}

void add_manipulator(struct workspace *w, struct manipulator *m, struct position pos)
{
	add_item(w, (void *)m, pos, ITEM_MANIPULATOR);
}

void add_glyph(struct workspace *w, struct glyph *g, struct position pos)
{
	add_item(w, (void *)g, pos, ITEM_GLYPH);
}

void *remove_item(struct workspace *w, struct position pos, enum item_type t)
{
	void *ret;
	struct item *i;

	i = item_at(w, pos);
	if (i->type == t) {
		ret = (void *)i->item;
		i->type = ITEM_NONE;
		i->item = NULL;
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
	struct glyph *g;
	g = (struct glyph *)remove_item(w, pos, ITEM_GLYPH);
	return g;
}

struct atom *atom_at(struct workspace *w, struct position pos)
{
	struct atom *a;
	struct item *i = item_at(w, pos);
	if (i->type == ITEM_ATOM)
		a = (struct atom *)i->item;
	else
		a = NULL;
	return a;
}

struct manipulator *manipulator_at(struct workspace *w, struct position pos)
{
	struct manipulator *m;
	struct item *i = item_at(w, pos);
	if (i->type == ITEM_MANIPULATOR)
		m = (struct manipulator *)i->item;
	else
		m = NULL;
	return m;
}

struct glyph *glyph_at(struct workspace *w, struct position pos)
{
	struct glyph *g;
	struct item *i = item_at(w, pos);
	if (i->type == ITEM_GLYPH)
		g = (struct glyph *)i->item;
	else
		g = NULL;
	return g;
}
