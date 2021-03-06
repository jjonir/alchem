#include "alchem.h"
#include "atom.h"
#include "glyph.h"
#include "workspace.h"
#include <stdlib.h>

const char *glyph_string_table[] = {
	"BOND", "UNBOND", "SOURCE", "TRANSMUTE"
};

struct glyph *new_glyph(enum glyph_op op, int type, struct position pos)
{
	struct glyph *g;

	g = (struct glyph *)malloc(sizeof(struct glyph));
	g->pos = pos;
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
#ifdef ENABLE_LOG
	logf("acting with glyph %li\n", (long)g);
	log_indent();
	logf("operation %s\n", glyph_string_table[g->op]);
#endif
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
#ifdef ENABLE_LOG
	log_unindent();
#endif
}
