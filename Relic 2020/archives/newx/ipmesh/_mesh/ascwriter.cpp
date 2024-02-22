#pragma warning(disable: 4786)

#include "ascwriter.h"
#include "simplemesh.h"

#include <vector>
#include <iostream>
#include <fstream>

void AscWriter::write(const SimpleMesh &mesh, const char *filename)
{
    std::ofstream out(filename);
    write(mesh, out);
    out.close();
}

void AscWriter::write(const SimpleMesh &mesh, std::ostream &s)
{
    s << "Ambient light color: Red=0.3 Green=0.3 Blue=0.3" << std::endl <<
         "Named object: \"Mesh0\""                         << std::endl <<
         "Tri-mesh, Vertices: " << mesh.vertices.size() << 
         " Faces: " << mesh.triangles.size()           << std::endl <<
         "Vertex list:"                                    << std::endl;
    int i = 0, sz = mesh.vertices.size();
	for (; i < sz; i++)
	    s << "Vertex " << i << ":  X:" << mesh.vertices[i].x << " Y:" << 
		     mesh.vertices[i].y << " Z:" << mesh.vertices[i].z << " U:0 V:0" << std::endl;
    sz = mesh.triangles.size();
    for (i = 0; i < sz; i++)
        s << "Face list:" << std::endl << 
	        "Face " << i << ":     A:" << mesh.triangles[i].pa << " B:" << 
			mesh.triangles[i].pb << " C:" << mesh.triangles[i].pc << 
		    " AB:1 BC:1 CA:1" << std::endl << "Smoothing: 1" << std::endl;
}
