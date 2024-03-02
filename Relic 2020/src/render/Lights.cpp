#include "Lights.h"
#include "GraphicsDevice.h"
#include <algorithm>

namespace {

struct _comparator {
    inline bool operator()(const Light &a, const Light &b) {
        // enabled=true always sorts higher than enabled=false
        // then by ascending id
        return a.enabled != b.enabled ? a.enabled > b.enabled : a.id < b.id;
    }
};

}

void Lights::_Sort()
{
    if (!mSorted) {
        //std::sort(mLights.begin(), mLights.end(), _comparator());
        std::sort(mLights.begin(), mLights.end(),
                  [](const Light &a, const Light &b) { return a.enabled != b.enabled ? a.enabled > b.enabled : a.id < b.id; });
    }
    mSorted = true;
}

typename Lights::list_const_iter_t Lights::_Find(size_t id) const
{
    // use binary search when mSorted==true for an optimization if mLights ever gets large enough
    for (auto i = mLights.begin(), e = mLights.end(); i != e; ++i)
        if ((*i).id == id)
            return i;
    return mLights.end();
}

typename Lights::list_iter_t Lights::_Find(size_t id)
{
    // possible TODO use binary search when mSorted==true for an optimization if mLights ever gets large enough
    for (auto i = mLights.begin(), e = mLights.end(); i != e; ++i)
        if ((*i).id == id)
            return i;
    return mLights.end();
}

Lights &Lights::SetVector(size_t id, Light::VectorType type, const Vector4 &v)
{
    Light lux(id);
    const Light *p = Get(id);
    if (p) {
        lux = *p;
    }
    Set(lux.Set(type, v));
    return *this;
}

Lights &Lights::SetFloat(size_t id, Light::FloatType type, float value)
{
    Light lux(id);
    const Light *p = Get(id);
    if (p) {
        lux = *p;
    }
    lux.Set(type, value);
    Set(lux);
    return *this;
}

const float *Lights::GetFloat(size_t id, Light::FloatType type)
{
    const Light *p = Get(id);
    return p ? &(p->floats[type]) : nullptr;
}

const Vector4 *Lights::GetVector(size_t id, Light::VectorType type)
{
    const Light *p = Get(id);
    return p ? &(p->vectors[type]) : nullptr;
}

const Light &Lights::Enable(size_t id, bool enabled)
{
    list_iter_t f = _Find(id);
    if (f == mLights.end()) {
        Light lux(id, enabled);
        mSorted = false;
        return Set(lux);
    }
    (*f).enabled = enabled;
    mSorted = false;
    return *f;
}

const Light &Lights::Set(const Light &light)
{
    auto i = _Find(light.id);
    if (i != mLights.end()) {
        // already exists
        if ((*i).enabled != light.enabled) {
            // mark dirty if enableds differ
            // otherwise they will sort the same, given the same id
            mSorted = false;
        }
        return (*i = light);
    }
    mLights.push_back(light);
    mSorted = false;
    return mLights.back();
}

void Lights::Remove(size_t id)
{
    mLights.erase(
       std::remove_if(mLights.begin(), mLights.end(), [id](auto a) { return a.id == id; }),
                  mLights.end());
}

const Light *Lights::Get(size_t id) const
{
    auto i = _Find(id);
    return i != mLights.end() ? &(*i) : nullptr;
}

void Lights::Apply(GraphicsDevice &device, const OverheadCamera &camera)
{
    device.EnableLighting(mEnabled);
    if (!mEnabled) {
        return;
    }
    _Sort();
    size_t slot = 0;
    for (const auto &light : mLights) {
        if (light.enabled)
            device.SetLight(camera, slot++, light, true);
    }
    for (size_t max = GetMaxActiveLights(); slot < max; slot++) {
        device.EnableLight(slot, false);
    }
}
