#ifndef _DRAWLIST_H
#define _DRAWLIST_H

#include "core/core.h"
#include "render/RenderStates.h"

#include <vector>

class RenderContext;
class OverheadCamera;
class DeviceModel;
class Texture;
class Material;
class IDrawable;

class DrawItem
{
public:
    
    Matrix              transform;
    IDrawable          *drawable;
    Texture            *texture;
    const RenderStates *states;
    const Material     *material;
    float               transparentz;
    
    DrawItem() : drawable(0), texture(0), states(0), material(0), transparentz(1.0f) {}
    DrawItem(const Matrix &transform, IDrawable *drawable, Texture *texture, const RenderStates *states, const Material *material, float transparentz = 1.0f) 
        : transform(transform), drawable(drawable), texture(texture), states(states), material(material), transparentz(transparentz) {}
};

class DrawList
{    
public:
    
    void Add(const DrawItem &item);
    void Clear();
    void Render(RenderContext &context);

private:

    std::vector<DrawItem> list;
    
    static DrawList *instance;
};

#endif // _DRAWLIST_H