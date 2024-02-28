#ifndef _LIGHTS_H
#define _LIGHTS_H

#include "core/core.h"
#include <vector>
#include "Light.h"

class GraphicsDevice;

class Lights
{
    typedef std::vector<Light>              list_t;
    typedef typename list_t::const_iterator list_const_iter_t;
    typedef typename list_t::iterator       list_iter_t;
    
    list_t mLights;
    bool   mSorted;
    
    static const size_t MAX_ACTIVE_LIGHTS_OGL = 8;
    
    struct Comparator {
        inline bool operator()(const Light &a, const Light &b) {
            // enabled=true always sorts higher than enabled=false
            // then by ascending id
            return a.enabled != b.enabled ? a.enabled > b.enabled : a.id < b.id;
        }
    };
    
    void _Sort();
    list_const_iter_t _Find(size_t id) const;
    list_iter_t       _Find(size_t id);

public:
       
    inline Lights() : mSorted(true) {}
    
    inline size_t GetMaxActiveLights() const { return MAX_ACTIVE_LIGHTS_OGL; } // make static?
    
    void          Set(const Light &light);
    void          Remove(size_t id);
    const Light  *Get(size_t id) const;

    void          Render(GraphicsDevice &device);
};

#endif // _LIGHTS_H
