#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "io.h"
#include "workspace.h"
#include "atom.h"
#include "manipulator.h"

int main() {
	workspace_t *w;
	manipulator_t *m;

	init_io();

	w = new_workspace();

	m = new_manipulator(PX, 2, 1, 0, 0);
	inst_add(m, CLOSE_HEAD);
	inst_add(m, ROT_PZ);
	inst_add(m, OPEN_HEAD);
	inst_add(m, ROT_NZ);
	add_manipulator(w, m);

	m = new_manipulator(PX, 2, 0, 2, 0);
	inst_add(m, OPEN_HEAD);
	inst_add(m, RETRACT);
	inst_add(m, CLOSE_HEAD);
	inst_add(m, EXTEND);
	add_manipulator(w, m);
/*
	add_manipulator(w, new_manipulator(PX, 4, 0, 0, 0));
	add_manipulator(w, new_manipulator(NX, 4, 1, 0, 0));
	add_manipulator(w, new_manipulator(PY, 4, 0, 1, 0));
	add_manipulator(w, new_manipulator(NY, 4, 1, 1, 0));

	add_manipulator(w, new_manipulator(PZ, 4, 2, 0, 0));
	add_manipulator(w, new_manipulator(NZ, 4, 3, 0, 0));
*/
	add_glyph(w, new_glyph(SOURCE, (uint8_t)ATOM_AL, pos(3, 0, 0), pos(3, 0, 0)));
	add_glyph(w, new_glyph(BOND, 0, pos(1, 2, 0), pos(2, 2, 0)));

	edit_workspace_loop(w);

	end_io();
	return 0;
}
