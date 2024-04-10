#pragma once

#include <stdlib.h> // for size_t

class ResizeBuffer
{
public:

    ResizeBuffer(size_t size);
    ResizeBuffer(const void *data, size_t size);
    ResizeBuffer(const ResizeBuffer &other);
    ~ResizeBuffer();

    ResizeBuffer &operator=(const ResizeBuffer &other);

    void               reserve(size_t size);
    void               clear();

    inline size_t      size()  const { return mSize; }
    inline const char *data()  const { return mData; }
    inline char       *data()        { return mData; }

    void               assign(const void *data, size_t size);

private:

    char   *mData;
    size_t  mSize;
};
