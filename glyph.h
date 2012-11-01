#ifndef _GLYPH_H
#define _GLYPH_H

#include <stdint.h>
#include "list.h"
#include "space.h"

struct workspace;

typedef enum glyph_op {
	BOND, UNBOND, SOURCE, TRANSMUTE
} glyph_op_t;

typedef struct glyph {
	struct position pos; // TODO might not need this
	enum glyph_op op;
	uint8_t type;
} glyph_t;

struct glyph *new_glyph(enum glyph_op op, uint8_t type, struct position pos);
void delete_glyph(struct glyph *g);
void act(struct workspace *w, struct glyph *g);

#endif
