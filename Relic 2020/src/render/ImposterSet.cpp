#include "ImposterSet.h"

#include "core/core.h"
#include "core/random.h"

#include "render/GLUtils.h"
#include "render/GLStates.h"
#include "render/GLIncludes.h"
#include "render/DeviceTexture.h"
#include "render/GraphicsDevice.h"
#include "render/OverheadCamera.h"
#include "render/Material.h"
#include "render/RenderContext.h"
#include "render/DeviceTexture.h"

// the following are for testing collision with particle effects
#include "GLAbstract.h"

#include <algorithm>
#include <iostream>

struct Comparator
{
    bool ascending;
    inline Comparator(bool ascending = true) : ascending(ascending) {}
    inline bool operator()(const Imposter &a, const Imposter &b) const {
        float d = a.distance - b.distance;
        return ascending ? d < 0 : d > 0;
    }
};

ImposterSet::~ImposterSet() {}

ImposterSet::ImposterSet(const ImposterAttributes &attributes, DeviceTexture *texture) : _attributes(attributes), _changed(false), _texture(texture)
{
    _attributes.texture_id = texture ? texture->GetId() : 0;
}

void ImposterSet::add(const Imposter &imp)
{
    _imposters.push_back(imp);
}

void ImposterSet::clear()
{
    _imposters.clear();
    _buffer.clear();
}

int ImposterSet::prepare()
{
    const ImposterAttributes &attr = attributes();
    if (attr.color.w < 0.01f || !attr.texture_id /*|| !attr.program_id*/ || _imposters.empty()) {
        return 0;
    }

    static const Tuple3f SCALE(-1.0f, -1.0f, 1.0f),
                         UL(Tuple3f(-0.5f, -0.5f, 0.0f) * SCALE), UR(Tuple3f( 0.5f, -0.5f, 0.0f) * SCALE),
                         LL(Tuple3f(-0.5f,  0.5f, 0.0f) * SCALE), LR(Tuple3f( 0.5f,  0.5f, 0.0f) * SCALE);

    enum { VERTICES_PER_QUAD = 4, INDICES_PER_QUAD = 6 };

    size_t imposter_count = _imposters.size();

    _buffer.reserve(imposter_count * VERTICES_PER_QUAD, imposter_count * INDICES_PER_QUAD);

    int quads = 0, indices = 0;

    typedef CommonVertex v_t;

    for (imposter_iter_t i = _imposters.begin(), e = _imposters.end(); i != e; ++i) {
        (*i).distance = camera_position().distance((*i).position);
    }

    Comparator comparator(!transparent());
    std::sort(_imposters.begin(), _imposters.end(), comparator);

    for (imposter_const_iter_t i = _imposters.begin(), e = _imposters.end(); i != e; ++i) {
        const Imposter &imposter = *i;

        const Tuple3f &pos  = imposter.position,
                       size(imposter.size.x, imposter.size.y, 1.0);
        const Tuple2f &uv0  = imposter.texture_uv[0],
                      &uv1  = imposter.texture_uv[1];

        const v_t vertices_data[] = {
            v_t { pos + UL * size, uv0 }, v_t { pos + LL * size, Tuple2f(uv0.x, uv1.y) },
            v_t { pos + LR * size, uv1 }, v_t { pos + UR * size, Tuple2f(uv1.x, uv0.y) },
        };

        const uint16_t indices_data[] = {
            uint16_t(indices + 0), uint16_t(indices + 1), uint16_t(indices + 2),
            uint16_t(indices + 2), uint16_t(indices + 1), uint16_t(indices + 3)
        };

        _buffer.add_vertices(vertices_data);
        _buffer.add_indices(indices_data);

        indices += VERTICES_PER_QUAD;
        quads++;
    }

    return quads;
}

bool ImposterSet::render(RenderContext &context)
{
    const ImposterAttributes &attr = attributes();
    if (attr.color.w < 0.01f || !attr.texture_id || _imposters.empty()) {
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
        _texture->Set(context.device, GL_SRC_ALPHA, /*GL_DST_ALPHA*/ GL_ONE);
    }

    if (transparent()) {
        GLSetDepthWrite(false);
    }

    Vector4 color(attr.color/* * 0.5*/);
    GraphicsDevice::GetInstance()->SetColor(color);

    GLSetEnabled(GL_TEXTURE_2D, true);

    _buffer.render(context);

    if (transparent()) {
        GLSetDepthWrite(true);
    }

    _texture->Set(context.device, GL_ONE, GL_ONE);
    if (attr.blend_type != ImposterAttributes::BLEND_DARK) {
    //    _texture->Set(context.device, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    return true;
}

