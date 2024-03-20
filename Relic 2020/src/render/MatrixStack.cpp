#include "MatrixStack.h"

MatrixStack::MatrixStack() : _valids(~0ull)
{
    Matrix4f m;
    m.identity();
    for (size_t i = 0; i < size_t(TYPE__COUNT); i++) {
        _types[i] = m;
    }
}

void MatrixStack::_update(Type type) const
{
    for (size_t i = size_t(TYPE_PROJECTION_VIEW); i < size_t(TYPE__COUNT); i++) {
        uint64_t bit = (1ull << i);
        if (!(bit & uint64_t(type))) {
            Type t = Type(i);
            const Matrix4f &p = _types[TYPE_PROJECTION], &v = _types[TYPE_VIEW], &m = _types[TYPE_MODEL], &pv = _types[TYPE_PROJECTION_VIEW];
            switch (t) {
            case TYPE_PROJECTION_VIEW:       _types[t] = p * v;  break;
            case TYPE_VIEW_MODEL:            _types[t] = v * m;  break;
            case TYPE_PROJECTION_VIEW_MODEL: _types[t] = pv * m; break;
            default:                                             break;
            }
            _valids |= bit;
        }
    }
}
