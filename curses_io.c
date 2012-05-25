#include <curses.h>
#include <time.h>
#include <stdint.h>
#include "io.h"
#include "workspace.h"

int foo = 0;

/* Local Function Prototypes */
void display(workspace_t *w);
int8_t process_input_blocking(workspace_t *w);
void run(workspace_t *w);
op_t pick_op(void);
element_t pick_element(void);
void draw_bond(position_t p1, position_t p2);
void draw_manipulator(manipulator_t *m);
void print_inst_list(manipulator_t *m);
void draw_glyph(glyph_t *g);

/* External Interface Functions */
void init_io(void) {
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
}

void end_io(void) {
	endwin();
}

void edit_workspace_loop(workspace_t *w) {
	int8_t cmd;
	do {
		display(w);
		cmd = process_input_blocking(w);
	} while(cmd != -1);
}

/* Local Functions */
void display(workspace_t *w) {
	int x, y, z;
	atom_t *a;
	bond_t *b;
	manipulator_t *m;
	glyph_t *g;

	z = 0;

	clear();

	/* Draw the grid */
	for(x = 0; x < GRID_SZ_X; x++) {
		for(y = 0; y < GRID_SZ_Y; y++) {
			if((x == w->pos.x) && (y == w->pos.y) && (z == w->pos.z))
				attron(A_STANDOUT);
			mvaddch(3*y+1, 4*x+1, '.');
			if((x == w->pos.x) && (y == w->pos.y) && (z == w->pos.z))
				attroff(A_STANDOUT);
		}
	}

	/* Draw glyphs */
	list_for_each_entry(g, &w->glyphs, list) {
		if(POS_EQ(w->pos, g->pos1) || POS_EQ(w->pos, g->pos2))
			attron(A_STANDOUT);
		draw_glyph(g);
		if(POS_EQ(w->pos, g->pos1) || POS_EQ(w->pos, g->pos2))
			attroff(A_STANDOUT);
	}

	/* Draw the manipulators */
	list_for_each_entry(m, &w->manipulators, list) {
		if(POS_EQ(w->pos, m->pos))
			attron(A_STANDOUT);
		draw_manipulator(m);
		if(POS_EQ(w->pos, m->pos)) {
			attroff(A_STANDOUT);
			print_inst_list(m);
		}
	}

	/* Redraw the selected manipulator to be sure it's on top */
	attron(A_STANDOUT);
	if((m = manipulator_at(&w->manipulators, w->pos)) != NULL)
		draw_manipulator(m);
	attroff(A_STANDOUT);

	/* Draw the atoms */
	list_for_each_entry(a, &w->atoms, list) {
		if(POS_EQ(w->pos, a->pos))
			attron(A_STANDOUT);
		mvaddstr(3*a->pos.y+1, 4*a->pos.x+1, element_string(a->element));
		list_for_each_entry(b, &a->bond_list, list) {
			draw_bond(a->pos, b->bonded_atom->pos);
		}
		if(POS_EQ(w->pos, a->pos))
			attroff(A_STANDOUT);
	}

	/* Draw the cursor if there's no object selected */
	if(!manipulator_at(&w->manipulators, w->pos) &&
			!atom_at(&w->atoms, w->pos)) {
		attron(A_STANDOUT);
		mvaddch(3*w->pos.y+1, 4*w->pos.x+1, '.');
		attroff(A_STANDOUT);
	}

	refresh();
}

// TODO document input commands; both in README.md and in a case 'h' or case '?'
int8_t process_input_blocking(workspace_t *w) {
	manipulator_t *m, *mindex;
	glyph_t *g, *gindex;
	int c = getch();

	m = manipulator_at(&w->manipulators, w->pos);
	g = glyph_at(&w->glyphs, w->pos);

	switch(c) {
	/* Grid Navigation */
	case KEY_LEFT:
		if(w->pos.x > 0)
			w->pos.x--;
		break;
	case KEY_RIGHT:
		if(w->pos.x < GRID_SZ_X-1)
			w->pos.x++;
		break;
	case KEY_UP:
		if(w->pos.y > 0)
			w->pos.y--;
		break;
	case KEY_DOWN:
		if(w->pos.y < GRID_SZ_Y-1)
			w->pos.y++;
		break;
	/* Thing Placement and Removal */
	case 'm':
		if(m == NULL)
			add_manipulator(w, new_manipulator(NY, 2, w->pos.x, w->pos.y, w->pos.z));
		break;
	case 'M':
		if(m != NULL)
			remove_manipulator(m);
		break;
	case 'b':
		if((g == NULL) && (m == NULL)) {
			if(glyph_at(&w->glyphs, pos(w->pos.x+1, w->pos.y, w->pos.z)) == NULL) {
				add_glyph(w, new_glyph(BOND, 0, w->pos, pos(w->pos.x+1, w->pos.y, w->pos.z)));
			}
		}
		break;
	case 'B':
		if(g != NULL)
			remove_glyph(g);
		break;
	case 'u':
		if((g == NULL) && (m == NULL)) {
			if(glyph_at(&w->glyphs, pos(w->pos.x+1, w->pos.y, w->pos.z)) == NULL) {
				add_glyph(w, new_glyph(UNBOND, 0, w->pos, pos(w->pos.x+1, w->pos.y, w->pos.z)));
			}
		}
		break;
	case 'U':
		if(g != NULL)
			remove_glyph(g);
		break;
	case 'j': //j for jar? or something
		if((g == NULL) && (m == NULL)) {
			add_glyph(w, new_glyph(SOURCE, (uint8_t)pick_element(), w->pos, w->pos));
		}
		break;
	/* Thing Movement */
	case 'a':
		if(m) {
			if(m->pos.x > 0) {
				w->pos.x--;
				if(manipulator_at(&w->manipulators, w->pos) == NULL)
					m->pos.x--;
				else
					w->pos.x++;
			}
		}
		break;
	case 'd':
		if(m) {
			if(m->pos.x < GRID_SZ_X-1) {
				w->pos.x++;
				if(manipulator_at(&w->manipulators, w->pos) == NULL)
					m->pos.x++;
				else
					w->pos.x--;
			}
		}
		break;
	case 'w':
		if(m) {
			if(m->pos.y > 0) {
				w->pos.y--;
				if(manipulator_at(&w->manipulators, w->pos) == NULL)
					m->pos.y--;
				else
					w->pos.y++;
			}
		}
		break;
	case 's':
		if(m) {
			if(m->pos.y < GRID_SZ_Y-1) {
				w->pos.y++;
				if(manipulator_at(&w->manipulators, w->pos) == NULL)
					m->pos.y++;
				else
					w->pos.y--;
			}
		}
		break;
	/* Manipulator Manipulation */
	case '[':
		if(m)
			rotate_ccw_z(m);
		break;
	case ']':
		if(m)
			rotate_cw_z(m);
		break;
	case '=':
	case '+':
		if(m)
			extend(m);
		break;
	case '-':
		if(m)
			retract(m);
		break;
	case '\\':
	case '|':
		if(m)
			toggle_head(m, &w->atoms);
		break;
	/* Instruction Manipulation */
	case 'i':
		if(m) {
			inst_add(m, pick_op());
		}
		break;
	case 'I':
		if(m) {
			inst_add_prev(m, pick_op());
		}
		break;
	case 'y':
		if(m) {
			inst_del(m);
		}
		break;
	case ',':
	case '<':
		if(m->pc->list.prev != &m->inst_list)
			m->pc = list_entry(m->pc->list.prev, inst_t, list);
		break;
	case '.':
	case '>':
		if(m->pc->list.next != &m->inst_list)
			m->pc = list_entry(m->pc->list.next, inst_t, list);
		break;
	/* Execution */
	case 'x':
		list_for_each_entry(mindex, &w->manipulators, list) {
			step(mindex, &w->atoms);
		}
		break;
	case 'X':
		run(w);
		break;
	case 'g':
		list_for_each_entry(gindex, &w->glyphs, list) {
			act(gindex, &w->atoms);
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

void run(workspace_t *w) {
	manipulator_t *mindex;
	glyph_t *gindex;
	int cmd;
	clock_t t;

	t = clock() + (CLOCKS_PER_SEC / 2);
	timeout(0);
	list_for_each_entry(mindex, &w->manipulators, list) {
		mindex->pc = list_entry(mindex->inst_list.next, inst_t, list);
	}
	display(w);

	do {
		cmd = getch();
		if(clock() > t ) {
			list_for_each_entry(gindex, &w->glyphs, list) {
				act(gindex, &w->atoms);
			}
			list_for_each_entry(mindex, &w->manipulators, list) {
				step(mindex, &w->atoms);
			}
			display(w);
			t += (CLOCKS_PER_SEC / 2);
		}
	} while(cmd != 'Q');

	timeout(-1);
}

op_t pick_op(void) {
	op_t op = 0;
	int cmd;
	op_t y;

	do {
		mvprintw(1, 4*GRID_SZ_X+10, "Instructions");
		for(y = 0; y <= NOP; y++) {
			if(y == op)
				attron(A_STANDOUT);
			mvprintw(2+y, 4*GRID_SZ_X+10, op_string(y));
			if(y == op)
				attroff(A_STANDOUT);
		}
		cmd = getch();
		if(cmd == KEY_DOWN)
			op = (op == NOP) ? (NOP) : (op + 1);
		else if(cmd == KEY_UP)
			op = (op == 0) ? (op) : (op - 1);
	} while(cmd != '\n');
	return op;
}

element_t pick_element(void) {
	element_t element = 0;
	int cmd;
	element_t y;

	do {
		mvprintw(1, 4*GRID_SZ_X+1, "Elements");
		for(y = 0; y < ATOM_END; y++) {
			if(y == element)
				attron(A_STANDOUT);
			mvprintw(2+y, 4*GRID_SZ_X+1, element_string(y));
			if(y == element)
				attroff(A_STANDOUT);
		}
		cmd = getch();
		if(cmd == KEY_DOWN)
			element = (element == (ATOM_END-1)) ? (element) : (element + 1);
		else if(cmd == KEY_UP)
			element = (element == 0) ? (element) : (element - 1);
	} while(cmd != '\n');
	return element;
}

void draw_bond(position_t p1, position_t p2) {
	if((p2.y == p1.y) && (p2.z == p1.z)) {
		if(p2.x == (p1.x - 1)) {
			mvprintw(3*p1.y+1, 4*p1.x-1, "--");
		} else if(p2.x == (p1.x + 1)) {
			mvprintw(3*p1.y+1, 4*p1.x+3, "--");
		}
	} else if((p2.x == p1.x) && (p2.z == p1.z)) {
		if(p2.y == (p1.y - 1)) {
			mvaddch(3*p1.y-1, 4*p1.x+1, '|');
			mvaddch(3*p1.y,   4*p1.x+1, '|');
		} else if(p2.y == (p1.y + 1)) {
			mvaddch(3*p1.y+2, 4*p1.x+1, '|');
			mvaddch(3*p1.y+3, 4*p1.x+1, '|');
		}
	}
}

void draw_manipulator(manipulator_t *m) {
	int8_t dx=0, dy=0;
	uint8_t head_x, head_y;
	uint8_t x = m->pos.x;
	uint8_t y = m->pos.y;

	/* Draw the base */
	mvaddstr(3*y,   4*x, "/--\\");
	mvaddstr(3*y+1, 4*x, "|  |");
	mvaddstr(3*y+2, 4*x, "\\--/");

	/* Draw the arm */
	switch(m->orient) {
		case NX:
			for(dx = -1; dx > -(m->length); dx--) {
				mvaddch(3*y+1, 4*x+4*dx+3, '=');
				mvaddch(3*y+1, 4*x+4*dx+2, '=');
				mvaddch(3*y+1, 4*x+4*dx+1, '=');
				mvaddch(3*y+1, 4*x+4*dx,   '=');
			}
			break;
		case PX:
			for(dx = 1; dx < m->length; dx++) {
				mvaddch(3*y+1, 4*x+4*dx,   '=');
				mvaddch(3*y+1, 4*x+4*dx+1, '=');
				mvaddch(3*y+1, 4*x+4*dx+2, '=');
				mvaddch(3*y+1, 4*x+4*dx+3, '=');
			}
			break;
		case NY:
			for(dy = -1; dy > -(m->length); dy--) {
				mvaddch(3*y+3*dy+2, 4*x+1, '|');
				mvaddch(3*y+3*dy+1, 4*x+1, '|');
				mvaddch(3*y+3*dy,   4*x+1, '|');
			}
			break;
		case PY:
			for(dy = 1; dy < m->length; dy++) {
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
	if(m->orient != NY) {
		mvaddstr(head_y,   head_x, "/--\\");
	} else {
		mvaddch( head_y,   head_x,   '/'  );
		mvaddch( head_y,   head_x+3, '\\' );
	}
	if(m->orient != NX) {
		mvaddch( head_y+1, head_x,   '|'  );
	}
	if(m->orient != PX) {
		mvaddch( head_y+1, head_x+3, '|'  );
	}
	if(m->orient != PY) {
		mvaddstr(head_y+2, head_x, "\\--/");
	} else {
		mvaddch( head_y+2, head_x,   '\\' );
		mvaddch( head_y+2, head_x+3, '/'  );
	}

	/* Erase the correct characters if the head is open */
	if(m->head_state == HEAD_OPEN) {
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

void print_inst_list(manipulator_t *m) {
	inst_t *i;
	int y=0;

	mvprintw(1, 4*GRID_SZ_X+1, "Program");
	list_for_each_entry(i, &m->inst_list, list) {
		if(i == m->pc)
			attron(A_STANDOUT);
		mvprintw(2+y, 4*GRID_SZ_X+1, op_string(i->op));
		if(i == m->pc)
			attroff(A_STANDOUT);
		y++;
	}
}

void draw_glyph(glyph_t *g) {
	switch(g->op) {
	case BOND:
		mvaddch(3*g->pos1.y+2, 4*g->pos1.x+3, '+');
		mvaddch(3*g->pos2.y+2, 4*g->pos2.x+3, '+');
		break;
	case UNBOND:
		mvaddch(3*g->pos1.y+2, 4*g->pos1.x+3, '-');
		mvaddch(3*g->pos2.y+2, 4*g->pos2.x+3, '-');
		break;
	case SOURCE:
		mvaddch(3*g->pos1.y+1, 4*g->pos1.x  , '<');
		mvaddch(3*g->pos1.y+1, 4*g->pos1.x+3, '>');
		break;
	case TRANSMUTE:
		break;
	default:
		break;
	}
}
