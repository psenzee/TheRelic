#include "HandleMap.h"

HandleMap::Handle HandleMap::GetHandle(const void *p) const
{ 
    std::hash_map<OpaquePointer, Handle>::const_iterator i = mFrom.find(reinterpret_cast<OpaquePointer>(p));
    return i == mFrom.end() ? 0 : (*i).second;
}

const void *HandleMap::Get(Handle h) const
{ 
    std::hash_map<Handle, OpaquePointer>::const_iterator i = mTo.find(h);
    return reinterpret_cast<const void *>(i == mTo.end() ? 0 : (*i).second);
}

bool HandleMap::Set(Handle h, const void *p)
{
    if (h && p)
    {
        OpaquePointer op = reinterpret_cast<OpaquePointer>(p);
        if (GetHandle(p) || Get(h)) // already set
            return false;
        mFrom[op] = h;
        mTo[h] = op;
        return true;
    }
    return false;
}

bool HandleMap::Remove(Handle h, const void *p)
{
    if (h && p)
    {
        mTo.erase(h); mFrom.erase(reinterpret_cast<OpaquePointer>(p));
        return true;
    }
    return false;
}