#include "atom.h"
#include "manipulator.h"
#include "workspace.h"
#include <stdint.h>
#include <stdlib.h>

const char *op_string_table[] = {
	"ROT_PX", "ROT_PY", "ROT_PZ", "ROT_NX", "ROT_NY", "ROT_NZ",
	"EXTEND", "RETRACT", "OPEN_HEAD", "CLOSE_HEAD", "NOP"
};

struct manipulator *new_manipulator(enum orientation orient, uint8_t length, struct position pos) {
	struct manipulator *m;

	m = (struct manipulator *)malloc(sizeof(struct manipulator));
	SET_POS_V(m->pos, pos);
	m->orient = orient;
	m->length = length;
	m->head_state = HEAD_OPEN;
	m->grabbed_atom = NULL;
	INIT_LIST_HEAD(&m->inst_list);
	m->pc = NULL;
	return m;
}

void delete_manipulator(struct manipulator *m) {
	struct inst *i, *j;
	list_for_each_entry_safe(i, j, &m->inst_list, list) {
		list_del(&i->list);
		free(i);
	}
	free(m);
}

void inst_add(struct manipulator *m, enum op op) {
	struct inst *i = (struct inst *)malloc(sizeof(struct inst));
	i->op = op;
	if(list_empty(&m->inst_list)) {
		list_add(&i->list, &m->inst_list);
	} else {
		list_add(&i->list, &m->pc->list);
	}
	m->pc = i;
}

void inst_add_prev(struct manipulator *m, enum op op) {
	struct inst *i = (struct inst *)malloc(sizeof(struct inst));
	i->op = op;
	if(list_empty(&m->inst_list)) {
		list_add(&i->list, &m->inst_list);
		m->pc = i;
	} else {
		list_add_tail(&i->list, &m->pc->list);
	}
}

void inst_del(struct manipulator *m) {
	struct inst *i = m->pc;
	if(!list_empty(&m->inst_list)) {
		if(m->pc->list.next == &m->inst_list)
			m->pc = list_entry(m->pc->list.prev, inst_t, list);
		else
			m->pc = list_entry(m->pc->list.next, inst_t, list);
		list_del(&i->list);
		if(list_empty(&m->inst_list)) {
			m->pc = NULL;
		}
		free(i);
	}
}

const char *op_string(enum op op) {
	return op_string_table[op];
}

void step(struct workspace *w, struct manipulator *m) {
	if(!list_empty(&m->inst_list)) {
		switch(m->pc->op) {
		case ROT_PX:
			rotate_manipulator(w, m, PX);
			break;
		case ROT_NX:
			rotate_manipulator(w, m, NX);
			break;
		case ROT_PY:
			rotate_manipulator(w, m, PY);
			break;
		case ROT_NY:
			rotate_manipulator(w, m, NY);
			break;
		case ROT_PZ:
			rotate_manipulator(w, m, PZ);
			break;
		case ROT_NZ:
			rotate_manipulator(w, m, NZ);
			break;
		case EXTEND:
			extend(w, m);
			break;
		case RETRACT:
			retract(w, m);
			break;
		case OPEN_HEAD:
			if(m->head_state == HEAD_CLOSED)
				toggle_head(w, m);
			break;
		case CLOSE_HEAD:
			if(m->head_state == HEAD_OPEN)
				toggle_head(w, m);
			break;
		case NOP:
			break;
		default:
			break;
		}
		if(m->pc->list.next == &m->inst_list)
			m->pc = list_entry(m->inst_list.next, struct inst, list);
		else
			m->pc = list_entry(m->pc->list.next, struct inst, list);
	}
}

void rotate_manipulator(struct workspace *w, struct manipulator *m, enum orientation dir) {
	m->orient = rotated(m->orient, dir);
	if(m->grabbed_atom && (m->head_state == HEAD_CLOSED))
		rotate_compound(w, get_head_pos(m), m->pos, NZ);
}

void extend(struct workspace *w, struct manipulator *m) {
	struct position old = get_head_pos(m);
	struct position dp;
	if(m->length < MANIPULATOR_MAX_LENGTH) {
		m->length++;
	}
	POS_SUB(dp, get_head_pos(m), old);
	if(m->grabbed_atom && (m->head_state == HEAD_CLOSED))
		move_compound(w, get_head_pos(m), dp);
}

void retract(struct workspace *w, struct manipulator *m) {
	struct position old = get_head_pos(m);
	struct position dp;
	if(m->length > MANIPULATOR_MIN_LENGTH) {
		m->length--;
	}
	POS_SUB(dp, get_head_pos(m), old);
	if(m->grabbed_atom && (m->head_state == HEAD_CLOSED))
		move_compound(w, get_head_pos(m), dp);
}

// TODO prevent two heads grabbing one atom
// or, TODO prevent two heads moving a compound in different directions
// TODO either of these is non-trivial
void toggle_head(struct workspace *w, struct manipulator *m) {
	if(m->head_state == HEAD_OPEN) {
		m->head_state = HEAD_CLOSED;
		m->grabbed_atom = atom_at(w, get_head_pos(m));
	} else if(m->head_state == HEAD_CLOSED) {
		m->head_state = HEAD_OPEN;
		m->grabbed_atom = NULL;
	}
}

struct position get_head_pos(struct manipulator *m) {
	struct position ret;

	SET_POS_V(ret, m->pos);
	switch(m->orient) {
	case PX:
		ret.x += m->length;
		break;
	case NX:
		ret.x -= m->length;
		break;
	case PY:
		ret.y += m->length;
		break;
	case NY:
		ret.y -= m->length;
		break;
	case PZ:
		ret.z += m->length;
		break;
	case NZ:
		ret.z -= m->length;
		break;
	default:
		break;
	}
	return ret;
}
