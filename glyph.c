#include "atom.h"
#include "glyph.h"
#include "workspace.h"
#include <stdlib.h>

struct glyph *new_glyph(enum glyph_op op, uint8_t type, struct position pos)
{
	struct glyph *g;

	g = (struct glyph *)malloc(sizeof(struct glyph));
	SET_POS(g->pos, pos.x, pos.y, pos.z);
	g->op = op;
	g->type = type;

	return g;
}

void delete_glyph(struct glyph *g)
{
	free(g);
}

void act(struct workspace *w, struct glyph *g)
{
	struct atom *a;

	switch(g->op) {
	case BOND:
		add_bond(w, g->pos, (enum orientation)g->type);
		break;
	case UNBOND:
		remove_bond(w, g->pos, (enum orientation)g->type);
		break;
	case SOURCE:
//TODO move most of this into atom.c; workspace ignores add_* if there's something there already
		if (atom_at(w, g->pos) == NULL) {
			a = new_atom((enum element)g->type, g->pos);
			add_atom(w, a, g->pos);
		}
		break;
	case TRANSMUTE:
		break;
	default:
		break;
	}
}
