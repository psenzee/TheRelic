#ifndef _MARKLIST_H
#define _MARKLIST_H

#include "core/FixedList.h"

class MarkList
{
public:

    inline void     Mark(int id)          { if (id >= 0) mList.Set(id, true); }
    inline bool     HasMark(int id)       { return id < 0 ? false : mList.GetAt(id); }
    inline void     Unmark(int id)        { if (id >= 0) mList.Remove(id); }
    inline void     Clear()               { return mList.Clear(); }
    inline int      GetNext(int id) const { return mList.GetNext(id); }

private:

    enum { MAX_ITEMS = 32766 };
    FixedList<bool, MAX_ITEMS> mList;
};

#endif // _MARKLIST_H