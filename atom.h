#ifndef _ATOM_H
#define _ATOM_H

#include "space.h"
#include "list.h"

struct workspace;

enum element {
	ATOM_NONE, ATOM_H, ATOM_HE, ATOM_LI, ATOM_BE, ATOM_AL, ATOM_C, ATOM_N, ATOM_O, ATOM_F, ATOM_AR, //...
	ATOM_END
};

struct atom {
	struct list_head compound;
	int bonds[ORIENTATION_NUM];
	struct position pos;
	enum element element;
};

struct atom *new_atom(enum element e, struct position pos);
void add_bond(struct workspace *w, struct position pos1, enum orientation o);
void remove_bond(struct workspace *w, struct position pos1, enum orientation o);
void rotate_compound(struct workspace *w, struct position c, struct position pivot, enum orientation dir);
void move_compound(struct workspace *w, struct position c, struct position dp);
const char *element_string(enum element e);

#endif
