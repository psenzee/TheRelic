#ifndef _MOVABLEREGISTRY_H
#define _MOVABLEREGISTRY_H

#include "core/FixedList.h"

class Movable;

class MovableRegistry
{
public:

    int             Register(Movable *movable);
    inline int      Register(int id, Movable *movable) { return (id < 0 || !Set(id, movable)) ? -1 : id; }
    void            Unregister(Movable *movable);
    inline int      ChangeId(int from, int to)         { if (from < 0) return -1; Movable *m = GetAt(from); Remove(from); return to < 0 ? -1 : Register(to, m); }

    inline bool     Set(int id, Movable *movable)      { return id < 0 ? false : mList.Set(id, movable); }
    inline int      Add(Movable *movable)              { return mList.Add(movable); }
    inline bool     Remove(int id)                     { return id < 0 ? false : mList.Remove(id); }
    inline void     Clear()                            { return mList.Clear(); }

    inline Movable *GetAt(int id)                      { return id < 0 ? 0 : mList.GetAt(id); }
    inline int      GetNext(int id)              const { return mList.GetNext(id); }


    inline int      GetNextFree(int id)          const { return mList.GetNextFree(id); }
////////////TEMP////////////
    inline int      Count() const                      { return mList.Count(); }
    inline int      FreeCount() const                  { return mList.FreeCount(); }

    // debug & test
    inline void     PrintActiveList() const            { return mList.PrintActiveList(); }
    inline void     PrintFreeList() const              { return mList.PrintFreeList(); }


private:

    enum { MAX_ITEMS = 32766 };
    FixedList<Movable *, MAX_ITEMS> mList;
};

#endif // _MOVABLEREGISTRY_H