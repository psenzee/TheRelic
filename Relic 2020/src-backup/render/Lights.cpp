#include "Lights.h"
#include "GraphicsDevice.h"
#include <algorithm>

void Lights::_Sort()
{
    if (!mSorted) {
        //std::sort(mLights.begin(), mLights.end(), Comparator());
        std::sort(mLights.begin(), mLights.end(),
            [](const Light &a, const Light &b) { return a.enabled != b.enabled ? a.enabled > b.enabled : a.id < b.id; });
    }
    mSorted = true;
}

auto Lights::_Find(size_t id) const
{
    // use binary search when mSorted==true for an optimization if mLights ever gets large enough
    for (auto i = mLights.begin(), e = mLights.end(); i != e; ++i)
        if ((*i).id == id)
            return i;
    return mLights.end();
}

auto Lights::_Find(size_t id)
{
    // use binary search when mSorted==true for an optimization if mLights ever gets large enough
    for (auto i = mLights.begin(), e = mLights.end(); i != e; ++i)
        if ((*i).id == id)
            return i;
    return mLights.end();
}

void Lights::Set(const Light &light)
{
    auto i = _Find(light.id);
    if (i != mLights.end()) {
        // already exists
        if ((*i).enabled != light.enabled) {
            // mark dirty if enableds differ
            // otherwise they will sort the same, given the same id
            mSorted = false;
        }
        *i = light;
    }
    else {
        mLights.push_back(light);
        mSorted = false;
    }
}

void Lights::Remove(size_t id)
{
    v.erase(
        std::remove_if(mLights.begin(), mLights.end(),
            [](const Light &lux) { return lux.id == id; }), v.end());
}

const Light *Lights::Get(size_t id) const
{
    auto i = _Find(id);
    return i != mLights.end() ? &(*i) : nullptr;
}

void Lights::Render(GraphicsDevice &device)
{
    if (!device.IsLightingEnabled()) {
        return;
    }
    _Sort();
    for (const auto &light : mLights) {
        if (light.enabled) {
            device.SetLight(slot, light, true);
            slot++;
        }
    }
    for (size_t max = GetMaxActiveLights(); slot < max; slot++) {
        device.EnableLight(slot, false);
    }
}
