#pragma once

#include "core/core.h"
#include "fast/Allocator.h"
#include "Quad.h"
#include "QuadListAttributes.h"
#include "DynamicBuffer.h"
#include "GLAbstract.h"
#include "DeviceTexture.h"
#include "RenderContext.h"
#include "OverheadCamera.h"

class RenderContext;
class DeviceTexture;

template <typename Vertex>
class QuadDrawList
{
public:
    typedef DynamicBuffer<Vertex>                   buffer_t;
    typedef Quad<Vertex>                            quad_t;
    typedef std::vector<quad_t>                     quad_array_t;
    typedef std::vector<quad_t>::iterator           quad_iter_t;
    typedef std::vector<quad_t>::const_iterator     quad_const_iter_t;
    
    QuadDrawList(const QuadListAttributes &attributes, DeviceTexture *texture) : _attributes(attributes), _changed(false), _texture(texture)
    {
        _attributes.set_texture_id(texture ? texture->GetId() : 0);
    }
    
    inline const QuadListAttributes &attributes()                                 const { return _attributes; }
    inline void                      set_camera_position(const Vector3 &position)       { if (_camera_position != position) { mark_changed(); _camera_position = position; } }
    inline const Vector3            &camera_position()                            const { return _camera_position; }
    inline void                      mark_changed()                                     { _changed = true; }
    inline bool                      transparent()                                const { return true; /* TODO */ }
    inline void                      add(const quad_t &imp)                             { _quads.push_back(imp); }
    inline void                      clear()                                            { _quads.clear(); _buffer.clear(); }
    inline size_t                    prepare()                                          { return _prepare(); }
    inline bool                      render(RenderContext &context)                     { return _render(context); }

private:
    
    struct Comparator
    {
        bool ascending;
        inline Comparator(bool ascending = true) : ascending(ascending) {}
        inline bool operator()(const quad_t &a, const quad_t &b) const {
            float d = a.distance - b.distance;
            return ascending ? d < 0 : d > 0;
        }
    };

    QuadListAttributes  _attributes;
    DeviceTexture      *_texture;
    buffer_t            _buffer;
    quad_array_t        _quads;
    Vector3             _camera_position;
    bool                _changed;
    
    size_t _prepare()
    {
        const QuadListAttributes &attr = attributes();
        if (!attr.renderable() || _quads.empty()) {
            return 0;
        }

        size_t quad_count = _quads.size();
        _buffer.reserve(quad_count * quad_t::VERTICES_PER_QUAD, quad_count * quad_t::INDICES_PER_QUAD);

        for (auto &q : _quads) {
            q.distance = camera_position().distance(q.center);
        }

        Comparator comparator(!transparent());
        std::sort(_quads.begin(), _quads.end(), comparator);

        for (const auto &q : _quads) {
            _buffer.add_indexed_quad(q.vertices);
        }

        return _quads.size();
    }

    bool _render(RenderContext &context)
    {
        const QuadListAttributes &attr = attributes();
        if (!attr.renderable() || _quads.empty()) {
            return false;
        }

        GLLoadMatrixStack(
            context.camera.GetProjection(),
            context.camera.GetView(),
            context.transform
        );

        if (attr.blend_type() == QuadListAttributes::BLEND_DARK) {
            _texture->Set(context.device, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        } else {
            _texture->Set(context.device, GL_SRC_ALPHA, GL_DST_ALPHA /*GL_ONE*/);
        }

        if (transparent()) {
            GLSetDepthWrite(false);
        }

        Vector4 color(attr.color()/* * 0.5*/);
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
