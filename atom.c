#include "atom.h"
#include "workspace.h"
#include <stdlib.h>
#include <string.h>
#include "list.h"

const char *element_string_table[] = {
	"", "H", "He", "Li", "Be", "Al", "C", "N", "O", "F", "Ar" //...
};

void rotate_atom(struct atom *a, struct position pivot, enum orientation dir);
void move_atom(struct atom *a, struct position dp);
uint8_t in_compound(struct atom *find, struct list_head *l);
void build_compound(struct workspace *w, struct position pos, struct list_head *l);

struct atom *new_atom(enum element e, struct position pos)
{
	struct atom *a;

	a = (struct atom *)malloc(sizeof(struct atom));
	memset(a->bonds, 0, ORIENTATION_NUM);
	SET_POS(a->pos, pos.x, pos.y, pos.z);
	a->element = e;

	return a;
}

void add_bond(struct workspace *w, struct position pos1, enum orientation o)
{
	struct position pos2;
	struct atom *a1, *a2;

	POS_ADD(pos2, pos1, pos_shift(o, 1));

	a1 = atom_at(w, pos1);
	a2 = atom_at(w, pos2);
	if ((a1 != NULL) && (a2 != NULL)) {
		a1->bonds[o]++;
		a2->bonds[opposite(o)]++;
	}
//TODO check consistency, or maybe do that elsewhere, or maybe assume it works
}

void remove_bond(struct workspace *w, struct position pos1, enum orientation o)
{
	struct position pos2;
	struct atom *a1, *a2;

	POS_ADD(pos2, pos1, pos_shift(o, 1));

	a1 = atom_at(w, pos1);
	a2 = atom_at(w, pos2);
	if ((a1 != NULL) && (a2 != NULL)) {
		a1->bonds[o]--;
		a2->bonds[opposite(o)]--;
	}
//TODO don't reduce when 0, also maybe check consistency
}

void rotate_compound(struct workspace *w, struct position c, struct position pivot, enum orientation dir)
{
	atom_t *a;
	LIST_HEAD(to_rotate);

	build_compound(w, c, &to_rotate);
	list_for_each_entry(a, &to_rotate, compound) {
		remove_atom(w, a->pos);
		rotate_atom(a, pivot, dir);
	}
	list_for_each_entry(a, &to_rotate, compound) {
		add_atom(w, a, a->pos);
		//TODO check for something already there
	}
}

void move_compound(struct workspace *w, struct position c, struct position dp)
{
	atom_t *a;
	LIST_HEAD(to_move);

	build_compound(w, c, &to_move);
	list_for_each_entry(a, &to_move, compound) {
		remove_atom(w, a->pos);
		move_atom(a, dp);
	}
	list_for_each_entry(a, &to_move, compound) {
		add_atom(w, a, a->pos);
		//TODO check for something already there
	}
}

const char *element_string(enum element e)
{
	return element_string_table[e];
}

/* Local Functions */
void rotate_atom(struct atom *a, struct position pivot, enum orientation dir)
{
	struct position dp;
	POS_SUB(dp, a->pos, pivot);

	switch(dir) {
	//TODO other rotation directions
	//TODO update bond directions
	case PZ:
		a->pos.x = pivot.x - dp.y;
		a->pos.y = pivot.y + dp.x;
		break;
	case NZ:
		a->pos.x = pivot.x + dp.y;
		a->pos.y = pivot.y - dp.x;
		break;
	default:
		break;
	}
}

void move_atom(struct atom *a, struct position dp)
{
	POS_ADD(a->pos, a->pos, dp);
}

uint8_t in_compound(struct atom *find, struct list_head *l)
{
	struct atom *a;

	list_for_each_entry(a, l, compound) {
		if(a == find)
			return 1;
	}
	return 0;
}

/* NEVER try to use atom.compound without first calling this function */
/* atom.compound is not kept up-to-date in normal bonding activity */
void build_compound(struct workspace *w, struct position pos, struct list_head *l)
{
	struct atom *a;
	enum orientation dir;
	struct position adj;

	a = atom_at(w, pos);
	if ((a != NULL) && !in_compound(a, l)) {
		list_add(&a->compound, l);
		for (dir = 0; dir < ORIENTATION_NUM; dir++) {
			if (a->bonds[dir] > 0) {
				POS_ADD(adj, pos, pos_shift(dir, 1));
				build_compound(w, adj, l);
			}
		}
	}
}
