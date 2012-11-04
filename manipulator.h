#ifndef _MANIPULATOR_H
#define _MANIPULATOR_H

#include "space.h"
#include "list.h"

struct atom;
struct workspace;

#define MANIPULATOR_MIN_LENGTH 1
#define MANIPULATOR_MAX_LENGTH 5

enum head_state {
	HEAD_OPEN, HEAD_CLOSED
};

enum op {
	ROT_PX, ROT_PY, ROT_PZ, ROT_NX, ROT_NY, ROT_NZ,
	EXTEND, RETRACT, OPEN_HEAD, CLOSE_HEAD, NOP
};

struct inst {
	struct list_head list;
	enum op op;
};

struct manipulator {
	struct position pos;
	enum orientation orient;
	int length;
	enum head_state head_state;
	int grabbed;
	struct list_head inst_list;
	struct inst *pc;
};

struct manipulator *new_manipulator(enum orientation orient, int length, struct position pos);
void delete_manipulator(struct manipulator *m);
void inst_add(struct manipulator *m, enum op op);
void inst_add_prev(struct manipulator *m, enum op op);
void inst_del(struct manipulator *m);
const char *op_string(enum op op);
void step(struct workspace *w, struct manipulator *m);
void rotate_manipulator(struct workspace *w, struct manipulator *m, enum orientation dir);
void extend(struct workspace *w, struct manipulator *m);
void retract(struct workspace *w, struct manipulator *m);
void toggle_head(struct workspace *w, struct manipulator *m);
struct position get_head_pos(struct manipulator *m);

#endif
