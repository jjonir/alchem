#include <GL/glut.h>
#include <GL/gl.h>
#include <time.h>
#include <math.h>
#include "io.h"
#include "alchem.h"
#include "atom.h"
#include "glyph.h"
#include "manipulator.h"
#include "workspace.h"

#define SIMULATION_STEP_TIME 100

static struct workspace *w_io;
static int running = 0;
static int prevx, prevy;
static int rotating = 0;
static int panning = 0;
static float rotx = 0, roty = 0;
static float panx = 0, pany = 0;

/* Local Function Prototypes */
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

void draw_manipulator(struct manipulator *m);
void draw_glyph(struct glyph *g);
void draw_atom(struct atom *a);
void draw_grid(void);

void glut_sim_start(void);
void glut_sim_step(int val);
void glut_sim_stop(void);

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
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	//glutFullScreen();
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

	for_each_position(pos, w->size.x, w->size.y, w->size.z) {
		if ((a = atom_at(w, pos)) != NULL)
			draw_atom(a);
		if ((g = glyph_at(w, pos)) != NULL)
			draw_glyph(g);
		else if ((m = manipulator_at(w, pos)) != NULL)
			draw_manipulator(m);
		else
			; // TODO draw something?
	}

	glutSwapBuffers();
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
	glOrtho(-2 + panx, w_io->size.x + 2 + panx,
		-(w_io->size.y + 2 + pany), -(-2 + pany), -10, 10);
	glRotatef(rotx, 0, 1, 0);
	glRotatef(roty, 1, 0, 0);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
	(void)x;
	(void)y;

	if (key == 0x1B)
		exit(0);
	if (key == 'X') {
		if (running) {
			glut_sim_stop();
		} else {
			glut_sim_start();
		}
	}
}

void mouse(int button, int state, int x, int y) {
	prevx = x;
	prevy = y;
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN)
			rotating = 1;
		else
			rotating = 0;
	} else if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN)
			panning = 1;
		else
			panning = 0;
	}
}

void motion(int x, int y) {
	int dx = x - prevx;
	int dy = y - prevy;
	prevx = x;
	prevy = y;

	if (rotating) {
		rotx -= dx;
		roty -= dy;
	}
	if (panning) {
		panx -= ((float)dx * (4 + w_io->size.x) / glutGet(GLUT_WINDOW_WIDTH));
		pany -= ((float)dy * (4 + w_io->size.y) / glutGet(GLUT_WINDOW_HEIGHT));
	}
	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glutPostRedisplay();
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

	for_each_position(pos, w_io->size.x, w_io->size.y, w_io->size.z) {
		glPushMatrix();
		glTranslatef(pos.x, -pos.y, pos.z);
		glColor3f(0.0, 0.0, 1.0);
		glutSolidSphere(0.05, 10, 10);
		glPopMatrix();
	}
}

void glut_sim_start() {
	sim_start(w_io);
	running = 1;
	glutTimerFunc(SIMULATION_STEP_TIME, glut_sim_step, 0);
}

void glut_sim_step(int val) {
	(void)val;

#ifdef ENABLE_LOG
	static int stepct = 0;
	logf("\nStarting glut_io sim step %i\n", stepct++);
#endif

	sim_step(w_io);

	glutPostRedisplay();
	if (running)
		glutTimerFunc(SIMULATION_STEP_TIME, glut_sim_step, 0);
}

void glut_sim_stop() {
	sim_stop(w_io);
	running = 0;
}
