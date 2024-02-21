#include "VectorFieldMap.h"

VectorFieldMap::VectorFieldMap(float clampWeightLo, float clampWeightHi, int smoothWeights, int smoothVectors)
    : mData(0), mElementSize(1.f), mClampLo(clampWeightLo), mClampHi(clampWeightHi), mSmoothWeights(smoothWeights), mSmoothVectors(smoothVectors)
{
}

VectorFieldMap::~VectorFieldMap()
{ 
    Clear();
}

void VectorFieldMap::SetDimensions(const Vector2 minBounds, const Vector2 maxBounds, float elementSize)
{
    Clear();
    mSize.width  = (int)((maxBounds.x - minBounds.x) / elementSize) + 1;
    mSize.height = (int)((maxBounds.y - minBounds.y) / elementSize) + 1;
    mElementSize = elementSize;
    mOffset      = minBounds;
    mData        = new VectorFieldItem [mSize.width * mSize.height];
    memset(mData, 0, sizeof(VectorFieldItem) * mSize.width * mSize.height);
}

void VectorFieldMap::Create(WeightFunction function, void *user)
{
    for (int j = 0; j < mSize.height; j++)
        for (int i = 0; i < mSize.width; i++)
            SetWeight(i, j, math::clamp(function(GetPosition(i, j), user), mClampLo, mClampHi));
    Smooth(mSmoothWeights);
    CalculateVectors(mSmoothVectors);
    Normalize();
}

void VectorFieldMap::Clear()
{
    if (mData)
    {
        delete [] mData;
        mData = 0;
    }
}

const VectorFieldItem &VectorFieldMap::GetItem(int x, int y) const
{
    static const VectorFieldItem null;
    if (x < 0 || y < 0 || x >= mSize.width || y >= mSize.height)
        return null;
    return mData[y * mSize.width + x];
}

Vector2 VectorFieldMap::GetPosition(int x, int y) const
{
    Vector2 offset(mOffset + mElementSize * 0.5f);
    return Vector2(x * mElementSize + offset.x, y * mElementSize + offset.y);
}

void VectorFieldMap::SetWeight(int x, int y, float weight)
{
    if (x < 0 || y < 0 || x >= mSize.width || y >= mSize.height)
        return;
    mData[y * mSize.width + x].weight = weight;
}

float VectorFieldMap::GetWeight(int x, int y) const
{
    if (x < 0 || y < 0 || x >= mSize.width || y >= mSize.height)
        return mClampHi;
    return mData[y * mSize.width + x].weight;
}

float VectorFieldMap::GetGaussianBlurredValue3(int x, int y)
{
    float kernel[] = // gaussian
    {
        0.0751f,    0.1238f,    0.0751f, 
        0.1238f,    0.2042f,    0.1238f, 
        0.0751f,    0.1238f,    0.0751f,
    };
    return GetWeight(x - 1, y - 1) * kernel[0] + GetWeight(x, y - 1) * kernel[1] + GetWeight(x + 1,  y - 1) * kernel[2] +
           GetWeight(x - 1, y    ) * kernel[3] + GetWeight(x, y    ) * kernel[4] + GetWeight(x + 1,  y    ) * kernel[5] +
           GetWeight(x - 1, y + 1) * kernel[6] + GetWeight(x, y + 1) * kernel[7] + GetWeight(x + 1,  y + 1) * kernel[8];
}
/*
float VectorFieldMap::GetAverageValue(int x, int y, int kernelSize)
{
    float value = 0.f;
    int minSz = -kernelSize / 2, maxSz = kernelSize / 2;
    for (int j = minSz; j < maxSz; j++)
        for (int i = minSz; i < maxSz; i++)
            value += GetWeight(x + i, y + j);
    return value / float(kernelSize * kernelSize);
}
*/
void VectorFieldMap::GaussianBlur()
{
    float *data = new float [mSize.width * mSize.height];
    for (int j = 0; j < mSize.height; j++)
        for (int i = 0; i < mSize.width; i++)
            data[j * mSize.width + i] = GetGaussianBlurredValue3(i, j);
    for (int i = 0; i < mSize.width * mSize.height; i++)
        mData[i].weight = data[i];
    delete [] data;
}

void VectorFieldMap::Smooth(int kernelSize)
{
    switch (kernelSize)
    {
    case 9: GaussianBlur();
    case 8: GaussianBlur();
            GaussianBlur();
    case 7: GaussianBlur();
    case 6: GaussianBlur();
    case 5: GaussianBlur();
    case 4: GaussianBlur();
    case 3: GaussianBlur(); break;
    }
}

Vector2 VectorFieldMap::GetVector(int x, int y, int kernelSize)
{
    Vector2 vector;
    int halfKSize = kernelSize / 2;
    int minSz = -halfKSize, maxSz = halfKSize;
    float thisWeight = GetWeight(x, y);
    for (int j = minSz; j < maxSz; j++)
    {
        for (int i = minSz; i < maxSz; i++)
        {
            float weight = thisWeight - GetWeight(x + i, y + j);
            vector += Vector2((halfKSize - i) * weight, (halfKSize - j) * weight);
        }
    }                
    return vector / float(kernelSize * kernelSize);
}

void VectorFieldMap::CalculateVectors(int kernelSize)
{
    for (int j = 0; j < mSize.height; j++)
        for (int i = 0; i < mSize.width; i++)
            mData[j * mSize.width + i].vector = GetVector(i, j, kernelSize);
}

void VectorFieldMap::Normalize()
{
    VectorFieldItem item;
    for (int j = 0; j < mSize.height; j++)
    {
        for (int i = 0; i < mSize.width; i++)
        {
            item = mData[j * mSize.width + i];
            float length = item.vector.length();
            if (length > 0.f)
            {
                item.weight *= length;
                item.vector /= length;
            }
            mData[j * mSize.width + i] = item;
        }
    }
}

    CompactVectorFieldMap::CompactVectorFieldMap(const VectorFieldMap &src) : mSize(src.GetSize()), mElementSize(src.GetElementSize()), mOffset(src.GetOffset())
    {
        mData = new unsigned short [mSize.height * mSize.width];
        memset(mData, 0, sizeof(unsigned short) * mSize.height * mSize.width);
        //printf("Compact---\n");
        for (int j = 0; j < mSize.height; j++)
        {
            for (int i = 0; i < mSize.width; i++)
            {
                mData[j * mSize.width + i] = GetUint16(src.GetItem(i, j));
                /*
                VectorFieldItem a = src.GetItem(i, j);
                unsigned short id = GetUint16(a);
                VectorFieldItem b = GetItem(id);
                Vector3 d(fabsf(a.vector.x - b.vector.x),
                          fabsf(a.vector.y - b.vector.y),
                          fabsf(a.weight   - b.weight  ));
                if (d.x > 0.1f || d.y > 0.1f || d.z > 2.f)
                {
                    printf("%.2f %.2f %.2f\n", d.x, d.y, d.z);
                    VectorFieldItem a = src.GetItem(i, j),
                                    b = GetItem(id);
                }
                */
            }
        }
        //printf("\n\n");
    }

    CompactVectorFieldMap::~CompactVectorFieldMap()
    {
        delete [] mData;
    }

    unsigned short CompactVectorFieldMap::GetUint16(const VectorFieldItem &p)
    {
        int x = int(p.vector.x * 7.f + 7.f); // (-7, +7)
        int y = int(p.vector.y * 7.f + 7.f);
        int angle = (x << 4) | y;
        int w = int(p.weight * 0.5f); // how's the range on this?
        return (angle << 8) | w;
    }

    VectorFieldItem CompactVectorFieldMap::GetItem(unsigned short value)
    {
        VectorFieldItem item;
        item.vector.x = (((value >> 12) & 0x0f) - 7) / 7.f;
        item.vector.y = (((value >>  8) & 0x0f) - 7) / 7.f;
        int w = (char)(value & 0xff);
        item.weight = float(w * 2);
        item.vector.normalize();
        return item;
    }