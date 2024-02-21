#ifndef _NETPACK_H
#define _NETPACK_H

#include "core/core.h"
#include <string.h>
#include <string>

template <typename T> inline void Pack(char **to, const T &v)     { memcpy(*to, &v, sizeof(T)); (*to) += sizeof(T); }
template <typename T> inline void Unpack(const char **from, T &v) { memcpy(&v, *from, sizeof(T)); (*from) += sizeof(T); }

inline void PackBools(char **to, bool a, bool b = false, bool c = false, bool d = false, bool e = false, bool f = false, bool g = false, bool h = false)
{
    Pack(to, static_cast<unsigned char>((a ? 128 : 0) | (b ? 64 : 0) | (c ? 32 : 0) | (d ? 16 : 0) | 
                                        (e ?   8 : 0) | (f ?  4 : 0) | (g ?  2 : 0) | (h ?  1 : 0)));
}

inline void UnpackBools(const char **from, bool *a, bool *b = 0, bool *c = 0, bool *d = 0, bool *e = 0, bool *f = 0, bool *g = 0, bool *h = 0)
{
    unsigned char data = 0;
    Unpack(from, data);
    if (a) *a = (data & 128) ? true : false;
    if (b) *b = (data &  64) ? true : false;
    if (c) *c = (data &  32) ? true : false;
    if (d) *d = (data &  16) ? true : false;
    if (e) *e = (data &   8) ? true : false;
    if (f) *f = (data &   4) ? true : false;
    if (g) *g = (data &   2) ? true : false;
    if (h) *h = (data &   1) ? true : false;
}

inline float          UnpackFloat   (const char **from)            { float          v = 0.f; Unpack(from, v); return v; }
inline int            UnpackInt     (const char **from)            { int            v = 0;   Unpack(from, v); return v; }
inline unsigned       UnpackUInt    (const char **from)            { unsigned       v = 0;   Unpack(from, v); return v; }
inline char           UnpackChar    (const char **from)            { char           v = 0;   Unpack(from, v); return v; }
inline unsigned char  UnpackUChar   (const char **from)            { unsigned char  v = 0;   Unpack(from, v); return v; }
inline short          UnpackInt16   (const char **from)            { short          v = 0;   Unpack(from, v); return v; }
inline unsigned short UnpackUInt16  (const char **from)            { unsigned short v = 0;   Unpack(from, v); return v; }
inline unsigned short UnpackUInt16be(const char **from)            { int hi = UnpackUChar(from), lo = UnpackUChar(from); return static_cast<unsigned short>((hi << 8) | lo); }
inline Vector3        UnpackVec3    (const char **from)            { Vector3        v;       Unpack(from, v); return v; }

template <typename T> inline void PackChar    (char **to, T value) { Pack(to, static_cast<char>(value)); }
template <typename T> inline void PackUChar   (char **to, T value) { Pack(to, static_cast<unsigned char>(value)); }
template <typename T> inline void PackInt16   (char **to, T value) { Pack(to, static_cast<short>(value)); }
template <typename T> inline void PackUInt16  (char **to, T value) { Pack(to, static_cast<unsigned short>(value)); }
template <typename T> inline void PackUInt16be(char **to, T value) { PackUChar(to, (value >> 8)); PackUChar(to, value); }
template <typename T> inline void PackInt     (char **to, T value) { Pack(to, static_cast<int>(value)); }
template <typename T> inline void PackUInt    (char **to, T value) { Pack(to, static_cast<unsigned int>(value)); }

inline int            FloatToS32(float v, float factor)            { return static_cast<int>  (v * factor); }
inline float          S32ToFloat(int v, float factor)              { const float inv = 1.0f / factor; return static_cast<float>(v * inv); }
inline short          FloatToS16(float v, float factor)            { return static_cast<short>(v * factor); }
inline float          S16ToFloat(short v, float factor)            { const float inv = 1.0f / factor; return static_cast<float>(v * inv); }
inline char           FloatToS8 (float v, float factor)            { return static_cast<char> (v * factor); }
inline float          S8ToFloat (char v, float factor)             { const float inv = 1.0f / factor; return static_cast<float>(v * inv); }

const float POSITION_FACTOR = 1.0f;   // pixel level accuracy
const float VELOCITY_FACTOR = 128.0f; // this is for a normalized vector
const float ANGLE_FACTOR    = 128.0f / math::PI;

inline unsigned       Vec2ToU32 (const Vector2 &v, float factor)   { return static_cast<unsigned>(((static_cast<int>(v.x * factor) & 0x0000ffff) << 16) | (static_cast<int>(v.y * factor) & 0x0000ffff)); }
inline Vector2        U32ToVec2 (unsigned v, float factor)         { const float inv = 1.0f / factor; return Vector2(static_cast<float>(static_cast<short>(v >> 16)) * inv, static_cast<float>(static_cast<short>(v)) * inv); }
inline unsigned       Vec3ToU32 (const Vector3 &v, float factor)   { return static_cast<unsigned>(((static_cast<int>(v.x * factor) & 0x000000ff) << 16) | ((static_cast<int>(v.y * factor) & 0x000000ff) << 8) | (static_cast<int>(v.z * factor) & 0x000000ff)); }
inline Vector3        U32ToVec3 (unsigned v, float factor)         { const float inv = 1.0f / factor; return Vector3(static_cast<float>(static_cast<char>(v >> 16)) * inv, static_cast<float>(static_cast<char>(v >> 8)) * inv, static_cast<float>(static_cast<char>(v)) * inv); }
inline void           PackPositionXY(char **to, const Vector3 &v)  { Pack(to, Vec2ToU32(Vector2(v.x, v.y), POSITION_FACTOR)); }
inline Vector3        UnpackPositionXY(const char **from)          { Vector2 v(U32ToVec2(UnpackUInt(from), POSITION_FACTOR)); return Vector3(v.x, v.y, 0.f); }
inline void           PackVelocity(char **to, const Vector3 &v)    { Pack(to, Vec3ToU32(v.normal(), VELOCITY_FACTOR) | (FloatToS32(v.length(), POSITION_FACTOR) << 24)); }
inline Vector3        UnpackVelocity(const char **from)            { unsigned value = UnpackUInt(from); return U32ToVec3(value, VELOCITY_FACTOR) * S8ToFloat(static_cast<char>(value >> 24), POSITION_FACTOR); }
inline void           PackAngleS8(char **to, float a)              { Pack(to, FloatToS8(a, ANGLE_FACTOR)); }
inline float          UnpackAngleS8(const char **from)             { return S8ToFloat(UnpackChar(from), ANGLE_FACTOR); }
inline void           PackAngleS16(char **to, float a)             { Pack(to, FloatToS16(a, ANGLE_FACTOR)); }
inline float          UnpackAngleS16(const char **from)            { return S16ToFloat(UnpackInt16(from), ANGLE_FACTOR); }

enum PackType
{ 
    PACK_NULL = 0,
    PACK_LIST,
    PACK_MAP,
    
    PACK_BOOL_TRUE,   PACK_BOOL_FALSE,
    
    PACK_ZERO,
    PACK_INT8,        PACK_UINT8, 
    PACK_INT16,       PACK_UINT16,
    PACK_INT32,       PACK_UINT32,
    PACK_FLOAT,

    PACK_L8_STRING,
    PACK_L16_STRING,
    PACK_L32_STRING,

    PACK_DICTIONARY_STRING = 0x80 /* just has hi bit set */,
};

enum AbstractPackType
{
    ABSTRACT_PACK_NULL = 0,
    ABSTRACT_PACK_TABLE,
    ABSTRACT_PACK_BOOL,
    ABSTRACT_PACK_NUMBER,
    ABSTRACT_PACK_STRING
};

inline AbstractPackType GetAbstractPackType(PackType type)
{
    if ((type & PACK_DICTIONARY_STRING) != 0)
        return ABSTRACT_PACK_STRING;
    switch (type)
    {
    case PACK_NULL:       return ABSTRACT_PACK_NULL;
    case PACK_LIST: 
    case PACK_MAP:        return ABSTRACT_PACK_TABLE;
    case PACK_BOOL_TRUE: 
    case PACK_BOOL_FALSE: return ABSTRACT_PACK_BOOL;
    case PACK_ZERO:
    case PACK_INT8:        
    case PACK_UINT8:
    case PACK_INT16:       
    case PACK_UINT16:
    case PACK_INT32:      
    case PACK_UINT32:
    case PACK_FLOAT:      return ABSTRACT_PACK_NUMBER;
    case PACK_L8_STRING:  
    case PACK_L16_STRING:
    case PACK_L32_STRING: return ABSTRACT_PACK_STRING;       
    }
    // ERROR!
    return ABSTRACT_PACK_NULL;
}

inline bool IsPackTypeNull(char c)   { return c == PACK_NULL; }
inline bool IsPackTypeTable(char c)  { return c >= PACK_LIST && c <= PACK_MAP; }
inline bool IsPackTypeString(char c) { return c >= PACK_L8_STRING || c < 0; }
inline bool IsPackTypeNumber(char c) { return c >= PACK_ZERO && c <= PACK_FLOAT; }
inline bool IsPackTypeBool(char c)   { return c >= PACK_BOOL_TRUE && c <= PACK_BOOL_TRUE; }

class NetDictionary;

void PackString(char **to, const char *str, int size, const NetDictionary *dictionary = 0);
void PackString(char **to, const std::string &str, const NetDictionary *dictionary = 0);
int  UnpackString(const char **from, char *str, int maxsize, const NetDictionary *dictionary = 0);
int  UnpackString(const char **from, std::string &str, const NetDictionary *dictionary = 0);
void PackNumber(char **to, float number);
bool UnpackNumber(const char **from, float *number);

#endif // _NETPACK_H