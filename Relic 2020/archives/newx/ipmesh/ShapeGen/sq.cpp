#include <stdio.h>
#include <math.h>
#include <vector>
#include <deque>

#include "objfile.h"
#include "types/tuple3t.h"

double point_c(double v, double n, double r)
{
    double cv = cos(v), cpv = pow(abs(cv), n);
	return r + ((cv >= 0. ? 1. : -1.) * cpv);
}

double point_s(double v, double n)
{
    double sv = sin(v), spv = pow(abs(sv), n);
	return (sv >= 0. ? 1. : -1.) * spv;
}

Tuple3T<float> superquadric_point(double u, double v, double n, double e, double r)
{
	double cvnr = point_c(v, n, r);
	return Tuple3T<float>(float(cvnr * point_c(u, e, 0.0)),
                          float(cvnr * point_s(u, e)),
                          float(point_s(v, n)));
}

Tuple3T<float> superquadric_normal(double u, double v, double n, double e, double r)
{
    // does not handle toroids
    return Tuple3T<float>(
        // have i got u and v reversed here?
        float(pow(cos(v), 2.0 - n) * pow(cos(u), 2.0 - e)), 
        float(pow(cos(v), 2.0 - n) * pow(sin(u), 2.0 - e)),
        float(pow(sin(v), 2.0 - n)));
}

const double PI = 3.141592653589793;

unsigned sq_point_count(double r, double incv, double incu)
{
    double start = -PI, end = PI;
    if (r < 1.)	{ start /= 2.; end /= 2.; }
    double vdist = end - start, udist = PI - -PI;
    return ((unsigned)(vdist / incv) + 1) * ((unsigned)(udist / incu) + 1) * 4;
}

void sq_points(std::vector< Tuple3T<float> > &points, double n, double e, double r = 0., double incv = 0.05, double incu = 0.05)
{
	double start = -PI, end = PI, iu = incu + 0.0001, iv = incv + 0.0001;
	if (r < 1.)	{ start /= 2.; end /= 2.; }
	for (double v = start; v <= end; v += incv)
    {
	    for (double u = -PI; u <= PI; u += incu)
        {
            points.push_back(superquadric_point(u - iu, v - iv, n, e, r));
            points.push_back(superquadric_point(u,      v - iv, n, e, r));
            points.push_back(superquadric_point(u,      v,      n, e, r));
			points.push_back(superquadric_point(u - iu, v,      n, e, r));
        }
    }
}

Vertex superquadric_vertex(double u, double v, double n, double e, double r)
{
    Vertex vertex;
    vertex.setposition(superquadric_point (u, v, n, e, r));
    vertex.setnormal  (superquadric_normal(u, v, n, e, r));
    // don't yet know what to do with uv(s)
    return vertex;
}

void superquadric(FaceList &faces, double n, double e, double r = 0., double incv = 0.05, double incu = 0.05)
{
	double start = -PI, end = PI, iu = incu + 0.0001, iv = incv + 0.0001;
	if (r < 1.)	{ start /= 2.; end /= 2.; }
    Face face;
	for (double v = start; v <= end; v += incv)
    {
	    for (double u = -PI; u <= PI; u += incu)
        {
            face.clear();
            face.add(superquadric_vertex(u - iu, v - iv, n, e, r));
            face.add(superquadric_vertex(u,      v - iv, n, e, r));
            face.add(superquadric_vertex(u,      v,      n, e, r));
			face.add(superquadric_vertex(u - iu, v,      n, e, r));
            faces.add(face);
        }
    }
}

void write_points_obj(FILE *stream, const std::vector< Tuple3T<float> > &points)
{
    fprintf(stream, "g object\n\ns off\n\n");

    for (size_t i = 0, sz = points.size(); i < sz; i++)
        fprintf(stream, "v %f %f %f\n", points[i].x, points[i].y, points[i].z);

    fprintf(stream, "\n\n");

    for (size_t i = 0, sz = points.size(); i < sz; i += 4)
        fprintf(stream, "f %d %d %d %d\n", i + 1, i + 2, i + 3, i + 4);

    fprintf(stream, "\n#end");
}

void write_points_obj(const char *filename, const std::vector< Tuple3T<float> > &points)
{
    FILE *file = fopen(filename, "wb");
    write_points_obj(file, points);
    fclose(file);
}

#include <conio.h>

void sq_main()
{
	printf("count estimate %u\n", sq_point_count(0., 0.05, 0.05));
	std::vector< Tuple3T<float> > points;
	sq_points(points, 1, 0.1, 1.5); // (1, 1) = sphere; (1, .5) = pillow; (1, 0.1, 1.5) = squarish torus
	printf("count %u\n", points.size());
    write_points_obj("shape.obj", points);
	getch();
}