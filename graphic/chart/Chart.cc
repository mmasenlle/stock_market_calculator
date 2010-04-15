#define OPENGL

#ifdef OPENGL
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif
#include "logger.h"
#include "Chart.h"

Chart Chart::chart;

Chart::Chart() : dbfeeder(&db), dbstatistics(&db)
{
	
}

void Chart::init()
{
	if (db.connect(config.db_conninfo.c_str()) != CONNECTION_OK)
	{
		ELOG("Chart::init() -> db.connect(%s)", config.db_conninfo.c_str());
	}

}

void Chart::display()
{
#ifdef OPENGL
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	
	glutSwapBuffers();
#endif
}

void Chart::reshape(int w, int h)
{
#ifdef OPENGL
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.08, 1.08, -1.08, 1.08, -1.08, 1.08);
//	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
#endif
}

void Chart::keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{

	case 'q': case 'Q': case 27:
		exit(0);
	}
}

void Chart::show()
{
	char title[128];
	snprintf(title, sizeof(title), "CCLTOR CHART  '%s'   %s %s",
			config.value.c_str(), config.getType(), config.getItem());
#ifdef OPENGL
	int argc = 0;
	glutInit(&argc, NULL);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 480); 
	glutInitWindowPosition(100, 100);
	glutCreateWindow(title);
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glutMainLoop();
#endif
}

