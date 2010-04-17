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

	void get_normalized_points(std::vector<std::pair<float, float> > *points);
	
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
