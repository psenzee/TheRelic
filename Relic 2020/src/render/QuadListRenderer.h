#pragma once

#include <vector>
#include <unordered_map>
#include "GLAbstract.h"
#include "QuadDrawList.h"
#include "DeviceTexture.h"
#include "AABox.h"

class RenderContext;
class DeviceTexture;

template <typename Vertex>
struct QuadListRenderer
{
    typedef Quad<Vertex>         quad_t;
    typedef QuadDrawList<Vertex> quad_list_t;
    
    quad_list_t *get_draw_list(const QuadListAttributes &attributes, DeviceTexture *texture)
    {
        QuadListAttributes attr(attributes);
        attr.set_texture_id(texture ? texture->GetId() : 0);
        if (_sets.contains(attr)) {
            return _sets[attr];
        }
        return (_sets[attr] = new quad_list_t(attr, texture));
    }

    void add(const QuadListAttributes &attr, DeviceTexture *texture, const quad_t &quad)
    {
        get_draw_list(attr, texture)->add(quad);
    }

    void    clear()                         { for (const auto & [attr, set] : _sets) { set->clear(); } }
    void    prepare()                       { for (const auto & [attr, set] : _sets) { set->prepare(); } }
    void    render(RenderContext &rc)       { for (const auto & [attr, set] : _sets) { set->render(rc); } }
    size_t  count()                   const { size_t count = 0; for (const auto & [attr, set] : _sets) { count += set->count(); } return count; }
    AABox   aabox()                   const { AABox aab; for (const auto & [attr, set] : _sets) { const auto &ab = set->aabox(); ab.print(std::cout); aab.insert(ab); } return aab; }

private:

    typedef std::unordered_map<QuadListAttributes, quad_list_t *, hasher_t<QuadListAttributes> > map_t;

    map_t _sets;
    AABox _aabox;
};
