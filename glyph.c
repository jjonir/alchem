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
	g->a = NULL;

	return g;
}

void delete_glyph(struct glyph *g)
{
	free(g);
}

void act(struct workspace *w, struct glyph *g)
{
	switch(g->op) {
	case BOND:
		add_bond(w, g->pos, (enum orientation)g->type);
		break;
	case UNBOND:
		remove_bond(w, g->pos, (enum orientation)g->type);
		break;
	case SOURCE:
		if (g->a == NULL)
			g->a = new_atom((enum element)g->type, g->pos);
		break;
	case TRANSMUTE:
		break;
	default:
		break;
	}
}
