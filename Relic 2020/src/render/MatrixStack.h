#pragma once

#include "core/core.h"
#include "matrix4f.h"
#include <array>

struct MatrixStack
{
    enum Type { TYPE_PROJECTION = 0, TYPE_VIEW, TYPE_MODEL, TYPE_PROJECTION_VIEW, TYPE_VIEW_MODEL, TYPE_PROJECTION_VIEW_MODEL, TYPE__COUNT };

    typedef std::array<Matrix4f, TYPE__COUNT> matrix_array_t;

    inline const Matrix4f &get(Type type)                    const { if (~_valids != 0) _update(type); return _types[type]; }
    inline MatrixStack    &set(Type type, const Matrix4f &m)       { _types[type] = m; _invalidate_for(type); return *this; }

    MatrixStack();
    
private:

    inline uint64_t _flag(Type type) const { return 1ull << int(type); }
    
    inline void _invalidate_for(Type type) const
    {
        _valids |= _flag(type); // set the actual set value to valid
        static const uint64_t affects[TYPE__COUNT] = {
            /* TYPE_PROJECTION            */ _flag(TYPE_PROJECTION_VIEW) | _flag(TYPE_PROJECTION_VIEW_MODEL),
            /* TYPE_VIEW                  */ _flag(TYPE_PROJECTION_VIEW) | _flag(TYPE_VIEW_MODEL) | _flag(TYPE_PROJECTION_VIEW_MODEL),
            /* TYPE_MODEL                 */ _flag(TYPE_VIEW_MODEL) | _flag(TYPE_PROJECTION_VIEW_MODEL),
            /* TYPE_PROJECTION_VIEW       */ _flag(TYPE_PROJECTION_VIEW_MODEL),
            /* TYPE_VIEW_MODEL            */ _flag(TYPE_VIEW_MODEL),
            /* TYPE_PROJECTION_VIEW_MODEL */ 0
        };
        _valids &= ~affects[type];
    }
    
    void _update(Type type) const;

    mutable matrix_array_t _types;
    mutable uint64_t       _valids;
};
