#ifndef _MANIPULATOR_H
#define _MANIPULATOR_H

#include <stdint.h>
#include "atom.h"
#include "space.h"
#include "list.h"

#define MANIPULATOR_MIN_LENGTH 1
#define MANIPULATOR_MAX_LENGTH 5

typedef enum {
	HEAD_OPEN, HEAD_CLOSED
} head_state_t;

typedef enum {
	ROT_PX, ROT_PY, ROT_PZ, ROT_NX, ROT_NY, ROT_NZ,
	EXTEND, RETRACT, OPEN_HEAD, CLOSE_HEAD, NOP
} op_t;

typedef struct inst {
	struct list_head list;
	op_t op;
} inst_t;

typedef struct manipulator {
	struct list_head list;
	position_t pos;
	orientation_t orient;
	uint8_t length;
	head_state_t head_state;
	atom_t *grabbed_atom;
	struct list_head inst_list;
	inst_t *pc;
} manipulator_t;

manipulator_t *new_manipulator(orientation_t orient, uint8_t length, uint8_t x, uint8_t y, uint8_t z);
manipulator_t *manipulator_at(struct list_head *manipulators, position_t pos);
void delete_manipulator(manipulator_t *m);
void inst_add(manipulator_t *m, op_t op);
void inst_add_prev(manipulator_t *m, op_t op);
void inst_del(manipulator_t *m);
const char *op_string(op_t op);
void step(manipulator_t *m, struct list_head *atoms);
void rotate_ccw_z(manipulator_t *m);
void rotate_cw_z(manipulator_t *m);
void extend(manipulator_t *m);
void retract(manipulator_t *m);
void toggle_head(manipulator_t *m, struct list_head *atoms);
position_t get_head_pos(manipulator_t *m);

#endif
