#include <GL/glut.h>
#include <GL/gl.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include "io.h"
#include "atom.h"
#include "glyph.h"
#include "manipulator.h"
#include "workspace.h"

static struct workspace *w_io;
static int running = 0;

/* Local Function Prototypes */
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void animate(int val);

void draw_grid(void);
void draw_manipulator(struct manipulator *m);
void draw_glyph(struct glyph *g);
void draw_atom(struct atom *a);

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

void edit_workspace_loop(struct workspace *w) {
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
	struct atom *a;
	struct manipulator *m;
	struct glyph *g;
	struct workspace *w = w_io;
	struct position pos;

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0, 0, -2);

	draw_grid();

	for_each_position(pos, w->width, w->height, w->depth) {
		if ((a = atom_at(w, pos)) != NULL)
			draw_atom(a);
		else if ((g = glyph_at(w, pos)) != NULL)
			draw_glyph(g);
		else if ((m = manipulator_at(w, pos)) != NULL)
			draw_manipulator(m);
		else
			; // TODO draw something?
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
	struct manipulator *m;
	struct position pos;

	if (key == 0x1B)
		exit(0);
	if (key == 'X') {
		if (running) {
			running = 0;
		} else {
			for_each_position(pos, w_io->width, w_io->height, w_io->depth) {
				if ((m = manipulator_at(w_io, pos)) != NULL)
					m->pc = list_entry(m->inst_list.next, struct inst, list);
			}
			running = 1;
			glutTimerFunc(0, animate, 0);
		}
	}
}

void draw_manipulator(struct manipulator *m) {
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

void draw_glyph(struct glyph *g) {
	glPushMatrix();
	glTranslatef(g->pos.x, -g->pos.y, g->pos.z);

	switch (g->op) {
	case BOND:
		glColor3f(0.0, 1.0, 0.0);
		glutWireTorus(0.25, 0.5, 40, 40);
		break;
	case UNBOND:
		glColor3f(1.0, 0.0, 0.0);
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

void draw_atom(struct atom *a) {
	glPushMatrix();
	glTranslatef(a->pos.x, -a->pos.y, a->pos.z);

	glColor3f(0.5, 0.5, 0.5);
	glutSolidSphere(0.2, 40, 40);

	glPopMatrix();
}

void draw_grid(void) {
	struct position pos;

	for_each_position(pos, w_io->width, w_io->height, w_io->depth) {
		glPushMatrix();
		glTranslatef(pos.x, -pos.y, pos.z);
		glColor3f(0.0, 0.0, 1.0);
		glutSolidSphere(0.05, 10, 10);
		glPopMatrix();
	}
}

void animate(int val) {
	(void)val;
	struct manipulator *m;
	struct glyph *g;
	struct position pos;

	for_each_position(pos, w_io->width, w_io->height, w_io->depth) {
		if ((g = glyph_at(w_io, pos)) != NULL)
			act(w_io, g);
		if ((m = manipulator_at(w_io, pos)) != NULL)
			step(w_io, m);
	}

	glutPostRedisplay();
	if (running)
		glutTimerFunc(500, animate, 0);
}
