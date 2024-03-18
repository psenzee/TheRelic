#pragma once

void _glError(const char *function, const char *file, int line);

template <typename T>
inline T gl_error(T return_value, const char *function, const char *file, int line)
{
    GLenum err = 0;
    while((err = glGetError()) != GL_NO_ERROR) ;
    _glError(function, file, line);
    return return_value;
}

#define _GL(x)  gl_error((x), #x, __FILE__, __LINE__)
#define _GLv(x) do { (x); gl_error(0, #x, __FILE__, __LINE__); } while (0)
