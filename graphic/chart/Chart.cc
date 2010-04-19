
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <float.h>
#include "logger.h"
#include "Chart.h"

Chart Chart::chart;

Chart::Chart() : dbfeeder(&db), dbstatistics(&db)
{
	clear();
}

void Chart::init()
{
	if (db.connect(config.db_conninfo.c_str()) != CONNECTION_OK)
	{
		ELOG("Chart::init() -> db.connect(%s)", config.db_conninfo.c_str());
	}
}

void Chart::clear()
{
	min_x = min_y = FLT_MAX;
	max_x = max_y = FLT_MIN;
	points.clear();
}

void Chart::add_point(float x, float y)
{
	if (x < min_x) min_x = x;
	if (x > max_x) max_x = x;
	if (y < min_y) min_y = y;
	if (y > max_y) max_y = y;

	points.push_back(std::make_pair<float, float>(x, y));
}

void Chart::normalize()
{
	ILOG("Chart::normalize() -> time range: %f - %f", min_x, max_x);
	ILOG("Chart::normalize() -> value range: %f - %f", min_y, max_y);
	max_x -= min_x;
	max_y -= min_y;
	for (int i = 0; i < points.size(); i++)
	{
		points[i].first = (points[i].first - min_x) / max_x;
		points[i].second = (points[i].second - min_y) / max_y;
	}
}

float Chart::to_sec(int time)
{
	return ((((time / 10000) * 60) + ((time % 10000) / 100)) * 60) + (time % 100); 
}

float Chart::to_day(int date)
{
	return ((((date / 10000) * 12) + ((date % 10000) / 100)) * 30) + (date % 100);
}

void Chart::get_points()
{
	int r = -1;
	std::vector<double> raw_data;
	std::vector<int> raw_dates, raw_times;
	switch (config.item)
	{
	case CHARTITEM_PRICE:
		r = dbfeeder.get_value_prices(config.value.c_str(),
				config.day_start, config.time_start, config.day_end, config.time_end,
			    &raw_data, &raw_dates, &raw_times);
		break;
	case CHARTITEM_VOLUME:
		r = dbfeeder.get_value_volumes(config.value.c_str(),
			    config.day_start, config.time_start, config.day_end, config.time_end,
			    &raw_data, &raw_dates, &raw_times);
		break;
	case CHARTITEM_CAPITAL:
		r = dbfeeder.get_value_capitals(config.value.c_str(),
			    config.day_start, config.time_start, config.day_end, config.time_end,
			    &raw_data, &raw_dates, &raw_times);
		break;
	}
	
	if (raw_data.size())
	{
		float acum_y = (config.item == CHARTTYPE_MEAN) ? 0.0 : raw_data[0];
		int current_date = raw_dates[0];
		int cnt = 0, acum_x = 0;
		for (int i = 0; i < raw_data.size(); i++)
		{
			if (raw_dates[i] > current_date)
			{
				switch (config.type)
				{
				case CHARTTYPE_CLOSE:
				case CHARTTYPE_MIN:
				case CHARTTYPE_MAX:
					add_point(to_day(current_date), acum_y);
					acum_y = raw_data[i];
					break;
				case CHARTTYPE_MEAN:
					add_point(to_day(current_date), acum_y / cnt);
					acum_y = 0.0;
					cnt = 0;
					break;
				}
				acum_x += (24 * 3600);
				current_date = raw_dates[i];
			}
			switch (config.type)
			{
			case CHARTTYPE_ALL:
				add_point(acum_x + to_sec(raw_times[i]), raw_data[i]);
				break;
			case CHARTTYPE_CLOSE:
				acum_y = raw_data[i];
				break;
			case CHARTTYPE_MIN:
				if (acum_y > raw_data[i]) acum_y = raw_data[i];
				break;
			case CHARTTYPE_MEAN:
				acum_y += raw_data[i];
				cnt++;
				break;
			case CHARTTYPE_MAX:
				if (acum_y < raw_data[i]) acum_y = raw_data[i];
				break;
			}
		}
		switch (config.type)
		{
		case CHARTTYPE_CLOSE:
		case CHARTTYPE_MIN:
		case CHARTTYPE_MAX:
			add_point(to_day(current_date), acum_y);
			break;
		case CHARTTYPE_MEAN:
			if (cnt) add_point(to_day(current_date), acum_y / cnt);
			break;
		}
	}
	DLOG("Chart::get_points() -> r: %d raw: %d points: %d", r, raw_data.size(), points.size());
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

	chart.clear();
	chart.get_points();
	chart.normalize();

	glColor3f(.0, 1.0, .0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < chart.points.size(); i++)
	{
		glVertex3f(chart.points[i].first, chart.points[i].second, .0);
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
