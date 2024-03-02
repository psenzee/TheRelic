#ifndef _LIGHTS_H
#define _LIGHTS_H

#include "core/core.h"
#include <vector>
#include "Light.h"

class GraphicsDevice;
class OverheadCamera;

class Lights
{
    typedef std::vector<Light>              list_t;
    typedef typename list_t::const_iterator list_const_iter_t;
    typedef typename list_t::iterator       list_iter_t;
    
    list_t mLights;
    bool   mSorted;
    bool   mEnabled;
    
    static const size_t MAX_ACTIVE_LIGHTS_OGL = 8; // GL_MAX_LIGHTS
    
    void              _Sort();
    list_const_iter_t _Find(size_t id) const;
    list_iter_t       _Find(size_t id);

public:

    inline Lights() : mSorted(true), mEnabled(false) {}
    
    inline size_t        GetMaxActiveLights() const      { return MAX_ACTIVE_LIGHTS_OGL; } // make static?
    
    const Light         &Set(const Light &light);
    
    Lights              &SetVector(size_t id, Light::VectorType type, const Vector4 &v);
    const Vector4       *GetVector(size_t id, Light::VectorType type);
    
    Lights              &SetFloat(size_t id, Light::FloatType type, float v);
    const float         *GetFloat(size_t id, Light::FloatType type);
    
    Lights              &SetShininess(size_t id, float value);

    inline Lights       &Enable(bool enabled)            { mEnabled = enabled; return *this; }
    const Light         &Enable(size_t id, bool enabled);

    inline bool          IsEnabled() const               { return mEnabled; }
    inline bool          IsEnabled(size_t id) const      { const Light *lux = Get(id); return lux && lux->enabled; }
    
    void                 Remove(size_t id);
    const Light         *Get(size_t id) const;

    void                 Apply(GraphicsDevice &device, const OverheadCamera &camera);
};

#endif // _LIGHTS_H
