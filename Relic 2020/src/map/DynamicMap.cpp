#include "DynamicMap.h"
#include "Movable.h"
#include "core/random.h"

#include <algorithm>
#include "core/core_assert.h"

DynamicMap::DynamicMap(const Vector2 &size, const Vector2 &unitSize) 
    : mMap(int(size.x / unitSize.x) + 1, int(size.y / unitSize.y) + 1), mUnit(unitSize), mCollisionFunction(0), mCollisionUser(0)
{
}

Movable *DynamicMap::GetMovableById(int id)
{
    core_assert(id >= 0);
    return id < 0 ? 0 : GetRegistry().GetAt(id);
}

int DynamicMap::GetCountOfTypeAt(int type, const core::Point &at) const
{
    const item_t *list = GetList(at);
    int count = 0;
    while (list)
    {
        if (list->GetTypeId() == type)
            count++;
        list = list->mNext;
    }
    return count;
}

int DynamicMap::GetCountOfType(int type, const core::Rectangle &range)
{
    DynamicMapIterator i(this, range);
    int count = 0;
    while (i.HasNext())
    {
        Movable *m = i.GetNext();
        if (m->GetTypeId() == type)
            count++;
    }
    return count;
}

bool DynamicMap::Collides(const item_t *item, const core::Point &at) const
{
    if (!item || at.x < 0 || at.y < 0 || at.x >= static_cast<int>(mMap.width) || at.y >= static_cast<int>(mMap.height))
        return false;
    const item_t *list = GetList(at);
    float r = item->mRadius;
    Vector3 p(item->GetPosition());
    while (list)
    {
        if (list != item && list->IsCollidable())
        {
            float rr = list->mRadius + r;
            const Vector3 &q = list->GetPosition();
            if (p.distancesq(q) <= rr * rr)
                return true;
        }
        list = list->mNext;
    }
    return false;
}

extern float GetCharacterWeight(Movable *movable);
extern void  SetCharacterWeight(Movable *movable, float weight);

static inline bool DynamicMap_CollideComparator_Desc(DynamicMap::item_t *a, DynamicMap::item_t *b)
{
    float aweight = !a ? 0.f : GetCharacterWeight(a),
          bweight = !b ? 0.f : GetCharacterWeight(b);
    return bweight < aweight;
}

int DynamicMap::GetSortedList(item_t **items, int maxcount, const core::Point &at, int weightSeed)
{
    if (at.x < 0 || at.y < 0 || at.x >= static_cast<int>(mMap.width) || at.y >= static_cast<int>(mMap.height))
        return 0;
    item_t *list = GetList(at);
    int     count = 0;
    while (list)
    {
        if (list->IsCollidable() && !list->IsFixed() && list->IsMovable())
        {
            items[count++] = list;
            if (count >= maxcount)
                return count;
            if (weightSeed >= 0)
            {
                float delta = ((weightSeed + count) % 2) ? -0.00001f : +0.00001f;
                SetCharacterWeight(list, GetCharacterWeight(list) + delta);
            }
        }
        list = list->mNext;
    }
    std::sort(items, items + count, DynamicMap_CollideComparator_Desc);
    return count;
}

static int gUncollideCount = 1;

bool DynamicMap::Uncollide2(item_t *item, const core::Point &at)
{
    gUncollideCount = gUncollideCount + 1;
    enum { MAX_ITEMS = 1024 };
    Movable *list[MAX_ITEMS];
    int count = GetSortedList(list, MAX_ITEMS, at, gUncollideCount);
    return UncollideFromSorted(item, list, count);
}

bool DynamicMap::UncollideFromSorted(item_t *item, item_t **list, int count)
{
    if (!item || !item->IsCollidable() || !item->IsMovable() || item->IsFixed())
        return false;
    float r = item->mRadius;
    Vector3 p(item->GetPosition());
    for (int i = 0; i < count; i++)
    {
        item_t *listItem = list[i];
        if (listItem != item)
        {
            float rr = listItem->mRadius + r;
            const Vector3 &q = listItem->GetPosition();
            float dpq = p.distancesq(q);
            if (dpq <= rr * rr)
            {
                if (dpq != 0.f)
                {
                    dpq = sqrtf(dpq);
                    float newDistance = (rr - dpq) * 1.1f;
                    Vector3 d(((p - q) / dpq) * newDistance);
                    if (!listItem->IsMovable() || listItem->IsFixed())
                        listItem->SetPositionRaw(p + d);
                    else
                    {
                        // split the difference
                        item->SetPositionRaw(p + d * 0.5f);
                        listItem->SetPositionRaw(q - d * 0.5f);
                    }                     
                }
                return true;
            }
        }
    }
    return false;
}


bool DynamicMap::Uncollide(item_t *item, const core::Point &at)
{
    if (!item || at.x < 0 || at.y < 0 || 
        at.x >= static_cast<int>(mMap.width) || at.y >= static_cast<int>(mMap.height) || 
        !item->IsCollidable() || !item->IsMovable() || item->IsFixed())
        return false;
    item_t *list = GetList(at);
    float r = item->mRadius;
    Vector3 p(item->GetPosition());
    while (list)
    {
        if (list != item && list->IsCollidable())
        {
            float rr = list->mRadius + r;
            const Vector3 &q = list->GetPosition();
            float dpq = p.distancesq(q);
            if (dpq <= rr * rr)
            {
                if (dpq != 0.f)
                {
                    dpq = sqrtf(dpq);
                    float newDistance = (rr - dpq) * 1.1f;
                    Vector3 d(((p - q) / dpq) * newDistance);
                    if (!list->IsMovable() || list->IsFixed())
                        item->SetPositionRaw(p + d);
                    else
                    {
                        // split the difference
                        item->SetPositionRaw(p + d * 0.5f);
                        list->SetPositionRaw(q - d * 0.5f);
                    }                     
                }
                return true;
            }
        }
        list = list->mNext;
    }
    return false;
}

int DynamicMap::GetCollidables(Movable *item, float radius, Movable **items, int maxItems)
{
    if (!item || !item->IsCollidable())
        return 0;
    int count = 0;
    for (DynamicMapIterator iterator(GetAllClosest(item, radius)); iterator.HasNext() && count < maxItems; )
    {
        Movable *movable = iterator.GetNext();
        if (!movable->IsCollidable() /* && .. */)
            items[count++] = movable;
    }
    return count;
}

int DynamicMap::UncollideSq(Movable *item, float radius)
{
    enum { MAX_ITEMS = 1024 };
    Movable *items[MAX_ITEMS];
    int count = GetCollidables(item, radius, items, MAX_ITEMS);
    if (!count) return 0;
    int moved = 0;

    Movable *a = 0, *b = 0;
    for (int j = 0; j < count; j++)
    {
        a = items[j];
        float ra = a->GetRadius();
        Vector3 pa(a->GetPosition());
        bool aIsMovable = a->IsMovable() && !a->IsFixed();
        for (int i = 0; i < count; i++)
        {
            if (i != j) 
            {
                b = items[i];
                bool bIsMovable = b->IsMovable() && !b->IsFixed();
                if (!aIsMovable && !bIsMovable) // if neither are movable, there's nothing we could do anyway
                    continue;
                float rb = b->GetRadius();
                float r = ra + rb;
                Vector3 pb(b->GetPosition());
                float dSq = pa.distancesq(pb);
                if (dSq <= r * r)
                {
                    float d = sqrtf(dSq);
                    float newDistance = (r - d) * 1.1f;
                    Vector3 v(((pa - pb) / d) * newDistance);
                    if (aIsMovable && bIsMovable) // split the difference, could be weighted
                    {                        
                        a->SetPositionRaw(pa + v * 0.5f);
                        b->SetPositionRaw(pb - v * 0.5f);
                    }
                    else if (aIsMovable)
                        b->SetPositionRaw(pb + v);
                    else
                        a->SetPositionRaw(pa + v);
                    moved++;
                }
            }
        }
    }
    return moved;
}

bool DynamicMap::Collides(const item_t *item) const
{
    core::Point     p(Convert(item->mMapPosition));
    core::Rectangle r(CalculateSearchArea(item, item->GetRadius()));
    for (int j = p.y, jsz = p.y + r.height; j < jsz; j++)
        for (int i = p.x, isz = p.x + r.width; i < isz; i++)
            if (Collides(item, core::Point(i, j)))
                return true;
    return false;
}

bool DynamicMap::Uncollide(const core::Rectangle &range)
{
    DynamicMapIterator iterator(this, range);
    bool uncollided = false;
    while (iterator.HasNext())
    {
        item_t *m = iterator.GetNext();
        uncollided |= Uncollide(m);
    }
    return uncollided;
}

bool DynamicMap::Uncollide()
{
    // $TODO THIS IS CURRENTLY DISABLED!!!!
    return false;
    //bool uncollided = false;
    //for (std::vector<item_t *>::iterator i = mList.begin(), e = mList.end(); i != e; ++i)
    //    uncollided |= Uncollide(*i);
    //return uncollided;
    ///*
    //bool uncollided = false;
    //for (std::vector<item_t *>::iterator i = mList.begin(), e = mList.end(); i != e; ++i)
    //    uncollided |= UncollideSq(*i, (*i)->GetRadius()) > 0;
    //return uncollided;
    //* /
}

bool DynamicMap::Uncollide(item_t *item)
{
    core::Point     p(Convert(item->mMapPosition));
    core::Rectangle r(CalculateSearchArea(item, item->GetRadius()));
    Uncollide(item, p);
    bool collided = false;
    for (int j = p.y, jsz = p.y + r.height; j < jsz; j++)
        for (int i = p.x, isz = p.x + r.width; i < isz; i++)
//          if ((p.x != i || p.y != j) && Uncollide (item, core::Point(i, j)))
            if ((p.x != i || p.y != j) && Uncollide2(item, core::Point(i, j)))
                collided |= true;
    return collided;
}

void DynamicMap::Move(item_t *item, const Vector2 &to)
{
    core::Point point_at(Convert(item->mMapPosition)),
                point_to(Convert(to));
    if (point_at != point_to)
    {
        Remove(item, point_at);  // remove from list for point_at
        item->mMapPosition = to; // change item->position to 'to'
        Add(item, point_to);     // place in bucket for point_to
    }
    item->mMapPosition = to;
}

core::Rectangle DynamicMap::CalculateSearchArea(const Movable *movable, float radius) const
{
    const Vector2   point(v2(movable->GetPosition()));
    core::Point     p(Convert(point));
/*
    core::Rectangle rect(p.x, p.y, 1, 1);
    float           d = radius + radius;
    if (d <= mUnit.x && d <= mUnit.y) // case where the diameter is less than a tile size, most common
    {
        const Vector2 at(point.x - p.x * mUnit.x, point.y - p.y * mUnit.y);

        if (at.y >= mUnit.y - radius) { / *rect.y++;* / rect.height++; }
        if (at.y <=           radius) { rect.y--; rect.height++; }
        if (at.x >= mUnit.x - radius) { / *rect.x++;* / rect.width++;  }
        if (at.x <=           radius) { rect.x--; rect.width++;  }

        / *
        if (at.y >= mUnit.y - d) { / *rect.y++;* / rect.height++; }
        if (at.y <=           d) { rect.y--; rect.height++; }
        if (at.x >= mUnit.x - d) { / *rect.x++;* / rect.width++;  }
        if (at.x <=           d) { rect.x--; rect.width++;  }
        * /
    }
    else if (radius <= mUnit.x && radius <= mUnit.y)
    {
        return core::Rectangle(p.x - 1, p.y - 1, 3, 3);
    }
    else // case where the diameter is greater than a tile size, uncommon - not optimized
    {
        rect.width  += static_cast<int>(d / mUnit.x);
        rect.height += static_cast<int>(d / mUnit.y);
        rect.x      -= rect.width  / 2;
        rect.y      -= rect.height / 2;
    }
    return rect;
    */
    return core::Rectangle(p.x - 1, p.y - 1, 3, 3);
}

Movable *DynamicMap::GetClosest(const Movable *me, float radius, predicate_fn fn, void *user)
{
// $TODO radius * 1.5f is a hack 
    radius *= 2.2f;

//  core::Rectangle area = CalculateSearchArea(me, radius);   
    DynamicMapIterator iterator = GetAllClosest(me, radius);
    
    Vector3  meAt(me->GetPosition());
    float    closest        = radius; 
    Movable *closestMovable = 0;
    
    while (iterator.HasNext())
    {
        Movable *item = iterator.GetNext();    
        if (item != me)
        {
            float distance = item->GetPosition().distance(meAt);
            if (distance <= closest && item->IsCollidable() && (!fn || fn(user, item)))
            {
                closest        = distance;
                closestMovable = item;
            }
        }
    }
    return closestMovable;
}

DynamicMapIterator DynamicMap::GetAllClosest(const Movable *me, float radius)
{
    core::Rectangle area = CalculateSearchArea(me, radius);
    return DynamicMapIterator(this, area);
}

DynamicMapIterator DynamicMap::GetAll()
{ 
    return DynamicMapIterator(this, core::Rectangle(0, 0, mMap.width, mMap.height));
}

int DynamicMap::Count() const
{
    // slow.. todo speed this up the right way
    int count = 0;
    for (int j = 0, jsz = mMap.height; j < jsz; j++)
        for (int i = 0, isz = mMap.width; i < isz; i++)
            count += Count(core::Point(i, j));
    return count;
}

int DynamicMap::Count(const core::Point &at) const
{
    const Movable *m = GetList(at);
    return !m ? 0 : m->Count();
}

void DynamicMap::Remove(item_t *item, const core::Point &at)
{
    if (item)
    {
        int id = item->GetId();
        core_assert(id >= 0);
        GetRegistry().Unregister(item);
        item_t *prev = item->mPrev,
               *next = item->mNext;
        if (!prev)
            mMap.set(at.x, at.y, next);
        if (next)
            next->mPrev = prev;
        if (prev)
            prev->mNext = next;
        item->mNext = item->mPrev = 0;
        mRegistry.Unregister(item);
    }
}

void DynamicMap::Add(item_t *item, const core::Point &to)
{
    if (item)
    {
       int id = item->GetId();
       core_assert(id >= 0);
       GetRegistry().Register(id, item);
       item_t *head = mMap.get(to.x, to.y);
       mMap.set(to.x, to.y, item);
       item->mPrev = 0;
       item->mNext = head;
       if (head)
           head->mPrev = item;
       item->mMap = this;
       mRegistry.Register(item);
    }
}

Movable *DynamicMapIterator::GetNextList()
{
    int sz = mArea.width * mArea.height;
    Movable *list = 0;
    do
    {
        if (mAt >= sz)
            return 0;
        core::Point at(mAt % mArea.width + mArea.x, mAt / mArea.width + mArea.y);
        list = mMap->GetList(at);
        mAt++;
    } while (!list);
    return list;
}

// Test
#ifdef WIN32
#include <conio.h>
#include <stdio.h>
#include <vector>
#include <windows.h>
#include <mmsystem.h>

namespace DynamicMapTest
{

enum { TEST_NUMBER = 1000000 };

const float DYNAMICMAPTEST_MAPSIZE  = 128.0f;
const float DYNAMICMAPTEST_UNITSIZE = 16.0f;

void CreateAndAdd(DynamicMap &map, std::vector<Movable *> &list)
{
    core::Random r(1245);
    Movable *item = 0;
    for (int i = 0; i < TEST_NUMBER; i++)
    {
        item = new Movable(Vector3(DYNAMICMAPTEST_MAPSIZE * r.realf(), DYNAMICMAPTEST_MAPSIZE * r.realf(), DYNAMICMAPTEST_MAPSIZE * r.realf()));
        map.Add(item);
        list.push_back(item);
    }
}

void MoveThem(DynamicMap &map, std::vector<Movable *> &list)
{
    core::Random r(5421);
    for (std::vector<Movable *>::iterator i = list.begin(), e = list.end(); i != e; ++i)
        map.Move(*i, Vector2(DYNAMICMAPTEST_MAPSIZE * r.realf(), DYNAMICMAPTEST_MAPSIZE * r.realf()));
}

void RemoveThem(DynamicMap &map, std::vector<Movable *> &list)
{
    for (std::vector<Movable *>::iterator i = list.begin(), e = list.end(); i != e; ++i)
        map.Remove(*i);
}

void DeleteThem(std::vector<Movable *> &list)
{
    for (std::vector<Movable *>::iterator i = list.begin(), e = list.end(); i != e; ++i)
        delete *i;
    list.clear();
}

void test_DynamicMap()
{
    DynamicMap map(Vector2(DYNAMICMAPTEST_MAPSIZE, DYNAMICMAPTEST_MAPSIZE), Vector2(DYNAMICMAPTEST_UNITSIZE, DYNAMICMAPTEST_UNITSIZE));
    std::vector<Movable *> list;
    unsigned time = ::timeGetTime();
    CreateAndAdd(map, list);
    time = ::timeGetTime() - time;
    printf("Count after add    %-8d time %ums\n", map.Count(), time);
    time = ::timeGetTime();
    MoveThem(map, list);
    time = ::timeGetTime() - time;
    printf("Count after move   %-8d time %ums\n", map.Count(), time);
    time = ::timeGetTime();
    RemoveThem(map, list);
    time = ::timeGetTime() - time;
    printf("Count after remove %-8d time %ums\n", map.Count(), time);
    time = ::timeGetTime();
    DeleteThem(list);
    time = ::timeGetTime() - time;
    printf("Count after delete %-8d time %ums\n", map.Count(), time);
    printf("done\n");
    fflush(stdout);
    getch();
}

}
#endif
