#include "Movable.h"
#include "DynamicMap.h"

const float DEFAULT_MOVABLE_RADIUS = 40.0f;

float Movable::sDefaultRadius = DEFAULT_MOVABLE_RADIUS;

void Movable::SetId(int id)
{
    if (mId != id && mMap)
    {
        if (id != -1) mMap->GetRegistry().ChangeId(mId, id);
        else          mMap->GetRegistry().Unregister(this);
    }
    mId = id;
}

void Movable::MoveTo(const Vector3 &p, bool raw)
{
    if (p != GetPosition())
    {
        if (mMap)
            ((DynamicMap *)mMap)->Move(this, Vector2(p.x, p.y));
        mMapPosition = Vector2(p.x, p.y);
    }
    if (raw) mPositionVelocity.SetPositionRaw(p);
    else     mPositionVelocity.SetPosition(p);
}

void Movable::SetDynamicMap(DynamicMap *dm)
{ 
    if (mMap != dm)
    { 
        if (mMap)
            mMap->Remove(this);
        mMap = dm;
        if (mMap)
            mMap->Add(this);
    }
}

void Movable::MarkChanged()
{
    int id = GetId();
    if (mMap && id != -1)
        mMap->GetChanged().Mark(id);
}

bool Movable::HasChanged() const
{
    int id = GetId();
    return (!mMap || id == -1) ? false : mMap->GetChanged().HasMark(id);
}

int Movable::Count() const
{
    int count = 0;
    const Movable *list = this;
    while (list)
    {
        count++;
        list = list->mNext;
    }
    return count;
}