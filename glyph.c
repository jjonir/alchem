#include "glyph.h"
#include "atom.h"
#include <stdlib.h>

/* Local Function Prototypes */

/* External Interface Functions */
glyph_t *new_glyph(glyph_op_t op, uint8_t type, position_t p1, position_t p2) {
	glyph_t *g;

	g = (glyph_t *)malloc(sizeof(glyph_t));
	INIT_LIST_HEAD(&g->list);
	SET_POS(g->pos1, p1.x, p1.y, p1.z);
	SET_POS(g->pos2, p2.x, p2.y, p2.z);
	g->op = op;
	g->type = type;
	return g;
}

glyph_t *glyph_at(struct list_head *glyphs, position_t pos) {
	glyph_t *g;

	list_for_each_entry(g, glyphs, list) {
		if(POS_EQ(pos, g->pos1) || POS_EQ(pos, g->pos2))
			break;
	}
	if(g == list_entry(glyphs, glyph_t, list))
		g = NULL;

	return g;
}

void delete_glyph(glyph_t *g) {
	free(g);
}

void act(glyph_t *g, struct list_head *atoms) {
	atom_t *a1, *a2;

	a1 = atom_at(atoms, g->pos1);
	a2 = atom_at(atoms, g->pos2);
	switch(g->op) {
	case BOND:
		if(a1 && a2) {
			add_bond(a1, a2);
		}
		break;
	case UNBOND:
		if(a1 && a2) {
			remove_bond(a1, a2);
		}
		break;
	case SOURCE:
		if(a1 == NULL) {
			a1 = new_atom((element_t)g->type, g->pos1);
			list_add(&a1->list, atoms);
		}
		break;
	case TRANSMUTE:
		break;
	default:
		break;
	}
}
