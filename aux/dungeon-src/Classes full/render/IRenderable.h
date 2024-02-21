#ifndef _IRENDERABLE_H
#define _IRENDERABLE_H

#include "core/xna.h"
#include "core/aabox.h"

class GraphicsDevice;
class OverheadCamera;

class IRenderable
{
public:
    
    virtual ~IRenderable() {}
        
    virtual IRenderable *CopyIfNecessary()                                                                               = 0;
    virtual void         Update(const GameTime &time)                                                                    = 0;
    virtual void         Render(GraphicsDevice &device, const Matrix &transform, const OverheadCamera &camera, bool now) = 0;
    virtual bool         IsVisible(const Matrix &transform, const OverheadCamera &camera) const                          = 0;
    virtual AABox        GetBounds() const                                                                               = 0;
    virtual bool         IsStatic() const                                                                                = 0;
    
    virtual bool         SetProperty(const char *key, const char *value)                                                 = 0;
    virtual const char  *GetProperty(const char *key) const                                                              = 0;     
};
    
#endif // _IRENDERABLE_H
