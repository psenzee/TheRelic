#pragma once

#include "core/core.h"
#include "fast/Allocator.h"
#include "Imposter.h"
#include "ImposterAttributes.h"
#include "DynamicBuffer.h"

class RenderContext;
class DeviceTexture;

class ImposterSet
{
public:

    typedef DynamicBuffer<CommonVertex>           dyn_buffer_t;
    typedef std::vector<Imposter>                 imposter_array_t;
    typedef std::vector<Imposter>::iterator       imposter_iter_t;
    typedef std::vector<Imposter>::const_iterator imposter_const_iter_t;

    ImposterSet(const ImposterAttributes &attributes, DeviceTexture *texture);
    ~ImposterSet();

    inline const ImposterAttributes &attributes()                                 const { return _attributes; }
    inline void                      set_camera_position(const Vector3 &position)       { if (_camera_position != position) { mark_changed(); _camera_position = position; } }
    inline const Vector3            &camera_position()                            const { return _camera_position; }
    inline void                      mark_changed()                                     { _changed = true; }
    inline bool                      transparent()                                const { return true; /* TODO */ }

    void add(const Imposter &imposter);
    void clear();
    int  prepare();
    bool render(RenderContext &context);

private:

    ImposterAttributes   _attributes;
    DeviceTexture       *_texture;
    dyn_buffer_t         _buffer;
    imposter_array_t     _imposters;
    Vector3              _camera_position;
    bool                 _changed;
};
