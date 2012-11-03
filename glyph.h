#ifndef _GLYPH_H
#define _GLYPH_H

#include <stdint.h>
#include "list.h"
#include "space.h"

struct atom;
struct workspace;

enum glyph_op {
	BOND, UNBOND, SOURCE, TRANSMUTE
};

struct glyph {
	struct position pos;
	enum glyph_op op;
	uint8_t type;
	struct atom *a;
};

struct glyph *new_glyph(enum glyph_op op, uint8_t type, struct position pos);
void delete_glyph(struct glyph *g);
void act(struct workspace *w, struct glyph *g);

#endif
