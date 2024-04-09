#include <vector>
#include <unordered_map>
#include "GLAbstract.h"
#include "ImposterRenderer.h"
#include "DeviceTexture.h"

ImposterRenderer::ImposterRenderer() {}

ImposterSet *ImposterRenderer::get_set(const ImposterAttributes &attributes, DeviceTexture *texture)
{
    ImposterAttributes attr(attributes);
    attr.texture_id = texture->GetId();
    if (_sets.contains(attr)) {
        return _sets[attr];
    }
    return (_sets[attr] = new ImposterSet(attr, texture));
}

void ImposterRenderer::add(const ImposterAttributes &attr, DeviceTexture *texture, const Imposter &imposter)
{
    ImposterSet *is = get_set(attr, texture);
    is->add(imposter);
}

void ImposterRenderer::clear()
{
    for (const auto & [attr, set] : _sets) {
        set->clear();
    }
}

void ImposterRenderer::prepare()
{
    for (const auto & [attr, set] : _sets) {
        set->prepare();
    }
}

void ImposterRenderer::render(RenderContext &rc)
{
    for (const auto & [attr, set] : _sets) {
        set->render(rc);
    }
}
