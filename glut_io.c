#include <GL/glut.h>
#include <GL/gl.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include "io.h"
#include "workspace.h"

static workspace_t *w_io;

/* Local Function Prototypes */
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);

void draw_manipulator(manipulator_t *m);

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

	//glutDisplayFunc(display);
	glutIdleFunc(display);
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

	list_for_each_entry(g, &w->glyphs, list) {
		//TODO draw glyph
	}

	list_for_each_entry(m, &w->manipulators, list) {
		//TODO draw manipulator
		draw_manipulator(m);
	}

	list_for_each_entry(a, &w->atoms, list) {
		//TODO draw atom
	}

	glutSwapBuffers();
}

void reshape(int w, int h) {
	//TODO reshape viewwindow
	float ratio = 1.0;
	int mx, my, Mx, My;

	if(w > h) {
		my = 0; My = h;
		mx = (h-w) / 2; Mx = (h+w)/2;
	} else {
		mx = 0; Mx = w;
		my = (w-h)/2; My = (w+h)/2;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(mx, my, Mx, My);
	gluPerspective(45, ratio, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
	(void)x;
	(void)y;
	if(key == 0x1B)
		exit(0);
}

void draw_manipulator(manipulator_t *m) {
	glPushMatrix();

	glTranslatef(m->pos.x, m->pos.y, m->pos.z-10);

	switch(m->orient) {
	case NX:
		glRotatef(90, 0, 1, 0);
		//glTranslatef();
		break;
	case PX:
		glRotatef(-90, 0, 1, 0);
		//glTranslatef();
		break;
	case NY:
		glRotatef(90, 1, 0, 0);
		//glTranslatef();
		break;
	case PY:
		glRotatef(-90, 1, 0, 0);
		//glTranslatef();
		break;
	case NZ:
		glRotatef(180, 0, 1, 0);
		//glTranslatef();
		break;
	case PZ:
		//glRotatef(M_PI_4, 0, 1, 0);
		//glTranslatef();
		break;
	default:
		break;
	}

	glutWireSphere(0.5, 20, 20);
	glutSolidCone(0.125, m->length, 20, 5);
	glPopMatrix();
}
