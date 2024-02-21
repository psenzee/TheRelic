#ifndef _FIXEDBUFFER_H
#define _FIXEDBUFFER_H

#include <string.h>
#include <assert.h>

template <unsigned SIZE>
class FixedBuffer
{
public:

    inline FixedBuffer()                                { assign(0, 0); }
    inline FixedBuffer(const void *data, unsigned size) { assign(data, size); }
    inline FixedBuffer(const FixedBuffer<SIZE> &other)  { assign(other.data(), other.size()); }

    inline FixedBuffer &operator=(const FixedBuffer<SIZE> &other)
    {
        if (this != &other)
            assign(other.data(), other.size());
        return *this;
    }

    inline size_t      size()  const { return SIZE; }
    inline void        clear()       { memset(mData, 0, sizeof(mData)); }
    inline const char *data()  const { return mData; }
    inline char       *data()        { return mData; }

    inline void assign(const void *data, size_t size)
    {
        if (size > MAX_SIZE) // make sure it fits
            size = MAX_SIZE;
        memcpy(mData, data, size);
    }

private:

    char mData[SIZE];
};

#endif // _FIXEDBUFFER_H