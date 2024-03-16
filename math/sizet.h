#ifndef _SIZET_H
#define _SIZET_H

template <typename T>
class Size2T
{
public:
    T w, h;
      
    inline Size2T() {}
    inline Size2T(const T &v) : w(v), h(v) {}    
    inline Size2T(const T &w, const T &h) : w(w), h(h) {}
    inline Size2T(const Size2T<T> &v) : w(v.w), h(v.h) {}
    
    inline Size2T<T> &operator=(const Size2T<T> &v)       { w = v.w; h = v.h; return *this; }
    
    inline Size2T<T> operator+(const Size2T<T> &v)  const { return Size2T(w + v.w, h + v.h); }
    inline Size2T<T> operator-(const Size2T<T> &v)  const { return Size2T(w - v.w, h - v.h); }
    inline Size2T<T> operator*(const Size2T<T> &v)  const { return Size2T(w * v.w, h * v.h); }
    inline Size2T<T> operator/(const Size2T<T> &v)  const { return Size2T(w / v.w, h / v.h); }

    inline Size2T<T> &operator=(const T &v)               { w = v; h = v; return *this; }

    inline Size2T<T> operator+(const T &v)          const { return Size2T(w + v, h + v); }
    inline Size2T<T> operator-(const T &v)          const { return Size2T(w - v, h - v); }
    inline Size2T<T> operator*(const T &v)          const { return Size2T(w * v, h * v); }
    inline Size2T<T> operator/(const T &v)          const { return Size2T(w / v, h / v); }

    inline Size2T<T> &operator+=(const Size2T<T> &v)      { w += v; h += v; return *this; }
    inline Size2T<T> &operator-=(const Size2T<T> &v)      { w -= v; h -= v; return *this; }
    inline Size2T<T> &operator*=(const Size2T<T> &v)      { w *= v; h *= v; return *this; }
    inline Size2T<T> &operator/=(const Size2T<T> &v)      { w /= v; h /= v; return *this; }
    
    inline Size2T<T> &operator+=(const T &v)              { w += v; h += v; return *this; }
    inline Size2T<T> &operator-=(const T &v)              { w -= v; h -= v; return *this; }
    inline Size2T<T> &operator*=(const T &v)              { w *= v; h *= v; return *this; }
    inline Size2T<T> &operator/=(const T &v)              { w /= v; h /= v; return *this; }
};

template <typename T>
class Size3T
{
public:
    T w, h, d;
      
    inline Size3T() {}
    inline Size3T(const T &v) : w(v), h(v), d(v) {}    
    inline Size3T(const T &w, const T &h, const T &d) : w(w), h(h), d(d) {}
    inline Size3T(const Size3T<T> &v) : w(v.w), h(v.h), d(v.d) {}
    
    inline Size3T<T> &operator=(const Size3T<T> &v)       { w = v.w; h = v.h; d = v.d; return *this; }
    
    inline Size3T<T> operator+(const Size3T<T> &v)  const { return Size3T(w + v.w, h + v.h, d + v.d); }
    inline Size3T<T> operator-(const Size3T<T> &v)  const { return Size3T(w - v.w, h - v.h, d - v.d); }
    inline Size3T<T> operator*(const Size3T<T> &v)  const { return Size3T(w * v.w, h * v.h, d * v.d); }
    inline Size3T<T> operator/(const Size3T<T> &v)  const { return Size3T(w / v.w, h / v.h, d / v.d); }

    inline Size3T<T> &operator=(const T &v)               { w = v; h = v; return *this; }

    inline Size3T<T> operator+(const T &v)          const { return Size3T(w + v, h + v, d + v); }
    inline Size3T<T> operator-(const T &v)          const { return Size3T(w - v, h - v, d - v); }
    inline Size3T<T> operator*(const T &v)          const { return Size3T(w * v, h * v, d * v); }
    inline Size3T<T> operator/(const T &v)          const { return Size3T(w / v, h / v, d / v); }

    inline Size3T<T> &operator+=(const Size3T<T> &v)      { w += v; h += v; d += v; return *this; }
    inline Size3T<T> &operator-=(const Size3T<T> &v)      { w -= v; h -= v; d -= v; return *this; }
    inline Size3T<T> &operator*=(const Size3T<T> &v)      { w *= v; h *= v; d *= v; return *this; }
    inline Size3T<T> &operator/=(const Size3T<T> &v)      { w /= v; h /= v; d /= v; return *this; }
    
    inline Size3T<T> &operator+=(const T &v)              { w += v; h += v; d += v; return *this; }
    inline Size3T<T> &operator-=(const T &v)              { w -= v; h -= v; d -= v; return *this; }
    inline Size3T<T> &operator*=(const T &v)              { w *= v; h *= v; d *= v; return *this; }
    inline Size3T<T> &operator/=(const T &v)              { w /= v; h /= v; d /= v; return *this; }
};

#endif  /** _SIZET_H */