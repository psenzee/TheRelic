/*
 *  DrawList.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/2/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "DrawList.h"
#include "GraphicsDevice.h"
#include "DeviceModel.h"
#include "Texture.h"
#include "time/FrameRateTracker.h"

#include <algorithm>

DrawList *DrawList::instance = 0;

void DrawList::Add(const Matrix &transform, DeviceModel *model, Texture *texture, const RenderStates &states, float transparentz)
{
    DrawItem item;
    item.transform    = transform;
    item.model        = model;
    item.texture      = texture;
    item.states       = states;
    item.transparentz = transparentz;
    list.push_back(item);
}

inline unsigned round_pow2(unsigned x)
{ 
    --x; 
    x |= (x >> 16); x |= (x >>  8); x |= (x >>  4); x |= (x >>  2); x |= (x >>  1); 
    return ++x;
}

void DrawList::Clear()
{
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
        if (a.states.Transparent == b.states.Transparent)
        {
            if (a.transparentz > b.transparentz)
                return true;
            else if (a.transparentz < b.transparentz)
                return false;
            return a.model < b.model;
        }
        else if (a.states.Transparent)
            return false;
        else if (b.states.Transparent)
            return true;
        else if (a.texture == b.texture)
            return a.model < b.model;
        return a.texture < b.texture;
        /*
        else if (a.model == b.model)
            return a.texture < b.texture;
        return a.model < b.model;    
        */
    }
};

//FrameRateTracker dlsort("dlsort", 40), dlrender("dlrender", 40);

void DrawList::Render(GraphicsDevice &device, const OverheadCamera &camera)
{
    DrawItemComparator c;
    //dlsort.StartFrame();
    std::sort(list.begin(), list.end(), c);
    //dlsort.EndFrame();    
    Texture   *texture = 0;
    DrawItem   item;
    //dlrender.StartFrame();
    for (std::vector<DrawItem>::iterator i = list.begin(), e = list.end(); i != e; ++i)
    {
        item = *i;
        if (item.states.Transparent || item.texture != texture)
        {
            texture = item.texture;
            if (texture)
                texture->Set(device);
        }
        if (texture)
        {
        //    item.states.Apply(device);
            item.model->Render(device, item.transform, camera);
            item.states.Unapply(device);            
        }
    }
    //dlrender.EndFrame();
}

DrawList *DrawList::GetDrawList()
{
    if (!instance)
        instance = new DrawList;
    return instance;
}