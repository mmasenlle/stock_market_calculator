#ifndef _CHART_H_
#define _CHART_H_

#include <vector>
#include "DBfeeder.h"
#include "DBstatistics.h"
#include "ChartConfig.h"

class Chart
{
	CcltorDB db;
	DBfeeder dbfeeder;
	DBstatistics dbstatistics;
	
	float min_x, max_x, min_y, max_y;
	std::vector<std::pair<float, float> > points;
	void clear();
	void add_point(float x, float y);
	void normalize();
	float to_sec(int time);
	float to_day(int date);
	void get_points();
	
	static void display();
	static void reshape(int w, int h);
	static void keyboard(unsigned char key, int x, int y);
	
public:
	ChartConfig config;

	Chart(); 
	void init();
	void show();
	
	static Chart chart;
};

#endif /*_CHART_H_*/
