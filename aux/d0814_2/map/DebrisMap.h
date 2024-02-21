#ifndef _DEBRISMAP_H
#define _DEBRISMAP_H

#include "core/core.h"
#include "fast/Allocator.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

class IGameObject;
class RenderContext;

struct DebrisItem
{
    // how do we condense this down to be more compact?
    // for one, object should be an index into a table, 1 byte
    IGameObject   *object;    
    unsigned char  rotationz; // positive angle mapped from 0-255
    unsigned char  offset[3]; // offset from center of tile
    unsigned char  scale;     // scale mapped to 0-255
    unsigned char  timer;
    
    void Render(const RenderContext &rc, const Vector3 &at);
    void Update(const GameTime &time);
    void Clear();

    inline float GetScale() const // negative scales not supported
    {
        const float INV_16 = 1.f / 16.f;
        return scale * INV_16;
    }
    inline void SetScale(float v) // negative scales not supported
    {
        scale = (unsigned char)(v * 16.0f);
    }
};

class DebrisList // sizeof(DebrisItem) [8bytes] * MAX_DEBRIS [16] = 128 bytes
{
    enum { MAX_DEBRIS = 16, COMPACT_TARGET = 4 };
       
public:
    
    CLASS_NEW_DELETE();
    
    DebrisList();

    void Add(IGameObject *object, const Vector3 &translate, float rotationz, float scale);
    void Compact();
    void Update(const GameTime &time);
    void Render(const RenderContext &rc, const Vector3 &at);
    void Clear();

    int  GetActiveCount() const;
    
private:
    
    DebrisItem mItems[MAX_DEBRIS];
    
    void Add(DebrisItem &debris);
    int  FirstAvailable() const;
};

class DebrisMap
{
public:

    DebrisMap(const core::Size &size, const core::Size &elementSize);
    ~DebrisMap();
    
    void Add(IGameObject *object, const Vector3 &at, float rotation, float scale);
    void Update(const GameTime &time, const core::Rectangle &r);
    void Render(const RenderContext &rc, const core::Rectangle &r);
    void Clear();

private:

    void PrintActiveCount() const;

    int               GetActiveCount(const core::Rectangle &r) const;

    void              Compact(const core::Rectangle &r);
    void              Render(const RenderContext &rc, const core::Point &at);
    DebrisList       &GetList(const core::Point &at);
    const DebrisList &GetList(const core::Point &at) const;

    DebrisList *mData;
    DebrisList  mDefault;
    core::Size  mSize;
    core::Size  mElementSize;
    FILE       *mFile;
};


#endif // _DEBRISMAP_H