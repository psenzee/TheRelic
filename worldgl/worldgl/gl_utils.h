#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void _oglError(const char *function, const char *file, int line);

#define _GL(x) (x); _oglError(#x, __FILE__, __LINE__);
