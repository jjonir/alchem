#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <stdint.h>
#include "io.h"
#include "workspace.h"
#include "atom.h"
#include "manipulator.h"

int main() {
	int8_t cmd;
	workspace_t *w;

	init_io();

	w = new_workspace();
	add_atom(w, new_atom(ATOM_C, pos(0, 0, 0)));
	add_atom(w, new_atom(ATOM_AL, pos(1, 1, 0)));
	add_atom(w, new_atom(ATOM_F, pos(2, 3, 0)));
	add_atom(w, new_atom(ATOM_O, pos(9, 9, 0)));
	add_manipulator(w, new_manipulator(NX, 2, 2, 5, 0));
	add_manipulator(w, new_manipulator(PX, 2, 3, 5, 0));
	do {
		display(w);
		cmd = process_input_blocking(w);
	} while(cmd != -1);

	end_io();
	return 0;
}
