#include "manipulator.h"
#include "atom.h"
#include <stdint.h>
#include <stdlib.h>

const char *op_string_table[] = {
	"ROT_PX", "ROT_PY", "ROT_PZ", "ROT_NX", "ROT_NY", "ROT_NZ",
	"EXTEND", "RETRACT", "OPEN_HEAD", "CLOSE_HEAD", "NOP"
};

/* Local Function Prototypes */

/* External Interface Functions */
manipulator_t *new_manipulator(orientation_t orient, uint8_t length, uint8_t x, uint8_t y, uint8_t z) {
	manipulator_t *m;

	m = (manipulator_t *)malloc(sizeof(manipulator_t));
	INIT_LIST_HEAD(&m->list);
	SET_POS(m->pos, x, y, z);
	m->orient = orient;
	m->length = length;
	m->head_state = HEAD_OPEN;
	m->grabbed_atom = NULL;
	INIT_LIST_HEAD(&m->inst_list);
	m->pc = NULL;
	return m;
}

manipulator_t *manipulator_at(struct list_head *manipulators, position_t pos) {
	manipulator_t *m;

	list_for_each_entry(m, manipulators, list) {
		if(POS_EQ(pos, m->pos))
			break;
	}
	if(m == list_entry(manipulators, manipulator_t, list))
		m = NULL;

	return m;
}

void delete_manipulator(manipulator_t *m) {
	inst_t *i, *j;
	list_for_each_entry_safe(i, j, &m->inst_list, list) {
		list_del(&i->list);
		free(i);
	}
	free(m);
}

void inst_add(manipulator_t *m, op_t op) {
	inst_t *i = (inst_t *)malloc(sizeof(inst_t));
	i->op = op;
	if(list_empty(&m->inst_list)) {
		list_add(&i->list, &m->inst_list);
		m->pc = i;
	} else {
		list_add(&i->list, &m->pc->list);
	}
}

void inst_add_prev(manipulator_t *m, op_t op) {
	inst_t *i = (inst_t *)malloc(sizeof(inst_t));
	i->op = op;
	if(list_empty(&m->inst_list)) {
		list_add(&i->list, &m->inst_list);
		m->pc = i;
	} else {
		list_add_tail(&i->list, &m->pc->list);
	}
}

void inst_del(manipulator_t *m) {
	inst_t *i = m->pc;
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

const char *op_string(op_t op) {
	return op_string_table[op];
}

void step(manipulator_t *m, struct list_head *atoms) {
	if(!list_empty(&m->inst_list)) {
		switch(m->pc->op) {
		case ROT_PX:
		case ROT_NX:
		case ROT_PY:
		case ROT_NY:
			break;
		case ROT_PZ:
			rotate_cw_z(m);
			break;
		case ROT_NZ:
			rotate_ccw_z(m);
			break;
		case EXTEND:
			extend(m);
			break;
		case RETRACT:
			retract(m);
			break;
		case OPEN_HEAD:
			if(m->head_state == HEAD_CLOSED)
				toggle_head(m, atoms);
			break;
		case CLOSE_HEAD:
			if(m->head_state == HEAD_OPEN)
				toggle_head(m, atoms);
			break;
		case NOP:
			break;
		default:
			break;
		}
		if(m->pc->list.next == &m->inst_list)
			m->pc = list_entry(m->inst_list.next, inst_t, list);
		else
			m->pc = list_entry(m->pc->list.next, inst_t, list);
	}
}

void rotate_ccw_z(manipulator_t *m) {
	switch(m->orient) {
	case NY:
		m->orient = NX;
		break;
	case NX:
		m->orient = PY;
		break;
	case PY:
		m->orient = PX;
		break;
	case PX:
		m->orient = NY;
		break;
	default:
		break;
	}
	if(m->grabbed_atom && (m->head_state == HEAD_CLOSED))
		rotate_compound(m->grabbed_atom, m->pos, NZ);
}

void rotate_cw_z(manipulator_t *m) {
	switch(m->orient) {
	case NY:
		m->orient = PX;
		break;
	case PX:
		m->orient = PY;
		break;
	case PY:
		m->orient = NX;
		break;
	case NX:
		m->orient = NY;
		break;
	default:
		break;
	}
	if(m->grabbed_atom && (m->head_state == HEAD_CLOSED))
		rotate_compound(m->grabbed_atom, m->pos, PZ);
}

void extend(manipulator_t *m) {
	position_t old = get_head_pos(m);
	position_t dp;
	if(m->length < MANIPULATOR_MAX_LENGTH) {
		m->length++;
	}
	POS_SUB(dp, get_head_pos(m), old);
	if(m->grabbed_atom && (m->head_state == HEAD_CLOSED))
		move_compound(m->grabbed_atom, dp);
}

void retract(manipulator_t *m) {
	position_t old = get_head_pos(m);
	position_t dp;
	if(m->length > MANIPULATOR_MIN_LENGTH) {
		m->length--;
	}
	POS_SUB(dp, get_head_pos(m), old);
	if(m->grabbed_atom && (m->head_state == HEAD_CLOSED))
		move_compound(m->grabbed_atom, dp);
}

//TODO prevent two heads grabbing one atom
void toggle_head(manipulator_t *m, struct list_head *atoms) {
	if(m->head_state == HEAD_OPEN) {
		m->head_state = HEAD_CLOSED;
		m->grabbed_atom = atom_at(atoms, get_head_pos(m));
	} else if(m->head_state == HEAD_CLOSED) {
		m->head_state = HEAD_OPEN;
		m->grabbed_atom = NULL;
	}
}

position_t get_head_pos(manipulator_t *m) {
	position_t ret = {m->pos.x, m->pos.y, m->pos.z};
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

/* Local Functions */
