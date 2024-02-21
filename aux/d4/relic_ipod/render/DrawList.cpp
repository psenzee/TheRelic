// DrawList.cpp

#include "DrawList.h"
#include "render/GraphicsDevice.h"
#include "render/RenderContext.h"
#include "IDrawable.h"
#include "render/Texture.h"
#include "time/FrameRateTracker.h"

#include <algorithm>

DrawList *DrawList::instance = 0;
/*
void DrawList::Add(const Matrix &transform, IDrawable *drawable, Texture *texture, const RenderStates *states, const Material *material, float transparentz)
{
    DrawItem item(transform, drawable, texture, states, material, transparentz);
    Add(item);
}
*/

void DrawList::Add(const DrawItem &item)
{
    if (item.drawable)
    {
        item.drawable->Retain();
        if (item.texture)
            item.texture->Retain();
        list.push_back(item);
    }
}

inline unsigned round_pow2(unsigned x)
{ 
    --x; 
    x |= (x >> 16); x |= (x >>  8); x |= (x >>  4); x |= (x >>  2); x |= (x >>  1); 
    return ++x;
}

void DrawList::Clear()
{
    for (std::vector<DrawItem>::iterator i = list.begin(), e = list.end(); i != e; ++i)
    {
        (*i).drawable->Release();
        if ((*i).texture)
            (*i).texture->Release();
    }
    unsigned minsz = 128, sz = round_pow2((unsigned)list.size());
    sz = minsz > sz ? minsz : sz;
    list.reserve(sz);
    list.erase(list.begin(), list.end()); // don't clear() cause that invalidates reserve()..
}

class DrawItemComparator
{
public:
    inline bool operator()(const DrawItem &a, const DrawItem &b)
    {
        bool aTrans = a.states ? a.states->Transparent : false,
             bTrans = b.states ? b.states->Transparent : false;
        if (aTrans && aTrans == bTrans)
        {
            if      (a.transparentz > b.transparentz)
                return true;
            else if (a.transparentz < b.transparentz)
                return false;
            return a.drawable < b.drawable;
        }
        else if (aTrans)
            return false;
        else if (bTrans)
            return true;
        else if (a.texture == b.texture)
            return a.drawable < b.drawable;
        return a.texture < b.texture;
        /*
        else if (a.drawable == b.drawable)
            return a.texture < b.texture;
        return a.drawable < b.drawable;
        */
    }
};

void DrawList::Render(RenderContext &context)
{
    DrawItemComparator c;
    std::sort(list.begin(), list.end(), c);
    Texture *texture = 0;
    DrawItem item;
    RenderContext rc(context);
    bool depthTest = true;
    bool depthWrite = true;
    rc.device.EnableDepthTest(depthTest);
    rc.device.EnableDepthWrite(depthWrite);
    for (std::vector<DrawItem>::iterator i = list.begin(), e = list.end(); i != e; ++i)
    {
        item = *i;
        if ((item.states && item.states->Transparent) || item.texture != texture)
        {
            texture = item.texture;
            if (texture)
                texture->Set(context.device);
        }
        if (texture)
        {
            if (item.states)
            {
                item.states->Apply(context.device);
                if (item.states->DepthTest != depthTest)
                    rc.device.EnableDepthTest((depthTest = item.states->DepthTest));
                if (item.states->DepthWrite != depthWrite)
                    rc.device.EnableDepthWrite((depthTest = item.states->DepthWrite));
            }
            if (item.material)
                context.device.SetMaterial(*item.material);
            rc.transform = item.transform;
            item.drawable->RenderImmediate(rc);
            if (item.states)
                item.states->Unapply(context.device);
        }
    }
}