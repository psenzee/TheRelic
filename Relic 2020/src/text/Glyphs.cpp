#include "Glyphs.h"

Glyphs *Glyphs::_instance;

int Glyphs::Render(RenderContext &context, const GameTime &time)
{
    int count = 0;
    _renderer.clear();
    /*
    std::vector<ParticleSystem *> fx(mParticleSystems); // make a copy, because we're going to delete from the original
    append(std::span<ParticleSystem *>(mDeferred.data(), mDeferred.size()), fx);
    for (std::vector<ParticleSystem *>::iterator i = fx.begin(), e = fx.end(); i != e; ++i) {
        //count += (*i)->Render(context, time);
        count += (*i)->prepare(_renderer, time);
    }
    */
    _renderer.prepare();
    _renderer.render(context);
    return count;
}
