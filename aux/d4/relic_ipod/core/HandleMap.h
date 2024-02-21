#ifndef _HANDLEMAP_H
#define _HANDLEMAP_H

#include <hash_map>

class HandleMap
{
    typedef intptr_t OpaquePointer;
    
public:

    typedef intptr_t Handle;

    // 0 is not a valid pointer or handle

    Handle      GetHandle(const void *p)     const;
    const void *Get(Handle h)                const;
    bool        Set(Handle h, const void *p);
    
    inline bool Remove(Handle h)         { return Remove(h, Get(h)); }    
    inline bool Remove(const void *p)    { return Remove(GetHandle(p), p); }

private:

    std::hash_map<Handle, OpaquePointer> mTo;
    std::hash_map<OpaquePointer, Handle> mFrom;
    
    bool Remove(Handle h, const void *p);
};

#endif // _HANDLEMAP_H
