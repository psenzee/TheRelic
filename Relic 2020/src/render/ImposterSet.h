#pragma once

#include "core/core.h"
#include "fast/Allocator.h"
#include "Imposter.h"
#include "ImposterAttributes.h"
#include "DynamicBuffer.h"
#include "GLAbstract.h"
#include "DeviceTexture.h"
#include "RenderContext.h"
#include "OverheadCamera.h"

class RenderContext;
class DeviceTexture;

template <typename Vertex>
class ImposterSet
{
public:
    typedef DynamicBuffer<Vertex>                   buffer_t;
    typedef Imposter<Vertex>                        imposter_t;
    typedef std::vector<imposter_t>                 imposter_array_t;
    typedef std::vector<imposter_t>::iterator       imposter_iter_t;
    typedef std::vector<imposter_t>::const_iterator imposter_const_iter_t;
    
    ImposterSet(const ImposterAttributes &attributes, DeviceTexture *texture) : _attributes(attributes), _changed(false), _texture(texture)
    {
        _attributes.texture_id = texture ? texture->GetId() : 0;
    }
    
    inline const ImposterAttributes &attributes()                                 const { return _attributes; }
    inline void                      set_camera_position(const Vector3 &position)       { if (_camera_position != position) { mark_changed(); _camera_position = position; } }
    inline const Vector3            &camera_position()                            const { return _camera_position; }
    inline void                      mark_changed()                                     { _changed = true; }
    inline bool                      transparent()                                const { return true; /* TODO */ }
    inline void                      add(const imposter_t &imp)                         { _imposters.push_back(imp); }
    inline void                      clear()                                            { _imposters.clear(); _buffer.clear(); }
    inline size_t                    prepare()                                          { return _prepare(); }
    inline bool                      render(RenderContext &context)                     { return _render(context); }

private:
    
    struct Comparator
    {
        bool ascending;
        inline Comparator(bool ascending = true) : ascending(ascending) {}
        inline bool operator()(const imposter_t &a, const imposter_t &b) const {
            float d = a.distance - b.distance;
            return ascending ? d < 0 : d > 0;
        }
    };

    ImposterAttributes   _attributes;
    DeviceTexture       *_texture;
    buffer_t             _buffer;
    imposter_array_t     _imposters;
    Vector3              _camera_position;
    bool                 _changed;
    
    size_t _prepare()
    {
        const ImposterAttributes &attr = attributes();
        if (!attr.renderable() || _imposters.empty()) {
            return 0;
        }

        size_t imposter_count = _imposters.size();

        enum { VERTICES_PER_QUAD = 4, INDICES_PER_QUAD = 6 };
        _buffer.reserve(imposter_count * VERTICES_PER_QUAD, imposter_count * INDICES_PER_QUAD);

        for (imposter_iter_t i = _imposters.begin(), e = _imposters.end(); i != e; ++i) {
            (*i).distance = camera_position().distance((*i).center);
        }

        Comparator comparator(!transparent());
        std::sort(_imposters.begin(), _imposters.end(), comparator);

        for (imposter_const_iter_t i = _imposters.begin(), e = _imposters.end(); i != e; ++i) {
            const imposter_t &imposter = *i;
            _buffer.add_indexed_quad(imposter.vertices);
        }

        return _imposters.size();
    }

    bool _render(RenderContext &context)
    {
        const ImposterAttributes &attr = attributes();
        if (!attr.renderable() || _imposters.empty()) {
            return false;
        }

        GLLoadMatrixStack(
            context.camera.GetProjection(),
            context.camera.GetView(),
            context.transform
        );

        if (attr.blend_type == ImposterAttributes::BLEND_DARK) {
            _texture->Set(context.device, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        } else {
            _texture->Set(context.device, GL_SRC_ALPHA, GL_DST_ALPHA /*GL_ONE*/);
        }

        if (transparent()) {
            GLSetDepthWrite(false);
        }

        Vector4 color(attr.color * 0.5);
        GraphicsDevice::GetInstance()->SetColor(color);

        GLSetEnabled(GL_TEXTURE_2D, true);

        _buffer.render(context);

        if (transparent()) {
            GLSetDepthWrite(true);
        }

        _texture->Set(context.device, GL_ONE, GL_ONE);

        return true;
    }
};
