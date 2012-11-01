#ifndef _ATOM_H
#define _ATOM_H

#include <stdint.h>
#include "space.h"
#include "list.h"

struct workspace;

typedef enum element {
	ATOM_H, ATOM_HE, ATOM_LI, ATOM_BE, ATOM_AL, ATOM_C, ATOM_N, ATOM_O, ATOM_F, ATOM_AR, //...
	ATOM_END
} element_t;

typedef struct atom {
	struct list_head compound;
	uint8_t bonds[ORIENTATION_NUM];
	position_t pos;
	element_t element;
} atom_t;

struct atom *new_atom(enum element e, struct position pos);
void add_bond(struct workspace *w, struct position pos1, enum orientation o);
void remove_bond(struct workspace *w, struct position pos1, enum orientation o);
void rotate_compound(struct workspace *w, struct position c, struct position pivot, enum orientation dir);
void move_compound(struct workspace *w, struct position c, struct position dp);
const char *element_string(enum element e);

#endif
