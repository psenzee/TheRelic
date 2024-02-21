#ifndef _FIXEDBITMAP_H
#define _FIXEDBITMAP_H

template <int Bits>
class FixedBitMap
{
public:

    enum { BITS = Bits, BYTES = (BITS + 7) / 8 };

    char data[BYTES];

    inline FixedBitMap(bool u = false)                        { clear(u); }

    inline void                 clear(bool u = false)         { memset(data, u ? 0xff : 0, sizeof(data)); }
    inline bool                 get(unsigned i)         const { return (data[i >> 3] & (1 << (i & 7))) > 0; }
    inline void                 set(unsigned i, bool u)       { char &c = data[i >> 3], m = 1 << (i & 7); c = (u ? (c | m) : (c & ~m)); }
    inline int                  bytes()                 const { return BYTES; }
    inline int                  bits()                  const { return BITS; }
    inline unsigned char       *ptr(unsigned i)               { return data + (x >> 3); }
    inline const unsigned char *ptr(unsigned i)         const { return data + (x >> 3); }
};

#endif // _FIXEDBITMAP_H