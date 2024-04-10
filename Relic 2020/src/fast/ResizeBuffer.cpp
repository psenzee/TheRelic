#include "ResizeBuffer.h"

#include <string.h>
#include "core/core_assert.h"

ResizeBuffer::ResizeBuffer(size_t size) : mData(0), mSize(0)
{
    reserve(size);
}

ResizeBuffer::ResizeBuffer(const void *data, size_t size) : mData(0), mSize(0)
{
    assign(data, size);
}

ResizeBuffer::ResizeBuffer(const ResizeBuffer &other) : mData(0), mSize(0)
{
    assign(other.data(), other.size());
}

ResizeBuffer::~ResizeBuffer()
{
    clear();
}

ResizeBuffer &ResizeBuffer::operator=(const ResizeBuffer &other)
{
    if (this != &other)
        assign(other.data(), other.size());
    return *this;
}

void ResizeBuffer::reserve(size_t size)
{
    if (mSize < size)
    {
        char *data = new char [size];
        memcpy(data, mData, mSize);
        mSize = size;
        if (mData)
            delete [] mData;
        data = mData;
    }
}

void ResizeBuffer::clear()
{
    mSize = 0;
    delete [] mData;
    mData = 0;
}

void ResizeBuffer::assign(const void *data, size_t size)
{
    reserve(size);
    memcpy(mData, data, size);
}
