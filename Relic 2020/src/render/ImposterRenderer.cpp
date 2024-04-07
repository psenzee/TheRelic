#include <vector>
#include <unordered_map>
#include "GLAbstract.h"
#include "ImposterRenderer.h"

ImposterRenderer::ImposterRenderer()
{
}

ImposterSet *ImposterRenderer::get_set(const ImposterAttributes &attributes, DeviceTexture *texture)
{
    if (_sets.contains(attributes)) {
        return _sets[attributes];
    }
    return (_sets[attributes] = new ImposterSet(attributes, texture));
}

void ImposterRenderer::add(const ImposterAttributes &attr, DeviceTexture *texture, const Imposter &imposter)
{
    ImposterSet *is = get_set(attr, texture);
    is->add(imposter);
}

void ImposterRenderer::clear()
{
    for (auto const & [attr, set] : _sets) {
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
