#pragma once

#include "core/core.h"
#include "tuple2f.h"
#include "tuple3f.h"
#include "tuple4f.h"
#include <array>

struct Imposter
{
    Tuple3f                position;
    std::array<Tuple2f, 2> texture_uv;
    Tuple2f                size;
    float                  distance;
};
