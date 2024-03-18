#include "render/GLIncludes.h"

#include <cstring>
#include <vector>
#include <cstdio>
#include <iostream>

const char *glErrorString(int error)
{
    switch (error)
    {
    case GL_NO_ERROR:          return "GL_NO_ERROR";
    case GL_INVALID_ENUM:      return "GL_INVALID_ENUM";
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
    case GL_INVALID_VALUE:     return "GL_INVALID_VALUE";
    case GL_OUT_OF_MEMORY:     return "GL_OUT_OF_MEMORY";
    default:                   return "GL_NO_ERROR";
    }
    return "<unknown>";
}

void _glError(const char *function, const char *file, int line)
{
    GLenum error = glGetError();
    if (error == GL_NO_ERROR) {
        return;
    }
    const char *str = (const char *)glErrorString(error);
    char buf[2048] = "";
    size_t end = snprintf(buf, sizeof(buf) - 1, "\nOpenGL error in %s at line %d calling %s: ", file, line, function),
           size = sizeof(buf) - end - 1;
    char *p = buf + end;
    if (str) { std::snprintf(p, size, "'%s'", str); }
    else     { std::snprintf(p, size, "'%d 0x%X'", error, error); }
    std::cerr << buf << std::endl;
}

