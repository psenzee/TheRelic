#include "shapes.h"
#include "faces.h"
#include "uvmap.h"
#include "types/tuple2t.h"
#include "types/tuple3t.h"
#include "types/matrix4t.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

void transform(const Matrix4T<float> &m, const Matrix4T<float> &it, Vertex &v)
{
    v.position = m.point(v.position);
    v.normal   = it.vector(v.normal).normal();
}

void transform(const Matrix4T<float> &m, const Matrix4T<float> &it, Face &face)
{
    for (std::vector<Vertex>::iterator i = face.vertices.begin(), e = face.vertices.end(); i != e; ++i)
        transform(m, it, (*i));
}

void transform(const Matrix4T<float> &m, Face &face)
{
    Matrix4T<float> it(m.invert(true));
    transform(m, it, face);
}

void transform(const Matrix4T<float> &m, const Matrix4T<float> &it, FaceList &faces)
{
    for (std::vector<Face>::iterator i = faces.faces.begin(), e = faces.faces.end(); i != e; ++i)
        transform(m, it, *i);
}

void transform(const Matrix4T<float> &m, FaceList &faces)
{
    Matrix4T<float> it(m.invert(true));
    transform(m, it, faces);
}

void superquadric(FaceList &faces, const SuperQuadricOptions &options)
{
    superquadric(faces, options.ner.x, options.ner.y, options.ner.z, options.increment.x, options.increment.y);
}

void sphere(FaceList &faces, const SphereOptions &options)
{
    sphere(faces, options.ccw, options.levels);
}

// these aliases allow us to template some reading code
void create(FaceList &faces, const SphereOptions &options)
{
    sphere(faces, options);
}

void create(FaceList &faces, const SuperQuadricOptions &options)
{
    superquadric(faces, options);
}

bool WRITE_VERTICES = false;

// superquadric: (1, 1) = sphere; (1, .5) = pillow; (1, 0.1, 1.5) = squarish torus

static void print(FILE *file, const Vertex &v)
{
    if (WRITE_VERTICES)
    {
        const Tuple3T<float> &normal = v.getnormal(),
                             &p      = v.getposition();
        const Tuple2T<float> &uv     = v.getuv(0);
        fprintf(file, "p (%6.4f, %6.4f, %6.4f)\t\t\tn (%6.4f, %6.4f, %6.4f)\t\t\tuv (%6.4f, %6.4f)\n", 
                p.x, p.y, p.z, normal.x, normal.y, normal.z, uv.x, uv.y);
    }
}

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

Vertex superquadric_vertex(double u, double v, double n, double e, double r)
{
    Vertex vertex;
    vertex.setposition(superquadric_point (u, v, n, e, r));
    vertex.setnormal  (superquadric_normal(u, v, n, e, r));
    // don't yet know what to do with uv(s)
    return vertex;
}

void superquadric(FaceList &faces, double n, double e, double r, double incv, double incu)
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

// sphere

/*% cc -g sphere.c -o sphere -lm
 *
 * sphere - generate a triangle mesh approximating a sphere by
 *  recursive subdivision. First approximation is an octahedron;
 *  each level of refinement increases the number of triangles by
 *  a factor of 4.
 * Level 3 (128 triangles) is a good tradeoff if gouraud
 *  shading is used to render the database.
 *
 * Usage: sphere [level] [-p] [-c]
 *	level is an integer >= 1 setting the recursion level (default 1).
 *	-p causes generation of a PPHIGS format ASCII archive
 *	    instead of the default generic output format.
 *	-c causes triangles to be generated with vertices in counterclockwise
 *	    order as viewed from the outside in a RHS coordinate system.
 *	    The default is clockwise order.
 *
 *  The subroutines print_object() and print_triangle() should
 *  be changed to generate whatever the desired database format is.
 *
 * Jon Leech (leech@cs.unc.edu) 3/24/89
 */

struct point
{
    double  x, y, z;
};

struct triangle
{
    point     pt[3];	/* Vertices of triangle */
    double    area;	/* Unused; might be used for adaptive subdivision */
};

struct object
{
    int       npoly;	/* # of triangles in object */
    triangle *poly;	/* Triangles */
};

object *create_sphere(int ccwflag, int maxlevel);

//

object *create_sphere(int ccwflag, int maxlevel);

static Face create_sphere_triangle_face(FILE *file, const triangle *t)
{
    Face face;
    Vertex v;
    for (int i = 0; i < 3; i++)
    {
        Tuple3T<float> p((float)t->pt[i].x, (float)t->pt[i].y, (float)t->pt[i].z);
        v.setposition(p);
        v.setnormal(p);
        v.setuv(0, map_unit_sphere(p));
        print(file, v);
        face.add(v);
    }
    return face;
}

void sphere(FaceList &faces, bool ccw, int maxlevel)
{
    FILE *file = 0;
    if (WRITE_VERTICES)
        file = fopen("vertices.txt", "w");
    object *sphere = create_sphere(ccw ? 1 : 0, maxlevel);
    Face face;
    for (int i = 0; i < sphere->npoly; i++)
        faces.add(create_sphere_triangle_face(file, &(sphere->poly[i])));
    if (WRITE_VERTICES)
        fclose(file);
}

//

/*% cc -g sphere.c -o sphere -lm
 *
 * sphere - generate a triangle mesh approximating a sphere by
 *  recursive subdivision. First approximation is an octahedron;
 *  each level of refinement increases the number of triangles by
 *  a factor of 4.
 * Level 3 (128 triangles) is a good tradeoff if gouraud
 *  shading is used to render the database.
 *
 * Usage: sphere [level] [-p] [-c]
 *	level is an integer >= 1 setting the recursion level (default 1).
 *	-p causes generation of a PPHIGS format ASCII archive
 *	    instead of the default generic output format.
 *	-c causes triangles to be generated with vertices in counterclockwise
 *	    order as viewed from the outside in a RHS coordinate system.
 *	    The default is clockwise order.
 *
 *  The subroutines print_object() and print_triangle() should
 *  be changed to generate whatever the desired database format is.
 *
 * Jon Leech (leech@cs.unc.edu) 3/24/89
 */

/* Six equidistant points lying on the unit sphere */
#define XPLUS {  1,  0,  0 }	/*  X */
#define XMIN  { -1,  0,  0 }	/* -X */
#define YPLUS {  0,  1,  0 }	/*  Y */
#define YMIN  {  0, -1,  0 }	/* -Y */
#define ZPLUS {  0,  0,  1 }	/*  Z */
#define ZMIN  {  0,  0, -1 }	/* -Z */

/* Vertices of a unit octahedron */
triangle octahedron[] =
{
    { { XPLUS, ZPLUS, YPLUS }, 0.0 },
    { { YPLUS, ZPLUS, XMIN  }, 0.0 },
    { { XMIN , ZPLUS, YMIN  }, 0.0 },
    { { YMIN , ZPLUS, XPLUS }, 0.0 },
    { { XPLUS, YPLUS, ZMIN  }, 0.0 },
    { { YPLUS, XMIN , ZMIN  }, 0.0 },
    { { XMIN , YMIN , ZMIN  }, 0.0 },
    { { YMIN , XPLUS, ZMIN  }, 0.0 }
};

/* A unit octahedron */
object oct =
{
    sizeof(octahedron) / sizeof(octahedron[0]),
    &octahedron[0]
};

/* Forward declarations */

point *normalize(point *p);
point *midpoint(point *a, point *b);

/* Normalize a point p */
point *normalize(point *p)
{
    static point r;
    double mag;

    r = *p;
    mag = r.x * r.x + r.y * r.y + r.z * r.z;
    if (mag != 0.0)
    {
	    mag = 1.0 / sqrt(mag);
	    r.x *= mag;
	    r.y *= mag;
	    r.z *= mag;
    }
    return &r;
}

/* Return the midpoint on the line between two points */
point *midpoint(point *a, point *b)
{
    static point r;

    r.x = (a->x + b->x) * 0.5;
    r.y = (a->y + b->y) * 0.5;
    r.z = (a->z + b->z) * 0.5;

    return &r;
}

object *create_sphere(int ccwflag, int maxlevel)
{
    object *oold = NULL, *onew = NULL;
    int     i = 0, level = 0;		/* Current subdivision level */

    if (ccwflag)
    {
	    /* Reverse order of points in each triangle */
	    for (i = 0; i < oct.npoly; i++)
        {
	        point tmp;
            tmp = oct.poly[i].pt[0];
	        oct.poly[i].pt[0] = oct.poly[i].pt[2];
	        oct.poly[i].pt[2] = tmp;
	    }
    }

    oold = &oct;

    /* Subdivide each starting triangle (maxlevel - 1) times */
    for (level = 1; level < maxlevel; level++)
    {
	    /* Allocate a new object */
	    onew = (object *)malloc(sizeof(object));
	    if (onew == NULL)
        {
            fprintf(stderr, "create_sphere: Out of memory on subdivision level %d\n",
		                    level);
	        return 0;
	    }
	    onew->npoly = oold->npoly * 4;

	    /* Allocate 4* the number of points in the current approximation */
	    onew->poly  = (triangle *)malloc(onew->npoly * sizeof(triangle));
	    if (onew->poly == NULL)
        {
	        fprintf(stderr, "create_sphere: Out of memory on subdivision level %d\n",
		                    level);
	        return 0;
	    }

	    /* Subdivide each triangle in the old approximation and normalize
	    *  the new points thus generated to lie on the surface of the unit
	    *  sphere.
	    * Each input triangle with vertices labelled [0,1,2] as shown
	    *  below will be turned into four new triangles:
	    *
	    *			Make new points
	    *			    a = (0+2)/2
	    *			    b = (0+1)/2
	    *			    c = (1+2)/2
	    *	  1
	    *	 /\		Normalize a, b, c
	    *	/  \
	    *    b/____\ c		Construct new triangles
	    *    /\    /\		    [0,b,a]
	    *   /	\  /  \		    [b,1,c]
	    *  /____\/____\	    [a,b,c]
	    * 0	  a	2	    [a,c,2]
	    */
	    for (i = 0; i < oold->npoly; i++)
        {
	        triangle *oldt = &oold->poly[i],
		             *newt = &onew->poly[i*4];
	        point a, b, c;

	        a = *normalize(midpoint(&oldt->pt[0], &oldt->pt[2]));
	        b = *normalize(midpoint(&oldt->pt[0], &oldt->pt[1]));
	        c = *normalize(midpoint(&oldt->pt[1], &oldt->pt[2]));

	        newt->pt[0] = oldt->pt[0];
	        newt->pt[1] = b;
	        newt->pt[2] = a;
	        newt++;

	        newt->pt[0] = b;
	        newt->pt[1] = oldt->pt[1];
	        newt->pt[2] = c;
	        newt++;

	        newt->pt[0] = a;
	        newt->pt[1] = b;
	        newt->pt[2] = c;
	        newt++;

	        newt->pt[0] = a;
	        newt->pt[1] = c;
	        newt->pt[2] = oldt->pt[2];
	    }

	    if (level > 1)
        {
	        free(oold->poly);
	        free(oold);
	    }

	    /* Continue subdividing new triangles */
	    oold = onew;
    }

    return oold;
}