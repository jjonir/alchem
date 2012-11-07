#include "alchem.h"
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
int in_compound(struct atom *find, struct list_head *l);
void build_compound(struct workspace *w, struct position pos, struct list_head *l);

struct atom *new_atom(enum element e, struct position pos)
{
	struct atom *a;

	a = (struct atom *)malloc(sizeof(struct atom));
	memset(a->bonds, 0, ORIENTATION_NUM);
	a->pos = pos;
	a->element = e;

	return a;
}

void add_bond(struct workspace *w, struct position pos1, enum orientation o)
{
	struct position pos2;
	struct atom *a1, *a2;

	pos2 = pos_add(pos1, pos_shift(o, 1));

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

	pos2 = pos_add(pos1, pos_shift(o, 1));

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
	struct atom *a;
	LIST_HEAD(to_rotate);

#ifdef ENABLE_LOG
	logf("rotating compound from %i,%i,%i about %i,%i,%i in direction %i\n", c.x,c.y,c.z, pivot.x,pivot.y,pivot.z, dir);
	log_indent();
#endif
	build_compound(w, c, &to_rotate);
	list_for_each_entry(a, &to_rotate, compound) {
		remove_atom(w, a->pos);
		rotate_atom(a, pivot, dir);
	}
	list_for_each_entry(a, &to_rotate, compound) {
		add_atom(w, a, a->pos);
		//TODO check for something already there
	}
#ifdef ENABLE_LOG
	log_unindent();
#endif
}

void move_compound(struct workspace *w, struct position c, struct position dp)
{
	struct atom *a;
	LIST_HEAD(to_move);

#ifdef ENABLE_LOG
	logf("moving compound from %i,%i,%i by %i,%i,%i\n", c.x,c.y,c.z, dp.x,dp.y,dp.z);
	log_indent();
#endif
	build_compound(w, c, &to_move);
	list_for_each_entry(a, &to_move, compound) {
		remove_atom(w, a->pos);
		move_atom(a, dp);
	}
	list_for_each_entry(a, &to_move, compound) {
		add_atom(w, a, a->pos);
		//TODO check for something already there
	}
#ifdef ENABLE_LOG
	log_unindent();
#endif
}

const char *element_string(enum element e)
{
	return element_string_table[e];
}

/* Local Functions */
void rotate_atom(struct atom *a, struct position pivot, enum orientation dir)
{
	struct position dp;
	dp = pos_sub(a->pos, pivot);

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
	a->pos = pos_add(a->pos, dp);
}

int in_compound(struct atom *find, struct list_head *l)
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

#ifdef ENABLE_LOG
	log("build compound\n");
	log_indent();
#endif
	a = atom_at(w, pos);
	if ((a != NULL) && !in_compound(a, l)) {
#ifdef ENABLE_LOG
		logf("adding atom %li\n", (long)a);
#endif
		list_add(&a->compound, l);
		for (dir = 0; dir < ORIENTATION_NUM; dir++) {
			if (a->bonds[dir] > 0) {
				adj = pos_add(pos, pos_shift(dir, 1));
				build_compound(w, adj, l);
			}
		}
	}
#ifdef ENABLE_LOG
	log_unindent();
#endif
}
