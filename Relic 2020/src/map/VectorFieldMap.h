#ifndef _VECTORFIELDMAP_H
#define _VECTORFIELDMAP_H

#include "core/core.h"

struct VectorFieldItem
{
    float   weight;
    Vector2 vector;

    inline VectorFieldItem(float weight = 0.f, const Vector2 &vector = Vector2()) : weight(weight), vector(vector) {}
};

class VectorFieldMap
{
public:

    typedef float (*WeightFunction)(const Vector2 &at, void *user);
        
    VectorFieldMap(float clampWeightLo, float clampWeightHi, int smoothWeights = 3, int smoothVectors = 9);
    ~VectorFieldMap();

    void                     SetDimensions(const Vector2 minBounds, const Vector2 maxBounds, float elementSize);
    void                     Create(WeightFunction function, void *user);
    void                     Clear();

    inline const core::Size      &GetSize()    const { return mSize; }
    inline float                  GetElementSize() const { return mElementSize; }
    inline const Vector2         &GetOffset() const { return mOffset; }

    inline const VectorFieldItem &GetItemAt(const Vector2 &p) const { Vector2 at((p - mOffset) + mElementSize * 0.5f); return GetItem(int(at.x / mElementSize), int(at.y / mElementSize)); }
    const VectorFieldItem        &GetItem(int x, int y) const;
        
private:

    void                     Normalize();
    Vector2                  GetPosition(int x, int y) const;
    void                     SetWeight(int x, int y, float weight);
    float                    GetWeight(int x, int y) const;
    float                    GetGaussianBlurredValue3(int x, int y);
    float                    GetAverageValue(int x, int y, int kernelSize);
    void                     GaussianBlur();
    void                     Smooth(int kernelSize);
    Vector2                  GetVector(int x, int y, int kernelSize);
    void                     CalculateVectors(int kernelSize);
        
    core::Size       mSize;
    float            mElementSize;
    Vector2          mOffset;
    VectorFieldItem *mData;
    float            mClampHi,
                     mClampLo;
    int              mSmoothWeights,
                     mSmoothVectors;
};

class CompactVectorFieldMap
{
public:

    CompactVectorFieldMap(const VectorFieldMap &src);
    ~CompactVectorFieldMap();

    inline const core::Size &GetSize()                   const { return mSize; }

    inline VectorFieldItem   GetItemAt(const Vector2 &p) const { Vector2 at((p - mOffset) + mElementSize * 0.5f); return GetItem(int(at.x / mElementSize), int(at.y / mElementSize)); }
    inline VectorFieldItem   GetItem(int x, int y)       const { return GetItem(mData[y * mSize.width + x]); }

private:

    static unsigned short GetUint16(const VectorFieldItem &p);
    static VectorFieldItem GetItem(unsigned short value);

    core::Size      mSize;
    float           mElementSize;
    Vector2         mOffset;

    unsigned short *mData;
};

#endif // _VECTORFIELDMAP_H