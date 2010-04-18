
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
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

void Chart::get_normalized_points(std::vector<std::pair<float, float> > *points)
{
//TODO: get data from db taking care of config
	std::vector<double> prices; std::vector<int> dates; std::vector<int> times;
	int r = dbfeeder.get_value_prices(config.value.c_str(),
	    config.day_start, config.time_start, config.day_end, config.time_end,
	    &prices, &dates, &times);
	DLOG("Chart::get_normalized_points() -> dbfeeder.get_value_prices: %d", r);
//TODO: maybe do some truncating on the data
	double max_price = 0.0;
	double min_price = 1e100;
	for (int i = 0; i < prices.size(); i++)
	{
		if (prices[i] > max_price) max_price = prices[i];
		if (prices[i] < min_price) min_price = prices[i];
	}
	max_price -= min_price;
	if (min_price > max_price)
	{
//		min_price -= max_price;
//		max_price *= 2.0;
	}
	int max_time = 1, min_time = 0;
	if (times.size())
	{
		min_time = times[0];
		max_time = times[times.size() - 1] - min_time;
	}
	points->clear();
	DLOG("Chart::get_normalized_points() -> max_price: %E", max_price);
	for (int i = 0; i < prices.size(); i++)
	{
//TODO: normalize each point
		points->push_back(std::make_pair<float, float>
		    ((((float)times[i] - min_time) / max_time), (prices[i] - min_price) / max_price));
//		    ((float)i / prices.size(), (prices[i] - min_price) / max_price));
//		points->push_back(std::make_pair<float, float>
//		    (((float)random() / RAND_MAX),((float)random() / RAND_MAX)));
	}
}

void Chart::display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	glColor3f (1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f (-.05, 0.0, 0.0);
	glVertex3f (1.05, 0.0, 0.0);
	glVertex3f (0.0, -.05, 0.0);
	glVertex3f (0.0, 1.05, 0.0);
//	glVertex3f (0.0, 0.0, -1.1);
//	glVertex3f (0.0, 0.0, 1.1);
	glEnd();

	std::vector<std::pair<float, float> > points;
	chart.get_normalized_points(&points);
	glColor3f(.0, 1.0, .0);
/*	for (int i = 0; i < points.size(); i++)
	{
		glPushMatrix();
		glTranslated(points[i].first, points[i].second, .0);
		glutSolidSphere(.003, 10, 8);
		glPopMatrix();
	}
*/
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < points.size(); i++)
	{
		glVertex3f(points[i].first, points[i].second, .0);
	}
	glEnd();
	
	glutSwapBuffers();
}

void Chart::reshape(int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-.08, 1.08, -.08, 1.08, -1.08, 1.08);
//	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
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
}
