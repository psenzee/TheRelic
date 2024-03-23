#pragma once

#include "core/core.h"
#include "fast/Allocator.h"

class RenderContext;
class DeviceTexture;

struct Particle
{
    Vector3                position,
                           velocity;
    Vector2                uv0,
                           uv1;
    float               // angle,
                           alpha,
                           size,
                           time,
                           expire;

    void                  *_user;

    float                  _maxSpeed,
                           _maxExpire,
                           _maxSize,
                           _maxZ;
    Vector3                _direction;
    bool                   _isShadow;

    inline Particle() : uv1(1.f, 1.f), alpha(1.f), size(10.f), time(0.0f), expire(1000.f),
                        _maxSpeed(1.f), _maxExpire(1000.f), _maxSize(75.f), _maxZ(50.f), _isShadow(false), _user(0) {}
    
    inline bool operator<(const Particle &b) const { return position.z < b.position.z; }
};
