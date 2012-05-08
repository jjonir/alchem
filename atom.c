#include "atom.h"
#include <stdlib.h>
#include <string.h>
#include "list.h"

const char *element_string_table[] = {
	"H", "He", "Li", "Be", "Al", "C", "N", "O", "F", "Ar" //...
};

/* Local Function Prototypes */
void rotate_atom(atom_t *a, position_t pivot, orientation_t dir);
void move_atom(atom_t *a, position_t dp);
uint8_t in_compound(atom_t *a, atom_t *c);
void build_compound(atom_t *a);

/* External Interface Functions */
atom_t *new_atom(element_t element, position_t pos) {
	atom_t *a;

	a = (atom_t *)malloc(sizeof(atom_t));
	INIT_LIST_HEAD(&a->list);
	INIT_LIST_HEAD(&a->bond_list);
	INIT_LIST_HEAD(&a->compound);
	SET_POS(a->pos, pos.x, pos.y, pos.z);
	a->element = element;

	return a;
}

atom_t *atom_at(struct list_head *atoms, position_t pos) {
	atom_t *a;

	list_for_each_entry(a, atoms, list) {
		if(POS_EQ(pos, a->pos))
			break;
	}
	if(a == list_entry(atoms, atom_t, list))
		a = NULL;

	return a;
}

void add_bond(atom_t *a1, atom_t *a2) {
	bond_t *b1, *b2;

	list_for_each_entry(b1, &a1->bond_list, list) {
		if(b1->bonded_atom == a2)
			break;
	}
	list_for_each_entry(b2, &a2->bond_list, list) {
		if(b2->bonded_atom == a1)
			break;
	}
	if((b1->bonded_atom == a2) && (b2->bonded_atom == a1)) {
		b1->bonds++;
		b2->bonds++;
	} else if((&b1->list == &a1->bond_list) && (&b2->list == &a2->bond_list)) {
		b1 = (bond_t *)malloc(sizeof(bond_t));
		b1->bonded_atom = a2;
		b2 = (bond_t *)malloc(sizeof(bond_t));
		b2->bonded_atom = a1;
		b1->bonds = b2->bonds = 1;

		list_add(&b1->list, &a1->bond_list);
		list_add(&b2->list, &a2->bond_list);
	} else {
		// one direction of the bond was found, the other was not...
		// this is an error state
	}
}

void remove_bond(atom_t *a1, atom_t *a2) {
	bond_t *b1, *b2;

	list_for_each_entry(b1, &a1->bond_list, list) {
		if(b1->bonded_atom == a2)
			break;
	}
	list_for_each_entry(b2, &a2->bond_list, list) {
		if(b2->bonded_atom == a1)
			break;
	}
	if((b1->bonded_atom == a2) && (b2->bonded_atom == a1)) {
		b1->bonds--;
		b2->bonds--;
		if((b1->bonds == 0) && (b2->bonds == 0)) {
			list_del(&b1->list);
			list_del(&b2->list);
			free(b1);
			free(b2);
		} else if(b1->bonds != b2->bonds) {
			// this is an error state
		}
	} else if((&b1->list == &a1->bond_list) && (&b2->list == &a2->bond_list)) {
		// no bonds, do nothing
	} else {
		// one direction of the bond was found, the other was not...
		// this is an error state
	}
}

void rotate_compound(atom_t *c, position_t pivot, orientation_t dir) {
	atom_t *a;

	build_compound(c);
	rotate_atom(c, pivot, dir);
	list_for_each_entry(a, &c->compound, compound) {
		rotate_atom(a, pivot, dir);
	}
}

void move_compound(atom_t *c, position_t dp) {
	atom_t *a;

	build_compound(c);
	move_atom(c, dp);
	list_for_each_entry(a, &c->compound, compound) {
		move_atom(a, dp);
	}
}

const char *element_string(element_t element) {
	return element_string_table[element];
}

/* Local Functions */
void rotate_atom(atom_t *a, position_t pivot, orientation_t dir) {
	int8_t dx=a->pos.x-pivot.x, dy=a->pos.y-pivot.y/*, dz=a->pos.z-pivot.z*/;

	switch(dir) {
	case PZ:
		a->pos.x = pivot.x - dy;
		a->pos.y = pivot.y + dx;
		break;
	case NZ:
		a->pos.x = pivot.x + dy;
		a->pos.y = pivot.y - dx;
		break;
	default:
		break;
	}
}

void move_atom(atom_t *a, position_t dp) {
	POS_ADD(a->pos, a->pos, dp);
}

uint8_t in_compound(atom_t *a, atom_t *c) {
	struct list_head *l;

	if(a == c)
		return 1;
	list_for_each(l, &c->compound) {
		if(&a->compound == l)
			return 1;
	}
	return 0;
}

void _build_compound(atom_t *a) {
	bond_t *b;

	list_for_each_entry(b, &a->bond_list, list) {
		if(!in_compound(b->bonded_atom, a)) {
			list_add(&b->bonded_atom->compound, &a->compound);
			_build_compound(b->bonded_atom);
		}
	}
}

/* NEVER try to use atom.compound without first calling this function */
/* atom.compound is not kept up-to-date in normal bonding activity */
void build_compound(atom_t *a) {
	INIT_LIST_HEAD(&a->compound);
	_build_compound(a);
}
