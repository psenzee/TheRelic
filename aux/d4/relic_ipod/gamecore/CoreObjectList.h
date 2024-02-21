#ifndef _COREOBJECTLIST_H
#define _COREOBJECTLIST_H

#include "IObjectList.h"

#include <vector>

class CoreObjectList : public IObjectList
{
public:

    CoreObjectList();
    CoreObjectList(const std::vector<IGameObject *> &objects);
    ~CoreObjectList();

    void               Add(IGameObject *o);
    IGameObject       *GetAt(int i);
    const IGameObject *GetAt(int i) const;
    void               Clear();

    // implementation of IObjectList
    int                GetObjectCount() const;
    IGameObject      **GetObjects();

private:
    
    std::vector<IGameObject *> mObjects;
};

#endif // _COREOBJECTLIST_H