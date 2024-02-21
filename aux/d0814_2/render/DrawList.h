#ifndef _DRAWLIST_H
#define _DRAWLIST_H

#include "core/core.h"
#include "render/RenderStates.h"
#include "fast/Allocator.h"

#include <vector>

class RenderContext;
class OverheadCamera;
class DeviceModel;
class Texture;
class Material;
class IDrawable;

typedef void (*breakfunction_t)();

class DrawItem
{
public:
    
  //CLASS_NEW_DELETE() // $TODO can't do this here until we provide an array allocator, REALLY need to do that
    
    Matrix              transform;
    IDrawable          *drawable;
    Texture            *texture;
    const RenderStates *states;
    const Material     *material;
    Vector4             color;
    float               transparentz;
    bool                blur;
    breakfunction_t     breakFunction;
    
    DrawItem() : drawable(0), texture(0), states(0), material(0), transparentz(1.0f), color(1.f, 1.f, 1.f, 1.f), breakFunction(0) {}
    DrawItem(const Matrix &transform, IDrawable *drawable, Texture *texture,
             const RenderStates *states, const Material *material, const Vector4 &color,
             bool blur,
             float transparentz = 1.0f) 
        : transform(transform), drawable(drawable), texture(texture),
          states(states), material(material), color(color), transparentz(transparentz),
          blur(blur), breakFunction(0)
    {
    }
};

class DrawList
{    
public:
    
    CLASS_NEW_DELETE()  
    
    void Add(const DrawItem &item);
    void Clear();
    void Render(RenderContext &context);

private:

    std::vector<DrawItem> list;
    
    static DrawList *instance;
};

#endif // _DRAWLIST_H