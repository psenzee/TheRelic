#pragma warning(disable: 4786)

#include "linelist.h"
#include "point.h"
#include "constants.h"
#include "plotter.h"
#include "triangle.h"

#include <vector>
#include <iostream>

#include <math.h>
/*
void sphere(double radius, int slices, int rows, std::vector<PointTriangle> &triangles)
{
	double slicefactor = PI / slices,
	       rowfactor   = PI / rows,
		   r0, r1, sf0, sf1;
	Point  a, b, c, d;

    for (int j = 0; j < rows; j++)
	{
	    r0 = sin(j       * rowfactor) * radius;
		r1 = sin((j + 1) * rowfactor) * radius;
        for (int i = 0; i < slices; i++)
		{
		    sf0 = i * slicefactor; sf1 = (i + 1) * slicefactor;
		    a.set(r0 * sin(sf0), r0, r0 * cos(sf0));
			b.set(r0 * sin(sf1), r0, r0 * cos(sf1));
            c.set(r1 * sin(sf0), r1, r1 * cos(sf0));
			d.set(r1 * sin(sf1), r1, r1 * cos(sf1));
            triangles.push_back(PointTriangle(a, b, c));
			triangles.push_back(PointTriangle(c, d, a));
		}
	}
}



void triangulate(const std::vector<Point> &points, const LineList &lines, std::vector<Triangle> &triangles)
{
	if (lines.isConvex(points))
	{
	    const std::vector<int> &ls = lines.list;
        for (int i = 0, sz = ls.size() - 1; i < sz; i++)
	        triangles.push_back(Triangle(ls[0], ls[i], ls[i + 1]));
	}
	else
	{
        std::vector<LineList> lnls;
		if (lines.splitConcave(points, lnls))
            for (int i = 0, sz = lnls.size(); i < sz; i++)
			    triangulate(points, lnls[i], triangles);
	}
}

void linelistmain(int argc, const char **argv)
{
    std::vector<Point> points;
	std::vector<Line>  lines;

    int    segments = 16;
	double factor   = (PI / 2) / segments;

    for (int i = 0; i < 16; i++)
	{
        points.push_back(Point(0, sin(i * factor), cos(i * factor)));
        if (i > 0)
		    lines.push_back(Line(i - 1, i));
	}

	LineList lnlist;

	lnlist.set(lines);
	lnlist.optimize(points);

	std::cout << lnlist.isConvex(points);

    Plotter plotter;

	plotter.open(0x00ffffff);

	//plotter.plot(lnlist, points, 0);

    points[8].y -= 0.1f;

	//plotter.plot(lnlist, points, 0x000000ff);

	std::cout << lnlist.isConvex(points);

    points[8].y += 0.2f;

	//plotter.plot(lnlist, points, 0x0000ff00);

	std::cout << lnlist.isConvex(points);

    points[8].y -= 0.2f;

	std::vector<Triangle> tris;
	triangulate(points, lnlist, tris);
    plotter.plot(tris, points, 0x0000ffff);


	std::vector<PointTriangle> ptris;

	//sphere(1, 16, 10, ptris);

    std::cout << ptris.size();

	std::vector<PointLine> plines;

    plotter.plot(ptris, 0x00ff00ff);
//  plotter.plot(plines, 0x00ff00ff);

	plotter.close();
}

*/