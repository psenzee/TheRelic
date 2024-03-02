#ifndef _LIGHT_H
#define _LIGHT_H

#include "core/core.h"
#include "fast/Allocator.h"
#include <array>

struct Light
{
    CLASS_NEW_DELETE()
    
    enum VectorType { V4_WORLD_POSITION, V4_AMBIENT, V4_DIFFUSE, V4_SPECULAR, V4_SPOT_DIRECTION, V4_COUNT };
    enum FloatType  {
        F1_SHININESS, F1_SPOT_EXPONENT, F1_SPOT_CUTOFF,
        F1_CONSTANT_ATTENUATION, F1_LINEAR_ATTENUATION, F1_QUADRATIC_ATTENUATION, F1_COUNT
    };

    typedef std::array<Vector4, V4_COUNT> array_v4_t;
    typedef std::array<float, F1_COUNT>   array_f_t;

    size_t     id;
    bool       enabled;
    array_v4_t vectors;
    array_f_t  floats;

    inline Light(const Light &u) : id(u.id), enabled(u.enabled), floats{ 0 }, vectors({ _1, _1, _1, _1, _1 }) {}
    inline Light(size_t id = 0, bool enabled = false) : id(id), enabled(enabled), floats{ 0 } {}

    inline Light         &Set(VectorType type, const Vector4 &v)  { vectors[type] = v; return *this; }
    inline const Vector4 &Get(VectorType type)              const { return vectors[type]; }
    
    inline Light         &Set(FloatType type, float v)            { floats[type] = v; return *this; }
    inline float          Get(FloatType type)               const { return floats[type]; }
    
private:
    
    static constexpr const Vector4 _0 { 0.f, 0.f, 0.f, 0.f };
    static constexpr const Vector4 _1 { 1.f, 1.f, 1.f, 1.f };
};

#endif // _LIGHT_H
