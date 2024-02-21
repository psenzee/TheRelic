#pragma warning(disable: 4786)

#include "netgenreader.h"
#include "simplemesh.h"
#include "io/linereader.h"

#include <stdlib.h>
#include <stdio.h>

void NetGenReader::write(const SimpleMesh &mesh, const char *filename)
{
    FILE *fp = fopen(filename, "w");
	int i = 0, sz = mesh.vertices.size();
	fprintf(fp, "surfacemesh\n%d\n", sz);
	for (; i < sz; i++)
	{
        fprintf(fp, "%10f %10f %10f\n",
		    (float)mesh.vertices[i].x,
			(float)mesh.vertices[i].y,
			(float)mesh.vertices[i].z);
	}
	i = 0; sz = mesh.triangles.size();
	fprintf(fp, "%d\n", sz);
	for (; i < sz; i++)
	{
        fprintf(fp, " %7d %7d %7d\n",
		    mesh.triangles[i].pa,
			mesh.triangles[i].pb,
			mesh.triangles[i].pc);
	}
	fclose(fp);
}

void NetGenReader::read(SimpleMesh &mesh, const char *filename)
{
    LineReader lr(filename);
    int        count = 0;
    while (lr.next() != NULL)
    {
	    if (lr.starts("surfaceelementsgi"))
	    {
		    lr.next(); 
		    count = lr.integer();
			mesh.triangles.clear();
		    for (int i = 0; i < count; i++)
		    {
		        lr.next();
		        mesh.triangles.push_back(
				    Triangle(lr.integer(41) - 1, 
					         lr.integer(49) - 1, 
							 lr.integer(57) - 1));
		    }
	    }
	    else if (lr.starts("points"))
	    {
		    lr.next(); 
		    count = lr.integer();
			mesh.vertices.clear();
		    for (int i = 0; i < count; i++)
		    {
		        lr.next();
		        mesh.vertices.push_back(
				    Point(
					    lr.real(3), 
						lr.real(26), 
						lr.real(50)));
		    }
	    }
	    else if (lr.starts("surfacemesh"))
	    {
		    lr.next(); 
		    count = lr.integer();
			int i = 0;
			mesh.triangles.clear();
			mesh.vertices.clear();
		    for (i = 0; i < count; i++)
		    {
		        lr.next();
		        mesh.vertices.push_back(
				    Point(
					    lr.real(0), 
						lr.real(12), 
						lr.real(23)));
		    }
			lr.next();
		    count = lr.integer();
		    for (i = 0; i < count; i++)
		    {
		        lr.next();
		        mesh.triangles.push_back(
				    Triangle(lr.integer(0)  - 1, 
					         lr.integer(10) - 1, 
							 lr.integer(18) - 1));
		    }
	    }
    }
}