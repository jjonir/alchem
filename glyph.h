#ifndef _GLYPH_H
#define _GLYPH_H

#include "list.h"
#include "space.h"
#include <stdint.h>

typedef enum {
	BOND, UNBOND, SOURCE, TRANSMUTE
} glyph_op_t;

typedef struct glyph {
	struct list_head list;
	position_t pos1;
	position_t pos2;
	glyph_op_t op;
	uint8_t type;
} glyph_t;

glyph_t *new_glyph(glyph_op_t op, uint8_t type, position_t p1, position_t p2);
glyph_t *glyph_at(struct list_head *glyphs, position_t pos);
void delete_glyph(glyph_t *g);
void act(glyph_t *g, struct list_head *atoms);

#endif
