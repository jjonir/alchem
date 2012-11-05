#include "alchem.h"
#include "atom.h"
#include "glyph.h"
#include "manipulator.h"
#include "workspace.h"
#include <stdlib.h>

static struct item *item_at(struct workspace *w, struct position pos);
static int add_item(struct workspace *w, void *item, struct position pos, enum item_type t);
static void *remove_item(struct workspace *w, struct position pos, enum item_type t);

struct workspace *new_workspace(int width, int height, int depth)
{
	struct workspace *w;
	size_t size;

	w = (struct workspace *)malloc(sizeof(struct workspace));
	w->size.x = width;
	w->size.y = height;
	w->size.z = depth;
	size = (size_t)(width * height * depth);
	w->items = (struct item *)calloc(size, sizeof(struct item));
	w->pos = position(0, 0, 0);

	return w;
}

struct item *item_at(struct workspace *w, struct position pos)
{
	int index;

	index = pos.x + w->size.x * (pos.y + w->size.y * pos.z);
	return &w->items[index];
}

int add_item(struct workspace *w, void *item, struct position pos, enum item_type t)
{
	struct item *i =item_at(w, pos);
	if (i->type == ITEM_NONE) {
		i->type = t;
		i->item = item;
		return 0;
	} else if ((i->type == ITEM_GLYPH) && (t = ITEM_ATOM)) {
		if (((struct glyph *)i->item)->a == NULL) {
			((struct glyph *)i->item)->a = (struct atom *)item;
			return 0;
		} else {
			return -1;
		}
	} else {
		return -1;
	}
}

int add_atom(struct workspace *w, struct atom *a, struct position pos)
{
	return add_item(w, (void *)a, pos, ITEM_ATOM);
}

int add_manipulator(struct workspace *w, struct manipulator *m, struct position pos)
{
	return add_item(w, (void *)m, pos, ITEM_MANIPULATOR);
}

int add_glyph(struct workspace *w, struct glyph *g, struct position pos)
{
	return add_item(w, (void *)g, pos, ITEM_GLYPH);
}

void *remove_item(struct workspace *w, struct position pos, enum item_type t)
{
	struct item *i;
	void *ret;

	i = item_at(w, pos);
	if (i->type == t) {
		i->type = ITEM_NONE;
		ret = i->item;
		i->item = NULL;
	} else if ((i->type == ITEM_GLYPH) && (t == ITEM_ATOM)) {
		ret = ((struct glyph *)i->item)->a;
		((struct glyph *)i->item)->a = NULL;
	} else {
		ret = NULL;
	}

	return ret;
}

struct atom *remove_atom(struct workspace *w, struct position pos)
{
	return (struct atom *)remove_item(w, pos, ITEM_ATOM);
}

struct manipulator *remove_manipulator(struct workspace *w, struct position pos)
{
	return (struct manipulator *)remove_item(w, pos, ITEM_MANIPULATOR);
}

struct glyph *remove_glyph(struct workspace *w, struct position pos)
{
	return (struct glyph *)remove_item(w, pos, ITEM_GLYPH);
}

struct atom *atom_at(struct workspace *w, struct position pos)
{
	struct item *i = item_at(w, pos);
	if (i->type == ITEM_ATOM)
		return (struct atom *)i->item;
	else if (i->type == ITEM_GLYPH)
		return ((struct glyph *)i->item)->a;
	else
		return NULL;
}

struct manipulator *manipulator_at(struct workspace *w, struct position pos)
{
	struct item *i = item_at(w, pos);
	if (i->type == ITEM_MANIPULATOR)
		return (struct manipulator *)i->item;
	else
		return NULL;
}

struct glyph *glyph_at(struct workspace *w, struct position pos)
{
	struct item *i = item_at(w, pos);
	if (i->type == ITEM_GLYPH)
		return (struct glyph *)i->item;
	else
		return NULL;
}

int move_manipulator(struct workspace *w, struct position from, struct position to)
{
	struct manipulator *m = remove_manipulator(w, from);
	if (m != NULL) {
		if (add_manipulator(w, m, to) == 0) {
			m->pos = to;
			return 0;
		} else {
			add_manipulator(w, m, from); // target space occupied
			return -2;
		}
	} else {
		return -1; // nothing to move
	}
}
