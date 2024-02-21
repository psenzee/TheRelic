#include "DynamicBuffer.h"

#include <string.h>
#include "core/core_assert.h"

DynamicBuffer::DynamicBuffer(size_t size) : mData(0), mSize(0)
{ 
    reserve(size);
}

DynamicBuffer::DynamicBuffer(const void *data, size_t size) : mData(0), mSize(0)
{ 
    assign(data, size);
}

DynamicBuffer::DynamicBuffer(const DynamicBuffer &other) : mData(0), mSize(0)
{ 
    assign(other.data(), other.size());
}

DynamicBuffer::~DynamicBuffer()
{
    clear();
}

DynamicBuffer &DynamicBuffer::operator=(const DynamicBuffer &other)
{
    if (this != &other)
        assign(other.data(), other.size());
    return *this;
}

void DynamicBuffer::reserve(size_t size)
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

void DynamicBuffer::clear()
{ 
    mSize = 0;
    delete [] mData;
    mData = 0;
}

void DynamicBuffer::assign(const void *data, size_t size)
{
    reserve(size);
    memcpy(mData, data, size);
}