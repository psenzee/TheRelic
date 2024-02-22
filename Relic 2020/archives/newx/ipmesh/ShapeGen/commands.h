#ifndef _COMMANDS_H
#define _COMMANDS_H

#include "types/tuple3t.h"
#include "types/tuple2t.h"

int read(int argc, int start, const char **args, float *v, int expected = 1);
int read(int argc, int start, const char **args, int   *v, int expected = 1);
int read(int argc, int start, const char **args, Tuple3T<float> &vector);
int read(int argc, int start, const char **args, Tuple2T<float> &vector);

#endif  // _COMMANDS_H