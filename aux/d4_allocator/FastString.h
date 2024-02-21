#ifndef _FASTSTRING_H
#define _FASTSTRING_H

#include <string>

#include "FixedString.h"

template <unsigned short SIZE>
class FastString
{
public:

    inline FastString()                               { assign(0); }
    inline FastString(const char *s)                  { assign(s); }
    inline FastString(const char *s, unsigned length) { assign(s, length); }
    inline FastString(const FastString<SIZE> &other)  { assign(other.data(), other.size()); }

    inline FastString &operator=(const FastString<SIZE> &other)
    {
        if (this != &other)
            assign(other.data(), other.size());
        return *this;
    }

    inline size_t      size()  const { return static_cast<size_t>(mSize); }
    inline bool        empty() const { return !mSize; }
    inline void        clear()       { assign(0); }
    inline const char *c_str() const { return mSize >= SIZE ? mDynamic.c_str() : mFixed.c_str(); }
    inline const char *data()  const { return mSize >= SIZE ? mDynamic.data()  : mFixed.data();  }

    inline void assign(const char *s)
    {
        if (!s) s = "";
        assign(s, strlen(s));
    }

    inline void assign(const char *s, size_t size)
    {
        mSize = size;
        if (size >= SIZE) { mFixed.clear();   mDynamic.assign(s, size); }
        else              { mDynamic.clear(); mFixed.assign  (s, size); }
    }

private:

    FixedString<SIZE> mFixed;
    std::string       mDynamic;
    int               mSize;
};

#endif // _FASTSTRING_H