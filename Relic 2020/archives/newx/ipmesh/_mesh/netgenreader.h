#ifndef _NETGENREADER_H
#define _NETGENREADER_H

class SimpleMesh;

class NetGenReader
{
public:

    static void read(SimpleMesh &mesh, const char *filename);
	static void write(const SimpleMesh &mesh, const char *filename);
};

#endif  /** _NETGENREADER_H */