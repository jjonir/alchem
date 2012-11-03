#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "io.h"
#include "atom.h"
#include "glyph.h"
#include "manipulator.h"
#include "workspace.h"

int main() {
	struct workspace *w;
	struct manipulator *m;
	struct glyph *g;

	init_io();

	w = new_workspace(10, 10, 10);

	m = new_manipulator(PX, 2, position(1, 0, 0));
	inst_add(m, CLOSE_HEAD);
	inst_add(m, ROT_PZ);
	inst_add(m, OPEN_HEAD);
	inst_add(m, ROT_NZ);
	add_manipulator(w, m, position(1, 0, 0));

	m = new_manipulator(PX, 2, position(0, 2, 0));
	inst_add(m, OPEN_HEAD);
	inst_add(m, RETRACT);
	inst_add(m, CLOSE_HEAD);
	inst_add(m, EXTEND);
	add_manipulator(w, m, position(0, 2, 0));
/*
	add_manipulator(w, new_manipulator(PX, 4, 0, 0, 0));
	add_manipulator(w, new_manipulator(NX, 4, 1, 0, 0));
	add_manipulator(w, new_manipulator(PY, 4, 0, 1, 0));
	add_manipulator(w, new_manipulator(NY, 4, 1, 1, 0));

	add_manipulator(w, new_manipulator(PZ, 4, 2, 0, 0));
	add_manipulator(w, new_manipulator(NZ, 4, 3, 0, 0));
*/
	g = new_glyph(SOURCE, (uint8_t)ATOM_AL, position(3, 0, 0));
	add_glyph(w, g, position(3, 0, 0));
	g = new_glyph(BOND, (uint8_t)PX, position(1, 2, 0));
	add_glyph(w, g, position(1, 2, 0));

	edit_workspace_loop(w);

	end_io();
	return 0;
}
