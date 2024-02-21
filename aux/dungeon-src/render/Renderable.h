#ifndef _RENDERABLE_H
#define _RENDERABLE_H

#include "core/xna.h"

#include "IRenderable.h"
#include "Transform.h"
#include "Models.h"

#include <vector>
#include <string>
#include <map>

#include "core/aabox.h"

class Event;
class RenderableModel;

class Renderable : public IRenderable
{
public:
        
    Renderable(const char *name, std::vector<IModel *> models, const Transform &transform, std::vector<Event *> events);
        
    void         Update(const GameTime &time);
    IRenderable *CopyIfNecessary();
    void         Render(GraphicsDevice &device, const Matrix &transform, const OverheadCamera &camera, bool now);
    bool         IsVisible(const Matrix &transform, const OverheadCamera &camera) const;
    AABox        GetBounds() const;
    bool         IsStatic() const;

    inline std::vector<Event *> &GetEvents() { return events; }
    
    bool         SetProperty(const char *key, const char *value); 
    const char  *GetProperty(const char *key) const;
    
private:
    
    std::string                        name;
    std::vector<IModel *>              models;
    Transform                          transform;
    std::vector<Event *>               events;
    AABox                              bounds; // cached
    std::map<std::string, std::string> properties;
};

#endif // _RENDERABLE_H