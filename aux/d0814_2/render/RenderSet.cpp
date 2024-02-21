#include "RenderSet.h"
#include "DrawList.h"
#include "core/Profiler.h"
#include "RenderContext.h"
#include "GraphicsDevice.h"

RenderSet *RenderSet::sInstance = 0;
Profiler  *RenderSet::sProfiler = 0;

RenderSet *RenderSet::GetInstance()
{
    if (!sInstance)
        sInstance = new RenderSet;
    return sInstance;
}

void RenderSet::SetProfiler(Profiler *profiler)
{
    sProfiler = profiler;    
}

void RenderSet::Add(const String &name, const DrawItem &item)
{
    DrawList *list = mMap[name];
    if (!list)
        mMap[name] = list = new DrawList;
    list->Add(item);
}

void RenderSet::Render(const String &name, RenderContext &context)
{
    DrawList *list = mMap[name];
    if (list)
    {
        if (sProfiler)
            sProfiler->StartTime(name.c_str());
        GraphicsDevice::GetInstance()->SetColor(Vector4(1.f, 1.f, 1.f, 1.f)); // clear color
        list->Render(context);
        list->Clear();
        if (sProfiler)
            sProfiler->EndTime(name.c_str());            
    }
}