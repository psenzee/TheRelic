#include "commands.h"

#include <stdio.h>
#include <string.h>
#include "types/tuple3t.h"
#include "types/tuple2t.h"
#include "strs.h"

template <typename T>
int read_type(int argc, int start, const char **args, T *v, int expected)
{
    int i = start;
    for (int count = 0; i < argc && count < expected; i++, count++)
    {
        const char *s = args[i];
        T value = T(0);
        if (!read(&s, value))
            break;
        for (int j = count; j < expected; j++)
            v[j] = value;
    }
    i--;
    return i;
}

int read(int argc, int start, const char **args, float *v, int expected)
{
    return read_type(argc, start, args, v, expected);
}

int read(int argc, int start, const char **args, int *v, int expected)
{
    return read_type(argc, start, args, v, expected);
}

int read(int argc, int start, const char **args, Tuple3T<float> &vector)
{
    float v[3] = { 0.0f, 0.0f, 0.0f };
    int at = read(argc, start, args, v, 3);
    vector = Tuple3T<float>(v[0], v[1], v[2]);
    return at;
}

int read(int argc, int start, const char **args, Tuple2T<float> &vector)
{
    float v[2] = { 0.0f, 0.0f };
    int at = read(argc, start, args, v, 2);
    vector = Tuple2T<float>(v[0], v[1]);
    return at;
}
