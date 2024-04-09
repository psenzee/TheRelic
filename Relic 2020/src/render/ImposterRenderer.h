#pragma once

#include <vector>
#include <unordered_map>
#include "GLAbstract.h"
#include "ImposterSet.h"
#include "DeviceTexture.h"

class RenderContext;
class DeviceTexture;

template <typename Vertex>
struct ImposterRenderer
{
    typedef Imposter<Vertex>    imposter_t;
    typedef ImposterSet<Vertex> imposter_set_t;
    
    imposter_set_t *get_set(const ImposterAttributes &attributes, DeviceTexture *texture)
    {
        ImposterAttributes attr(attributes);
        attr.texture_id = texture->GetId();
        if (_sets.contains(attr)) {
            return _sets[attr];
        }
        return (_sets[attr] = new imposter_set_t(attr, texture));
    }

    void add(const ImposterAttributes &attr, DeviceTexture *texture, const imposter_t &imposter)
    {
        imposter_set_t *is = get_set(attr, texture);
        is->add(imposter);
    }

    void clear()
    {
        for (const auto & [attr, set] : _sets) {
            set->clear();
        }
    }

    void prepare()
    {
        for (const auto & [attr, set] : _sets) {
            set->prepare();
        }
    }

    void render(RenderContext &rc)
    {
        for (const auto & [attr, set] : _sets) {
            set->render(rc);
        }
    }

private:

    typedef std::unordered_map<ImposterAttributes, imposter_set_t *, hasher_t<ImposterAttributes> > map_t;

    map_t _sets;
};
