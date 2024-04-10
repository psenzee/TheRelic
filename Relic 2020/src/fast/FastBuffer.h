#pragma

#include "FixedBuffer.h"
#include "ResizeBuffer.h"

template <unsigned short SIZE>
class FastBuffer
{
public:

    inline FastBuffer(size_t size)                   { reserve(0); }
    inline FastBuffer(const void *s, size_t size)    { assign(s, size); }
    inline FastBuffer(const FastBuffer<SIZE> &other) { assign(other.data(), other.size()); }

    inline FastBuffer &operator=(const FastBuffer<SIZE> &other)
    {
        if (this != &other)
            assign(other.data(), other.size());
        return *this;
    }

    inline size_t      size()  const { return mSize; }
    inline bool        empty() const { return !mSize; }
    inline void        clear()       { assign(0); }
    inline const char *c_str() const { return mSize >= SIZE ? mDynamic.c_str() : mFixed.c_str(); }
    inline const char *data()  const { return mSize >= SIZE ? mDynamic.data()  : mFixed.data();  }

    inline void        reserve(size_t size)
    { 
        if (size >= SIZE)
            mDynamic.reserve(size);
    }

    inline void        assign(const void *s, size_t size)
    {
        mSize = size;
        if (size >= SIZE) { mFixed.clear(); mDynamic.assign(s, size); }
        else              { mDynamic.clear(); mFixed.assign(s, size); }
    }

private:

    FixedBuffer<SIZE> mFixed;
    ResizeBuffer      mDynamic;
    size_t            mSize;
};

