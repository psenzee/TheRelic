#ifndef _FILTERS_H
#define _FILTERS_H

#include "core/core.h"
#include "MathUtil.h"

class FilterValue
{
public:

    // filter is how much weight is given to the incoming value, lo and hi are clamping values
    inline FilterValue(float value = 0.f, float filter = 0.1f, float lo = -FLT_MAX, float hi = FLT_MAX) : mValue(value), mLo(lo), mHi(hi), mFilter(filter) {}
    inline FilterValue(const FilterValue &other) : mValue(other.mValue), mLo(other.mLo), mHi(other.mHi), mFilter(other.mFilter) {}

    inline void  Set(float value)    { mValue = value; }
    inline void  Input(float value)  { mValue = math::clamp(mValue * (1.0f - mFilter) + (value * mFilter), mLo, mHi); }
    inline float Get() const         { return mValue; }

private:

    float mValue,
          mLo, mHi,
          mFilter;
};

class FilterAngle
{
public:

    // filter is how much weight is given to the incoming value, lo and hi are clamping values
    inline FilterAngle(float value = 0.f, float filter = 0.1f) : mValue(MathUtil::NormalizeAnglePiNegPi(value)), mFilter(filter) {}
    inline FilterAngle(const FilterAngle &other) : mValue(other.mValue), mFilter(other.mFilter) {}

    inline void  Set(float value)    { mValue = MathUtil::NormalizeAnglePiNegPi(value); }
    inline void  Input(float value)
    {
        float v = mValue + MathUtil::GetAngleDifference(MathUtil::NormalizeAnglePiNegPi(value), mValue);
        mValue = MathUtil::NormalizeAnglePiNegPi(mValue * (1.0f - mFilter) + (v * mFilter));
    }
    inline float Get() const         { return MathUtil::NormalizeAngle2Pi(mValue); }

private:

    float mValue,
          mFilter;
};

class FilterValue3
{
public:

    // filter is how much weight is given to the incoming value, lo and hi are clamping values
    inline FilterValue3(float filter = 0.1f, float lo = -FLT_MAX, float hi = FLT_MAX) : mValue(), mLo(lo), mHi(hi), mFilter(filter) {}
    inline FilterValue3(const FilterValue3 &other) : mValue(other.mValue), mLo(other.mLo), mHi(other.mHi), mFilter(other.mFilter) {}

    inline void           Set(const Vector3 &value)    { mValue = value; }
    inline void           Input(const Vector3 &value)  { mValue = Vector3(math::clamp(mValue.x * (1.0f - mFilter) + (value.x * mFilter), mLo, mHi),
                                                                          math::clamp(mValue.y * (1.0f - mFilter) + (value.y * mFilter), mLo, mHi),
                                                                          math::clamp(mValue.z * (1.0f - mFilter) + (value.z * mFilter), mLo, mHi)); }
    inline const Vector3 &Get() const                  { return mValue; }

private:

    Vector3 mValue;
    float   mLo, mHi,
            mFilter;
};

template <typename T>
class AverageValue
{
public:

    inline AverageValue() : mValue(), mCount(0) {}
    inline AverageValue(const AverageValue &other) : mValue(other.mValue), mCount(other.mCount) {}

    inline void     Clear()               { mValue = T(); mCount = 0; }
    inline void     Input(const T &value) { mValue += value; mCount++; }
    inline T        Get() const           { return mCount ? (mValue / (float)mCount) : T(); }

private:

    T        mValue;
    unsigned mCount;
};

template <typename T, int Depth>
class AverageQueueValue
{
public:

    inline AverageQueueValue() {}
    inline AverageQueueValue(const AverageQueueValue<T, Depth> &other) { for (int i = 0; i < Depth; i++) mValue[i] = other.mValue[i]; }

    inline void     Set(const T &value)   { for (int i = 0; i < Depth; i++) mValue[i] = value; }
    inline void     Input(const T &value) { memmove(&mValue[1], &mValue[0], sizeof(mValue) - sizeof(mValue[0])); mValue[0] = value; }
    inline T        Get() const           { T sum; for (int i = 0; i < Depth; i++) sum += mValue[i]; return sum / (float)Depth; }

private:

    T        mValue[Depth];
};

#endif // _FILTERS_H