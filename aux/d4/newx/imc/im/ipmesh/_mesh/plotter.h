#ifndef _PLOTTER_H
#define _PLOTTER_H

#include "line.h"
#include "plane.h"
#include "point.h"
#include "pointline.h"
#include "triangle.h"
#include "simplemesh.h"
#include "triangleidlist.h"

#include <iostream>
#include <vector>

#include "image/gdi/gdidisplay.h"
#include "image/template/rasterlinet.h"

class Plotter
{
    GdiDisplay *m_disp;

    float sx, sy, tx, ty,
	      width, height;

    inline Point m_transform(const Point &p)
	{
        return Point(width  / 2 +  p.y * sx + tx,
		             height / 2 + -p.z * sy + ty,
					 height / 2 + -p.x * sy + ty);
	}

public:

    Plotter() : m_disp(NULL), width(0), height(0), sx(20), sy(20), tx(10), ty(10) {}

    inline void open(int color)
	{
	    m_disp = new GdiDisplay(color);
		width  = (float)m_disp->width();
		height = (float)m_disp->height();
    }
	inline void close(void)
	{
	    if (m_disp != NULL) delete m_disp;
		m_disp = NULL;
	}
	~Plotter() { close(); }

    void plot(const Line &line, const std::vector<Point> &points, int color)
    {
        plot(points[line.pa], points[line.pb], color);
    }
	bool clippable(const Point &p)
	{
        return (p.x < 0 || p.y < 0 || p.x >= width || p.y >= height);
	}
    void plot(const Point &a, const Point &b, int color)
    {
	    Point pa(m_transform(a)),
		      pb(m_transform(b));
	    if (pa == pb)
		    return;
		if (!clippable(pa) || !clippable(pb))
            RasterLineT<int, GdiDisplay>::line((int)pa.x, (int)pa.y, (int)pb.x, (int)pb.y, color, *m_disp);
    }
    void plot(const PointLine &line, int color)
    {
	    plot(line.a, line.b, color);
    }
    void plot(const Point &u, int size, int color)
    {
	    Point p(m_transform(u));
        size /= 2;
        for (int i = -size, is = size; i < is; i++)
	        for (int j = -size, js = size; j < js; j++)
                m_disp->setAt((int)p.x + i, (int)p.y + j, color);
    }
    void point(const Point &u, int size, int color)
    {
        plot(u, size + 2, 0x00000000);
	    plot(u, size,     color);
    }
    void plot(const PointTriangle &t, int color, bool drawpoints = false)
    {
        plot(PointLine(t.a, t.b), color);
	    plot(PointLine(t.b, t.c), color);
	    plot(PointLine(t.c, t.a), color);
        if (drawpoints)
		{
	        point(t.a, 4, 0x00ff0000); // b
	        point(t.b, 4, 0x0000ff00); // g
	        point(t.c, 4, 0x000000ff); // r
		}
    }
    void plot(const Triangle &t, const std::vector<Point> &points, int color, bool drawpoints = false)
    {
        if (t.degenerate(points))
		    return;

        plot(Line(t.pa, t.pb), points, color);
	    plot(Line(t.pb, t.pc), points, color);
	    plot(Line(t.pc, t.pa), points, color);
        if (drawpoints)
		{
	        point(points[t.pa], 4, 0x00ff0000); // b
	        point(points[t.pb], 4, 0x0000ff00); // g
	        point(points[t.pc], 4, 0x000000ff); // r
		}
    }
    void plot(const SimpleMesh &mesh, const Triangle &t, int color, bool drawpoints = false)
    {
        plot(t, mesh.vertices, color, drawpoints);
    }
    void plot(const SimpleMesh &mesh, const TriangleIdList &t, int color = -1, bool drawpoints = false)
    {
        static int colors[4] = { 0x0000ff00, 0x00ff0000, 0x000000ff, 0x00ff00ff };
        for (int i = 0, sz = t.size(); i < sz; i++)
            plot(mesh, mesh.triangles[t.list[i]], color == -1 ? colors[i % 4] : color, drawpoints);
    }
    void plot(const SimpleMesh &mesh, const std::vector<Triangle> &t, int color = -1, bool drawpoints = false)
    {
        static int colors[4] = { 0x0000ff00, 0x00ff0000, 0x000000ff, 0x00ff00ff };
        for (int i = 0, sz = t.size(); i < sz; i++)
            plot(mesh, t[i], color == -1 ? colors[i % 4] : color, drawpoints);
    }
    void plot(const std::vector<Triangle> &t, const std::vector<Point> &points, int color = -1, bool drawpoints = false)
    {
        static int colors[4] = { 0x0000ff00, 0x00ff0000, 0x000000ff, 0x00ff00ff };
        for (int i = 0, sz = t.size(); i < sz; i++)
            plot(t[i], points, color == -1 ? colors[i % 4] : color, drawpoints);
    }
    void plot(const std::vector<PointTriangle> &t, int color = -1, bool drawpoints = false)
    {
        static int colors[4] = { 0x0000ff00, 0x00ff0000, 0x000000ff, 0x00ff00ff };
        for (int i = 0, sz = t.size(); i < sz; i++)
            plot(t[i], color == -1 ? colors[i % 4] : color, drawpoints);
    }
    void plot(const SimpleMesh &mesh, int color = -1, bool drawpoints = false)
    {
        static int colors[4] = { 0x0000ff00, 0x00ff0000, 0x000000ff, 0x00ff00ff };
        for (int i = 0, sz = mesh.triangles.size(); i < sz; i++)
            plot(mesh, mesh.triangles[i], color == -1 ? colors[i % 4] : color, drawpoints);
    }
	void plot(const LineList &lines, const std::vector<Point> &points, int color)
	{
	    if (lines.size() > 0)
		{
            for (int i = 0, sz = lines.size(); i < sz; i++)
		    {
	            const Point &a = points[lines.list[i]],
		                    &b = (i != sz - 1) ? points[lines.list[i + 1]] : points[lines.list[0]];
		        plot(a, b, color);
		    }
		}
	}
	void plot(std::vector<LineList> &polys, const std::vector<Point> &points, int color)
	{
	    for (std::vector<LineList>::iterator it = polys.begin(), end = polys.end(); it != end; ++it)
		    plot(*it, points, color);
	}
	void plot(std::vector<PointLine> &lines, int color)
	{
	    for (std::vector<PointLine>::iterator it = lines.begin(), end = lines.end(); it != end; ++it)
		    plot(*it, color);
	}
};

#endif  /** _PLOTTER_H */