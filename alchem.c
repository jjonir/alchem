#include <stdlib.h>
#include <string.h>
#include "io.h"
#include "alchem.h"
#include "atom.h"
#include "glyph.h"
#include "manipulator.h"
#include "workspace.h"

#ifdef ENABLE_LOG
FILE *logfile;
int indent_level;
#endif

int main() {
	struct workspace *w;
	struct manipulator *m;
	struct glyph *g;

#ifdef ENABLE_LOG
	logfile = fopen("log", "a");
	fprintf(logfile, "--------------------\n");
	fprintf(logfile, "Starting Alchem\n\n");
#endif

	init_io();

	w = new_workspace(5, 5, 5);

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
	g = new_glyph(SOURCE, (int)ATOM_AL, position(3, 0, 0));
	add_glyph(w, g, position(3, 0, 0));
	g = new_glyph(BOND, (int)PX, position(1, 2, 0));
	add_glyph(w, g, position(1, 2, 0));

	edit_workspace_loop(w);

	end_io();
	return 0;
}
