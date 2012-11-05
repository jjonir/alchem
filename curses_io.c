#include <curses.h>
#include <time.h>
#include "io.h"
#include "atom.h"
#include "glyph.h"
#include "manipulator.h"
#include "workspace.h"

int foo = 0;

/* Local Function Prototypes */
void display(struct workspace *w);
int process_input_blocking(struct workspace *w);
void run(struct workspace *w);
enum op pick_op(int x);
enum element pick_element(int x);
void draw_bond(struct position p1, struct position p2);
void draw_atom(struct atom *a);
void draw_manipulator(struct manipulator *m);
void print_inst_list(struct manipulator *m, int x);
void draw_glyph(struct glyph *g);

/* External Interface Functions */
void init_io(void)
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
}

void end_io(void)
{
	endwin();
}

void edit_workspace_loop(struct workspace *w)
{
	int cmd;
	do {
		display(w);
		cmd = process_input_blocking(w);
	} while (cmd != -1);
}

/* Local Functions */
void display(struct workspace *w)
{
	struct position pos;
	struct atom *a;
	struct manipulator *m;
	struct glyph *g;

	clear();

	for_each_position(pos, w->size.x, w->size.y, 1) {
		if (POS_EQ(pos, w->pos))
			attron(A_STANDOUT);

		if ((a = atom_at(w, pos)) != NULL) {
			draw_atom(a);
		} else if ((m = manipulator_at(w, pos)) != NULL) {
			draw_manipulator(m);
		} else if ((g = glyph_at(w, pos)) != NULL) {
			draw_glyph(g);
		} else {
			mvaddch(3*pos.y+1, 4*pos.x+1, '.');
		}

		if (POS_EQ(pos, w->pos))
			attroff(A_STANDOUT);
	}

	/* Redraw the selected manipulator to be sure it's on top */
	if ((m = manipulator_at(w, w->pos)) != NULL) {
		print_inst_list(m, 4 * w->size.x + 1);
		attron(A_STANDOUT);
		draw_manipulator(m);
		attroff(A_STANDOUT);
	}

	refresh();
}

// TODO document input commands; both in README.md and in a case 'h' or case '?'
int process_input_blocking(struct workspace *w)
{
	struct manipulator *m;
	struct glyph *g;
	struct position pos;
	int c = getch();

	m = manipulator_at(w, w->pos);
	g = glyph_at(w, w->pos);

	switch (c) {
	/* Grid Navigation */
	case KEY_LEFT:
		if (w->pos.x > 0)
			w->pos.x--;
		break;
	case KEY_RIGHT:
		if (w->pos.x < w->size.x-1)
			w->pos.x++;
		break;
	case KEY_UP:
		if (w->pos.y > 0)
			w->pos.y--;
		break;
	case KEY_DOWN:
		if (w->pos.y < w->size.y-1)
			w->pos.y++;
		break;
	/* Thing Placement and Removal */
	case 'm':
		m = new_manipulator(NY, 2, w->pos);
		add_manipulator(w, m, w->pos);
		break;
	case 'M':
		if ((m = remove_manipulator(w, w->pos)) != NULL)
			delete_manipulator(m);
		break;
	case 'b':
//TODO
		if ((g == NULL) && (m == NULL)) {
			g = new_glyph(BOND, (int)PX, w->pos);
			add_glyph(w, g, w->pos);
		}
		break;
	case 'B':
//TODO only remove bond?
		if ((g = remove_glyph(w, w->pos)) != NULL)
			delete_glyph(g);
		break;
	case 'u':
//TODO
		if ((g == NULL) && (m == NULL)) {
			g = new_glyph(UNBOND, (int)PX, w->pos);
			add_glyph(w, g, w->pos);
		}
		break;
	case 'U':
//TODO only remove unbond?
		if ((g = remove_glyph(w, w->pos)) != NULL)
			delete_glyph(g);
		break;
	case 'j': //j for jar? or something
//TODO
		if ((g == NULL) && (m == NULL)) {
			g = new_glyph(SOURCE, (int)pick_element(4 * w->size.x + 1), w->pos);
			add_glyph(w, g, w->pos);
		}
		break;
	/* Thing Movement */
	case 'a':
		if (move_manipulator(w, w->pos, pos_add(w->pos, position(-1, 0, 0))) == 0)
			w->pos.x--;
		break;
	case 'd':
		if (move_manipulator(w, w->pos, pos_add(w->pos, position(1, 0, 0))) == 0)
			w->pos.x++;
		break;
	case 'w':
		if (move_manipulator(w, w->pos, pos_add(w->pos, position(0, -1, 0))) == 0)
			w->pos.y--;
		break;
	case 's':
		if (move_manipulator(w, w->pos, pos_add(w->pos, position(0, 1, 0))) == 0)
			w->pos.y++;
		break;
	/* Manipulator Manipulation */
	case '[':
		if ((m = manipulator_at(w, w->pos)) != NULL)
			rotate_manipulator(w, m, NZ);
		break;
	case ']':
		if ((m = manipulator_at(w, w->pos)) != NULL)
			rotate_manipulator(w, m, PZ);
		break;
	case '=':
	case '+':
		if ((m = manipulator_at(w, w->pos)) != NULL)
			extend(w, m);
		break;
	case '-':
		if ((m = manipulator_at(w, w->pos)) != NULL)
			retract(w, m);
		break;
	case '\\':
	case '|':
		if ((m = manipulator_at(w, w->pos)) != NULL)
			toggle_head(w, m);
		break;
	/* Instruction Manipulation */
	case 'i':
		if ((m = manipulator_at(w, w->pos)) != NULL) {
			inst_add(m, pick_op(4 * w->size.x + 10));
		}
		break;
	case 'I':
		if ((m = manipulator_at(w, w->pos)) != NULL) {
			inst_add_prev(m, pick_op(4 * w->size.x + 10));
		}
		break;
	case 'y':
		if ((m = manipulator_at(w, w->pos)) != NULL) {
			inst_del(m);
		}
		break;
	case ',':
	case '<':
		if ((m = manipulator_at(w, w->pos)) != NULL) {
			if (m->pc->list.prev != &m->inst_list)
				m->pc = list_entry(m->pc->list.prev, struct inst, list);
		}
		break;
	case '.':
	case '>':
		if ((m = manipulator_at(w, w->pos)) != NULL) {
			if (m->pc->list.next != &m->inst_list)
				m->pc = list_entry(m->pc->list.next, struct inst, list);
		}
		break;
	/* Execution */
	case 'x':
		for_each_position(pos, w->size.x, w->size.y, w->size.z) {
			if ((m = manipulator_at(w, pos)) != NULL)
				step(w, m);
		}
		break;
	case 'X':
		run(w);
		break;
	case 'g':
		for_each_position(pos, w->size.x, w->size.y, w->size.z) {
			if ((g = glyph_at(w, pos)) != NULL)
				act(w, g);
		}
		break;
	/* Special Commands (Quit) */
	case 'Q':
		return -1;
		break;
	default:
		break;
	}

	return 0;
}

void run(struct workspace *w) {
	struct manipulator *m;
	struct glyph *g;
	struct position pos;
	int cmd;
	clock_t t;

	t = clock() + (CLOCKS_PER_SEC / 2);
	timeout(0);
	for_each_position(pos, w->size.x, w->size.y, w->size.z) {
		if ((m = manipulator_at(w, pos)) != NULL)
			m->pc = list_entry(m->inst_list.next, struct inst, list);
	}
	display(w);

	do {
		cmd = getch();
		if (clock() > t ) {
			for_each_position(pos, w->size.x, w->size.y, w->size.z) {
				if ((g = glyph_at(w, pos)) != NULL)
					act(w, g);
			}
			for_each_position(pos, w->size.x, w->size.y, w->size.z) {
				if ((m = manipulator_at(w, pos)) != NULL)
					step(w, m);
			}
			display(w);
			t += (CLOCKS_PER_SEC / 2);
		}
	} while (cmd != 'Q');

	timeout(-1);
}

enum op pick_op(int x) {
	enum op op = 0;
	int cmd;
	enum op y;

	do {
		mvprintw(1, x, "Instructions");
		for (y = 0; y <= NOP; y++) {
			if (y == op)
				attron(A_STANDOUT);
			mvprintw(2+(int)y, x, op_string(y));
			if (y == op)
				attroff(A_STANDOUT);
		}
		cmd = getch();
		if (cmd == KEY_DOWN)
			op = (op == NOP) ? (NOP) : (op + 1);
		else if (cmd == KEY_UP)
			op = (op == 0) ? (op) : (op - 1);
	} while (cmd != '\n');
	return op;
}

enum element pick_element(int x) {
	enum element element = ATOM_H;
	int cmd;
	enum element y;

	do {
		mvprintw(1, x, "Elements");
		for (y = 0; y < ATOM_END; y++) {
			if (y == element)
				attron(A_STANDOUT);
			mvprintw(2+(int)y, x, element_string(y));
			if (y == element)
				attroff(A_STANDOUT);
		}
		cmd = getch();
		if (cmd == KEY_DOWN)
			element = (element == (ATOM_END-1)) ? (element) : (element + 1);
		else if (cmd == KEY_UP)
			element = (element == 0) ? (element) : (element - 1);
	} while (cmd != '\n');
	return element;
}

//TODO it would be better if it was draw_bond(position, direction) instead
void draw_bond(struct position p1, struct position p2) {
	if ((p2.y == p1.y) && (p2.z == p1.z)) {
		if (p2.x == (p1.x - 1)) {
			mvprintw(3*p1.y+1, 4*p1.x-1, "--");
		} else if (p2.x == (p1.x + 1)) {
			mvprintw(3*p1.y+1, 4*p1.x+3, "--");
		}
	} else if ((p2.x == p1.x) && (p2.z == p1.z)) {
		if (p2.y == (p1.y - 1)) {
			mvaddch(3*p1.y-1, 4*p1.x+1, '|');
			mvaddch(3*p1.y,   4*p1.x+1, '|');
		} else if (p2.y == (p1.y + 1)) {
			mvaddch(3*p1.y+2, 4*p1.x+1, '|');
			mvaddch(3*p1.y+3, 4*p1.x+1, '|');
		}
	}
}

void draw_atom(struct atom *a)
{
	enum orientation dir;
	struct position adj;

	mvaddstr(3*a->pos.y+1, 4*a->pos.x+1, element_string(a->element));

	for (dir = 0; dir < ORIENTATION_NUM; dir++) {
		if (a->bonds[dir] > 0) {
			adj = pos_add(a->pos, pos_shift(dir, 1));
			draw_bond(a->pos, adj);
		}
	}
}

void draw_manipulator(struct manipulator *m)
{
	int dx=0, dy=0;
	int head_x, head_y;
	int x = m->pos.x;
	int y = m->pos.y;

	/* Draw the base */
	mvaddstr(3*y,   4*x, "/--\\");
	mvaddstr(3*y+1, 4*x, "|  |");
	mvaddstr(3*y+2, 4*x, "\\--/");

	/* Draw the arm */
	switch(m->orient) {
		case NX:
			for (dx = -1; dx > -(m->length); dx--) {
				mvaddch(3*y+1, 4*x+4*dx+3, '=');
				mvaddch(3*y+1, 4*x+4*dx+2, '=');
				mvaddch(3*y+1, 4*x+4*dx+1, '=');
				mvaddch(3*y+1, 4*x+4*dx,   '=');
			}
			break;
		case PX:
			for (dx = 1; dx < m->length; dx++) {
				mvaddch(3*y+1, 4*x+4*dx,   '=');
				mvaddch(3*y+1, 4*x+4*dx+1, '=');
				mvaddch(3*y+1, 4*x+4*dx+2, '=');
				mvaddch(3*y+1, 4*x+4*dx+3, '=');
			}
			break;
		case NY:
			for (dy = -1; dy > -(m->length); dy--) {
				mvaddch(3*y+3*dy+2, 4*x+1, '|');
				mvaddch(3*y+3*dy+1, 4*x+1, '|');
				mvaddch(3*y+3*dy,   4*x+1, '|');
			}
			break;
		case PY:
			for (dy = 1; dy < m->length; dy++) {
				mvaddch(3*y+3*dy,   4*x+1, '|');
				mvaddch(3*y+3*dy+1, 4*x+1, '|');
				mvaddch(3*y+3*dy+2, 4*x+1, '|');
			}
			break;
		case NZ:
		case PZ:
			mvaddch(3*y+1, 4*x+1, '*');
			break;
		default:
			break;
	}

	/* Draw the head */
	head_y = 3*y+3*dy;
	head_x = 4*x+4*dx;
	if (m->orient != NY) {
		mvaddstr(head_y,   head_x, "/--\\");
	} else {
		mvaddch( head_y,   head_x,   '/'  );
		mvaddch( head_y,   head_x+3, '\\' );
	}
	if (m->orient != NX) {
		mvaddch( head_y+1, head_x,   '|'  );
	}
	if (m->orient != PX) {
		mvaddch( head_y+1, head_x+3, '|'  );
	}
	if (m->orient != PY) {
		mvaddstr(head_y+2, head_x, "\\--/");
	} else {
		mvaddch( head_y+2, head_x,   '\\' );
		mvaddch( head_y+2, head_x+3, '/'  );
	}

	/* Erase the correct characters if the head is open */
	if (m->head_state == HEAD_OPEN) {
		switch(m->orient) {
		case NY:
			mvaddch(head_y,   head_x,   '\\');
			mvaddch(head_y,   head_x+3, '/');
			break;
		case NX:
			mvaddch(head_y,   head_x,   '\\');
			mvaddch(head_y+2, head_x,   '/');
			break;
		case PY:
			mvaddch(head_y+2, head_x,   '/');
			mvaddch(head_y+2, head_x+3, '\\');
			break;
		case PX:
			mvaddch(head_y,   head_x+3, '/');
			mvaddch(head_y+2, head_x+3, '\\');
			break;
		default:
			break;
		}
	}
}

void print_inst_list(struct manipulator *m, int x)
{
	struct inst *i;
	int y=0;

	mvprintw(1, x, "Program");
	list_for_each_entry(i, &m->inst_list, list) {
		if (i == m->pc)
			attron(A_STANDOUT);
		mvprintw(2+y, x, op_string(i->op));
		if (i == m->pc)
			attroff(A_STANDOUT);
		y++;
	}
}

void draw_glyph(struct glyph *g)
{
	struct position adj;
	switch(g->op) {
	case BOND:
		mvaddch(3*g->pos.y+2, 4*g->pos.x+3, '+');
		adj = pos_add(g->pos, pos_shift((enum orientation)g->type, 1));
		mvaddch(3*adj.y+2, 4*adj.x+3, '+');
		break;
	case UNBOND:
		mvaddch(3*g->pos.y+2, 4*g->pos.x+3, '-');
		adj = pos_add(g->pos, pos_shift((enum orientation)g->type, 1));
		mvaddch(3*adj.y+2, 4*adj.x+3, '-');
		break;
	case SOURCE:
		mvaddch(3*g->pos.y+1, 4*g->pos.x  , '<');
		mvaddch(3*g->pos.y+1, 4*g->pos.x+3, '>');
		break;
	case TRANSMUTE:
		break;
	default:
		break;
	}
}
