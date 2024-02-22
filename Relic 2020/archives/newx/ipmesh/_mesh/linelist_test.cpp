#include "linelist.h"
#include "point.h"
#include "constants.h"
#include "plotter.h"
#include "triangle.h"

#include <vector>
#include <iostream>

#include <math.h>

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


void circle(double radius, int slices, std::vector<PointLine> &lines)
{
	double slicefactor = PI * 2 / slices, sf0, sf1;
	Point  a, b;

    for (int i = 0; i < slices; i++)
	{
		sf0 = i * slicefactor; sf1 = (i + 1) * slicefactor;
		a.set(0, radius * sin(sf0), radius * cos(sf0));
        b.set(0, radius * sin(sf1), radius * cos(sf1));
        lines.push_back(PointLine(a, b));
	}
}

void circle(double radius, int slices, std::vector<PointTriangle> &triangles)
{
	double slicefactor = PI * 2 / slices, sf0, sf1;
	Point  a, b;

    for (int i = 0; i < slices; i++)
	{
		sf0 = i * slicefactor; sf1 = (i + 1) * slicefactor;
		a.set(0, radius * sin(sf0), radius * cos(sf0));
        b.set(0, radius * sin(sf1), radius * cos(sf1));
        triangles.push_back(PointTriangle(Point(0, 0, 0), a, b));
	}
}

void cube(std::vector<PointTriangle> &tris)
{
    Point llb(0, 0, 0), lrb(1, 0, 0),
	      llf(0, 0, 1), lrf(1, 0, 1),
		  ulb(0, 1, 0), urb(1, 1, 0),
	      ulf(0, 1, 1), urf(1, 1, 1);

    tris.push_back(PointTriangle(lrf, llf, llb)); // lower face..
    tris.push_back(PointTriangle(lrf, llb, lrb));

    tris.push_back(PointTriangle(ulb, ulf, urf)); // upper face..
    tris.push_back(PointTriangle(urb, ulb, urf));

    tris.push_back(PointTriangle(lrf, lrb, urf)); // right face..
    tris.push_back(PointTriangle(urb, lrb, urf));

    tris.push_back(PointTriangle(ulf, llb, llf)); // left face..
    tris.push_back(PointTriangle(ulf, llb, ulb));

    tris.push_back(PointTriangle(urf, ulf, llf)); // front face..
    tris.push_back(PointTriangle(urf, lrf, llf));

    tris.push_back(PointTriangle(llb, ulb, urb)); // back face..
    tris.push_back(PointTriangle(llb, lrb, urb));
}

void extrude(int slices, Point direction, const std::vector<PointLine> &lines, std::vector<PointTriangle> &triangles)
{
    double dx0, dx1, dy0, dy1, dz0, dz1;
	Point a, b, c, d;
    for (int i = 0; i < slices; i++)
	{
	    dx0 = direction.x * i; dx1 = direction.x * (i + 1);
		dy0 = direction.y * i; dy1 = direction.y * (i + 1);
		dz0 = direction.z * i; dz1 = direction.z * (i + 1);
		for (int j = 0, sz = lines.size(); j < sz; j++)
		{
		    a.set(lines[j].a.x + dx0, lines[j].a.y + dy0, lines[j].a.z + dz0);
			b.set(lines[j].b.x + dx0, lines[j].b.y + dy0, lines[j].b.z + dz0);
			c.set(lines[j].a.x + dx1, lines[j].a.y + dy1, lines[j].a.z + dz1);
			d.set(lines[j].b.x + dx1, lines[j].b.y + dy1, lines[j].b.z + dz1);
            triangles.push_back(PointTriangle(a, b, c));
			triangles.push_back(PointTriangle(c, d, a));
		}
	}
}

void onecone(double radius, Point location, int slices, int rows, std::vector<PointTriangle> &triangles)
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

void twocones(double radius, Point location, int slices, int rows, std::vector<PointTriangle> &triangles)
{
	double slicefactor = PI / slices,
	       rowfactor   = PI / rows,
		   r0, r1, sf0, sf1;
	Point  a, b, c, d;

    for (int j = 0; j < rows; j++)
	{
	    r0 = cos(j       * rowfactor) * radius;
		r1 = cos((j + 1) * rowfactor) * radius;
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

    points[8].y -= 0.1;

	//plotter.plot(lnlist, points, 0x000000ff);

	std::cout << lnlist.isConvex(points);

    points[8].y += 0.2;

	//plotter.plot(lnlist, points, 0x0000ff00);

	std::cout << lnlist.isConvex(points);

    points[8].y -= 0.2;

	std::vector<Triangle> tris;
	triangulate(points, lnlist, tris);
    plotter.plot(tris, points, 0x0000ffff);


	std::vector<PointTriangle> ptris;

	//sphere(1, 16, 10, ptris);

    std::cout << ptris.size();

	std::vector<PointLine> plines;

	circle(1, 64, plines);
	circle(1, 64, ptris);
	extrude(3, Point(0.1, 0.23, 0.4), plines, ptris);

    plotter.plot(ptris, 0x00ff00ff);
//  plotter.plot(plines, 0x00ff00ff);

	plotter.close();
}