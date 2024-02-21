#ifndef _GLOBAL_H
#define _GLOBAL_H

//#define _DEVELOPMENT_        1
//#define LITE_VERSION         1
//#define IPHONE_PORTRAIT 1
#define SHOW_INTRO_TUTORIAL  1
#define SHOW_TUTORIALS       1
#define NETWORK              1

#define OPENGLMESH_INCLUDE   "OpenGLESMeshInterleaved.h"
#define OPENGLMESH_EXTENSION "ipi"

class OpenGLESMeshInterleaved;

typedef OpenGLESMeshInterleaved OpenGLMeshType;

typedef int (*global_error_handler_t)(const char *format, ...);

#define globalErrorHandler printf

#ifdef __cplusplus
extern "C"
{
#endif

const char *globalReadTextFile(const char *);
const char *globalTranslatePath(const char *);
const char *globalTranslateReadWritePath(const char *);
const char *getGlobalReadOnlyPath();
const char *getGlobalReadWritePath();

#ifdef __cplusplus
}
#endif

class Profiler;
class GameState;

extern Profiler  *profiler;
extern GameState *state;

extern float _g_tiltFactor;
extern float _g_inputSensitivity;

#endif // _GLOBAL_H