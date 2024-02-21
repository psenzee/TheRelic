// DrawList.cpp

#include "DrawList.h"
#include "render/GraphicsDevice.h"
#include "render/RenderContext.h"
#include "IDrawable.h"
#include "render/Texture.h"
#include "time/FrameRateTracker.h"

#include <algorithm>

DrawList *DrawList::instance = 0;

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
    inline bool ComparePostTransparency(const DrawItem &a, const DrawItem &b) const
    {
        if (a.states && b.states && a.states->RenderOrder != b.states->RenderOrder)
            return a.states->RenderOrder < b.states->RenderOrder;
        if (a.texture != b.texture)
            return a.texture < b.texture;
        return a.drawable < b.drawable;
    }

//  const GraphicsDevice &mDevice;

public:

//  DrawItemComparator(const GraphicsDevice &device) : mDevice(device) {}

    inline bool operator()(const DrawItem &a, const DrawItem &b) const
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
        return ComparePostTransparency(a, b);
    }
};

void DrawList::Render(RenderContext &context)
{
    DrawItemComparator c;
    std::sort(list.begin(), list.end(), c);
    Texture *texture = 0;
    DrawItem item;
    RenderContext rc(context);

    for (std::vector<DrawItem>::iterator i = list.begin(), e = list.end(); i != e; ++i)
    {
        item = *i;
if (item.breakFunction)
    item.breakFunction();
        if (item.blur && !item.states->Blurrable)
            continue;
        if ((item.states && item.states->Transparent) || item.texture != texture)
        {
            texture = item.texture;
            if (texture)
                texture->Set(context.device);
        }
        if (texture)
        {
            TriState prevFogState = rc.device.GetEnableFog();
            if (item.states)
            {
                item.states->Apply(context.device);
                rc.device.SetUvTransform(item.states->UvTransform);
                rc.device.SetColor(item.states->Color * item.color);
                rc.device.EnableFog(item.states->Fog);
                bool depthTest = rc.depthTest.IsUnknown() ? item.states->DepthTest : rc.depthTest.ToBool();
                rc.device.EnableDepthTest(depthTest);
                bool depthWrite = rc.depthWrite.IsUnknown() ? item.states->DepthWrite : rc.depthWrite.ToBool();
                rc.device.EnableDepthWrite(depthWrite);
                rc.device.EnableCullFace(item.states->CullFace);
            }
            else
            {
                rc.device.SetColor(item.color);
            }
            if (item.material)
            {
                context.device.SetMaterial(*item.material);
            }
            rc.transform = item.transform;
            item.drawable->RenderImmediate(rc);
            if (item.states)
                item.states->Unapply(context.device);
            rc.device.EnableFog(prevFogState);
        }
    }
}