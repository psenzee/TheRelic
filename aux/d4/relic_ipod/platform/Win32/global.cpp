#include "core/global.h"
#include "core/file.h"

#include <stdio.h>

Profiler  *profiler = 0;
GameState *state    = 0;

//#define GLOBAL_PATH_ROOT "_data"
#define GLOBAL_PATH_ROOT "_configs"

float _g_inputSensitivity = 100.0f;
float _g_tiltFactor       = 1.0f;

const char *_g_readOnlyPath  = GLOBAL_PATH_ROOT;
const char *_g_readWritePath = GLOBAL_PATH_ROOT "/save";

extern "C" 
{

static const char *makePath(const char *path, const char *filename)
{
    static char complete[1024];
    sprintf(complete, "%s/%s", path, filename);
    return complete;
}

const char *globalTranslatePath(const char *filename)
{
    return makePath(_g_readOnlyPath, filename);
}

const char *getGlobalReadOnlyPath()
{
    return _g_readOnlyPath;
}

const char *globalTranslateReadWritePath(const char *filename)
{
    return makePath(_g_readWritePath, filename);
}

const char *getGlobalReadWritePath()
{
    return _g_readOnlyPath;
}

const char *globalReadTextFile(const char *filename)
{
    file::buffer_t b = file::read_file(globalTranslatePath(filename));
    if (!b.data)
    {
        printf("Unable to load text file '%s'\n", filename);
        return 0;
    }
    printf("Loading text file '%s' and leaking!!\n", filename);
    return (const char *)b.data;
}

}
