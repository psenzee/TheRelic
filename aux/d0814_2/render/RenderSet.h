#ifndef _RENDERSET_H
#define _RENDERSET_H

#include "core/core.h"
#include "fast/Allocator.h"

#include <map>

class Profiler;
class RenderContext;
class DrawItem;
class DrawList;

class RenderSet
{
public:
    
    CLASS_NEW_DELETE()

    void              Add(const String &name, const DrawItem &item);

    void              Render(const String &name, RenderContext &context);
    
    static RenderSet *GetInstance();
    static void       SetProfiler(Profiler *profiler);
    
private:

    static RenderSet *sInstance;
    static Profiler  *sProfiler;

    std::map<String, DrawList *> mMap;
};

#endif // _RENDERSET_H