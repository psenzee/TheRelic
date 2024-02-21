/*
 *  DrawList.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/2/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _DRAWLIST_H
#define _DRAWLIST_H

#include "core/xna.h"
#include "RenderStates.h"

#include <vector>

class GraphicsDevice;
class OverheadCamera;
class DeviceModel;
class Texture;

class DrawItem
{
public:
    
    Matrix         transform;
    DeviceModel   *model;
    Texture       *texture;
    RenderStates   states;
    float          transparentz;
    
    DrawItem() : model(0), texture(0), transparentz(1.0f) {}
};

class DrawList
{    
    std::vector<DrawItem> list;
    
    static DrawList *instance;
    
public:
    
    void Add(const Matrix &transform, DeviceModel *model, Texture *texture, const RenderStates &states, float transparentz);
    void Clear();
    void Render(GraphicsDevice &device, const OverheadCamera &camera);
    
    static DrawList *GetDrawList();
};

#endif // _DRAWLIST_H