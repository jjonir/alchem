#ifndef _ATOM_H
#define _ATOM_H

#include <stdint.h>
#include "space.h"
#include "list.h"

typedef enum {
	ATOM_H, ATOM_HE, ATOM_LI, ATOM_BE, ATOM_AL, ATOM_C, ATOM_N, ATOM_O, ATOM_F, ATOM_AR, //...
	ATOM_END
} element_t;

typedef struct atom {
	struct list_head list;
	struct list_head bond_list;
	struct list_head compound;
	position_t pos;
	element_t element;
} atom_t;

typedef struct bond {
	struct list_head list;
	atom_t *bonded_atom;
	uint8_t bonds;
} bond_t;

atom_t *new_atom(element_t element, position_t pos);
atom_t *atom_at(struct list_head *atoms, position_t pos);
void add_bond(atom_t *a1, atom_t *a2);
void remove_bond(atom_t *a1, atom_t *a2);
void rotate_compound(atom_t *c, position_t pivot, orientation_t dir);
void move_compound(atom_t *c, position_t dp);
const char *element_string(element_t element);

#endif
