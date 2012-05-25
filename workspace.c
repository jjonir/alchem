#include "workspace.h"
#include <stdlib.h>

workspace_t *new_workspace(void) {
	workspace_t *w;

	w = (workspace_t *)malloc(sizeof(workspace_t));
	INIT_LIST_HEAD(&w->atoms);
	INIT_LIST_HEAD(&w->manipulators);
	INIT_LIST_HEAD(&w->glyphs);
	SET_POS(w->pos, 0, 0, 0);

	return w;
}

void add_atom(workspace_t *w, atom_t *a) {
	list_add(&a->list, &w->atoms);
}

void add_manipulator(workspace_t *w, manipulator_t *m) {
	list_add(&m->list, &w->manipulators);
}

void remove_manipulator(manipulator_t *m) {
	list_del(&m->list);
	delete_manipulator(m);
}

void add_glyph(workspace_t *w, glyph_t *g) {
	list_add(&g->list, &w->glyphs);
}

void remove_glyph(glyph_t *g) {
	list_del(&g->list);
	delete_glyph(g);
}
