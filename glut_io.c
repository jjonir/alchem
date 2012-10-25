#include <GL/glut.h>
#include <GL/gl.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include "io.h"
#include "workspace.h"

static workspace_t *w_io;
static int running = 0;

/* Local Function Prototypes */
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void animate(int val);

void draw_grid(void);
void draw_manipulator(manipulator_t *m);
void draw_glyph(glyph_t *g);
void draw_atom(atom_t *a);

/* External Interface Functions */
void init_io(void) {
	int argc = 1;
	char *argv[] = {"alchem"};
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1000, 700);
	glutCreateWindow("alchem");
}

void end_io(void) {
	//nothing to do here
	//glut is closed with exit() or something
}

void edit_workspace_loop(workspace_t *w) {
	w_io = w;

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	//glutSpecialFunc(special);
	//glutMouseFunc(mouse);
	//glutFullScreen();
	//TODO initialize scenegraph based on w?
	glutMainLoop();
}

/* Local Functions */
void display(void) {
	atom_t *a;
	manipulator_t *m;
	glyph_t *g;
	workspace_t *w = w_io;

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0, 0, -2);

	draw_grid();

	list_for_each_entry(g, &w->glyphs, list) {
		draw_glyph(g);
	}

	list_for_each_entry(m, &w->manipulators, list) {
		draw_manipulator(m);
	}

	list_for_each_entry(a, &w->atoms, list) {
		draw_atom(a);
	}

	glutSwapBuffers();

	glutPostRedisplay();
}

void reshape(int w, int h) {
	//TODO reshape viewwindow
	//float ratio = 1.0;
	int dim;

	if(w > h)
		dim = h;
	else
		dim = w;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, dim, dim);
	//gluPerspective(45, ratio, 0.1, 100000.0);
	glOrtho(-2, 8, -8, 2, -10, 10);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
	(void)x;
	(void)y;
	if (key == 0x1B)
		exit(0);
	if (key == 'X') {
		if (running) {
			running = 0;
		} else {
			manipulator_t *mindex;
			list_for_each_entry(mindex, &w_io->manipulators, list)
				mindex->pc = list_entry(mindex->inst_list.next, inst_t, list);
			running = 1;
			glutTimerFunc(0, animate, 0);
		}
	}
}

void draw_manipulator(manipulator_t *m) {
	glPushMatrix();
	glTranslatef(m->pos.x, -m->pos.y, m->pos.z);

	switch (m->orient) {
	case NX:
		glRotatef(-90, 0, 1, 0);
		break;
	case PX:
		glRotatef(90, 0, 1, 0);
		break;
	case NY:
		glRotatef(-90, 1, 0, 0);
		break;
	case PY:
		glRotatef(90, 1, 0, 0);
		break;
	case NZ:
		glRotatef(180, 0, 1, 0);
		break;
	case PZ:
		break;
	default:
		break;
	}

	glColor3f(1.0, 0.0, 0.0);
	glutWireSphere(0.25, 40, 40);
	glColor3f(0.0, 1.0, 0.0);
	glutWireCone(0.125, m->length, 20, 5);
	glPopMatrix();
}

void draw_glyph(glyph_t *g) {
	glPushMatrix();
	glTranslatef(g->pos1.x, -g->pos1.y, g->pos1.z);

	switch (g->op) {
	case BOND:
		glColor3f(0.0, 1.0, 0.0);
		glutWireTorus(0.25, 0.5, 40, 40);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(g->pos2.x, -g->pos2.y, g->pos2.z);
		glutWireTorus(0.25, 0.5, 40, 40);
		break;
	case UNBOND:
		glColor3f(1.0, 0.0, 0.0);
		glutWireTorus(0.25, 0.5, 40, 40);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(g->pos2.x, -g->pos2.y, g->pos2.z);
		glutWireTorus(0.25, 0.5, 40, 40);
		break;
	case SOURCE:
		glColor3f(0.0, 0.0, 1.0);
		glutWireCube(0.5);
		break;
	case TRANSMUTE:
		break;
	default:
		break;
	}

	glPopMatrix();
}

void draw_atom(atom_t *a) {
	glPushMatrix();
	glTranslatef(a->pos.x, -a->pos.y, a->pos.z);

	glColor3f(0.5, 0.5, 0.5);
	glutSolidSphere(0.2, 40, 40);

	glPopMatrix();
}

void draw_grid(void) {
	int x, y, z;

	for (z = 0; z < GRID_SZ_Z; z++) {
		for (y = 0; y < GRID_SZ_Y; y++) {
			for (x = 0; x < GRID_SZ_X; x++) {
				glPushMatrix();
				glTranslatef(x, -y, z);
				glColor3f(0.0, 0.0, 1.0);
				glutSolidSphere(0.05, 10, 10);
				glPopMatrix();
			}
		}
	}
}

void animate(int val) {
	manipulator_t *mindex;
	glyph_t *gindex;
	(void)val;

	list_for_each_entry(gindex, &w_io->glyphs, list)
		act(gindex, &w_io->atoms);
	list_for_each_entry(mindex, &w_io->manipulators, list)
		step(mindex, &w_io->atoms);

	glutPostRedisplay();
	if (running)
		glutTimerFunc(500, animate, 0);
}
