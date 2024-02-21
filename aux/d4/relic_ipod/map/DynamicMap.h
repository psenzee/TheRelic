#ifndef _DYNAMICMAP_H
#define _DYNAMICMAP_H

#include "core/core.h"
#include "core/map2d.h"
#include "Movable.h"
#include "MovableRegistry.h"
#include "MarkList.h"

#include <vector>

class DynamicMapIterator;

class DynamicMap
{
public:

    typedef Movable item_t;
    typedef void (*collide_fn)(void *user, Movable *a, Movable *b);
    typedef bool (*predicate_fn)(void *user, Movable *a);

    DynamicMap(const Vector2 &size, const Vector2 &unitSize);

    void                                Move(item_t *item, const Vector2 &to);

    inline void                         SetCollisionAction(collide_fn fn, void *user) { mCollisionFunction = fn; mCollisionUser = user; }

    inline void                         Remove(item_t *item)                   { Remove(item, Convert(item->mMapPosition)); }
    inline void                         Add(item_t *item)                      { Add(item, Convert(item->mMapPosition)); }    
    int                                 Count(const core::Point &at)     const;    
    int                                 Count()                          const;

    inline item_t                      *GetList(const core::Point &at)         { return mMap.get(at.x, at.y); }
    inline const item_t                *GetList(const core::Point &at)   const { return mMap.get(at.x, at.y); }

    Movable                            *GetMovableById(int id);
    inline int                          GetNextFreeId(int id = -1)       const { return GetRegistry().GetNextFree(id); }

    bool                                Collides(const item_t *item) const;
    bool                                Collides(const item_t *item, const core::Point &p) const;

    bool                                Uncollide(item_t *item);
    bool                                Uncollide(item_t *item, const core::Point &p);
    int                                 UncollideSq(Movable *item, float radius);

    bool                                Uncollide(const core::Rectangle &range);
    // $TODO CURRENTLY DISABLED!!
    bool                                Uncollide();

    core::Rectangle                     CalculateSearchArea(const Movable *movable, float radius) const;
    Movable                            *GetClosest(const Movable *me, float radius, predicate_fn fn = 0, void *user = 0);
    DynamicMapIterator                  GetAllClosest(const Movable *me, float radius);

    MovableRegistry                    &GetRegistry()                          { return mRegistry; }
    const MovableRegistry              &GetRegistry()                    const { return mRegistry; }
    MarkList                           &GetChanged()                           { return mChanged; }
    const MarkList                     &GetChanged()                     const { return mChanged; }

private:
    
    friend class DynamicMapIterator;

    int GetCollidables(Movable *item, float radius, Movable **items, int maxItems);
    inline core::Point Convert(const Vector2 &to) const { return core::Point(int((to.x + mUnit.x * 0.5f) / mUnit.x), int((to.y + mUnit.y * 0.5f) / mUnit.y)); }

    void Remove(item_t *item, const core::Point &at);
    void Add(item_t *item, const core::Point &to);

    Map2d<item_t *>       mMap;
    Vector2               mUnit;
    MovableRegistry       mRegistry;
    MarkList              mChanged;

    collide_fn            mCollisionFunction;
    void                 *mCollisionUser;
};

class DynamicMapIterator
{
public:
    
    inline DynamicMapIterator(DynamicMap *map, const core::Rectangle &r) : mMap(map), mArea(r), mAt(0), mNext(0)
    {
        mNext = GetNextList();
    }
    
    inline bool HasNext() const { return mNext != 0; }

    inline Movable *GetNext()
    {
        if (!mNext)
            return 0;
        Movable *movable = mNext;
        mNext = mNext->mNext;
        if (!mNext)
            mNext = GetNextList();
        return movable;
    }

private:

    Movable *GetNextList();

    DynamicMap      *mMap;
    core::Rectangle  mArea;
    int              mAt;
    Movable         *mNext;
};

namespace DynamicMap_Test
{
void test_DynamicMap();
}

#endif // _DYNAMICMAP_H