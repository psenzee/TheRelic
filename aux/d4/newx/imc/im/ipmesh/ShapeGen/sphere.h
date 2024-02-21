#ifndef _SPHERE_H
#define _SPHERE_H

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

#endif // _SPHERE_H