#ifndef _MESHFILE_H
#define _MESHFILE_H

#include "faces.h"

bool read(const char *filename, FaceList &faces);
bool read_pov(const char *filename, FaceList &faces);

#endif // _MESHFILE_H