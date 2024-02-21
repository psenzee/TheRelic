#ifndef _ASCWRITER_H
#define _ASCWRITER_H

#include <iostream>

class SimpleMesh;

class AscWriter
{
public:

    static void write(const SimpleMesh &mesh, const char *filename);
	static void write(const SimpleMesh &mesh, std::ostream &s);
};

#endif  /** _ASCWRITER_H */