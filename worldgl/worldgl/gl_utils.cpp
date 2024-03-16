#pragma once

#include "gl_utils.h"

#include <cstdio>
#include <iostream>

void _oglError(const char *function, const char *file, int line)
{
    GLenum error = glGetError();
    if (error == GL_NO_ERROR) {
        return;
    }
    const char *str = (const char *)gluErrorString(error);
    char buf[2048] = "";
    size_t end = snprintf(buf, sizeof(buf) - 1, "OpenGL error in %s at line %d calling %s: ", file, line, function),
           size = sizeof(buf) - end - 1;
    char *p = buf + end;
    if (str) { std::snprintf(p, size, "'%s'", str); }
    else     { std::snprintf(p, size, "'%d 0x%X'", error, error); }
    std::cerr << buf << std::endl;
}
