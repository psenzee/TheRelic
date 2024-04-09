#pragma once

#include <vector>
#include <unordered_map>
#include "GLAbstract.h"
#include "ImposterSet.h"

class RenderContext;
class DeviceTexture;

struct ImposterRenderer
{
    ImposterRenderer();

    ImposterSet *get_set(const ImposterAttributes &attributes, DeviceTexture *texture);

    void add(const ImposterAttributes &attr, DeviceTexture *texture, const Imposter &imposter);
    void clear();
    void prepare();
    void render(RenderContext &rc);

private:

    typedef std::unordered_map<ImposterAttributes, ImposterSet *, hasher_t<ImposterAttributes> > map_t;

    map_t _sets;
};
