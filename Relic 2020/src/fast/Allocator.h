#ifndef _FAST_ALLOCATOR_2937830_H
#define _FAST_ALLOCATOR_2937830_H

#include <stdlib.h> // for size_t

class PoolAllocator;

class SmallObjectAllocator
{
    enum { INT_SIZE = sizeof(int) };
    
    enum { MAXIMUM_SIZE = INT_SIZE * 128,
           INCREMENT_SIZE = INT_SIZE,
           INCREMENT_SIZE_SHIFT = (INT_SIZE == 4 ? 2 : 3), // where x >> INCREMENT_SIZE_SHIFT == x / INCREMENT_SIZE, ie floor(log2(INCREMENT_SIZE))
           MAX_BUCKETS = MAXIMUM_SIZE / INCREMENT_SIZE,
           HEADER_SIZE = INT_SIZE,
           ALLOC_PAGE_SIZE = INT_SIZE * 1024 }; // 8k page for 64 bit, 4k for 32 bit

public:

    SmallObjectAllocator();
    ~SmallObjectAllocator();

    void            *Allocate(size_t size);
    void            *Reallocate(void *p, size_t size);
    void             Destroy(void *u);

    static size_t    GetSize(const void *u); // gets size rounded up to the nearest INCREMENT_SIZE

    void             PrintStatistics() const;
    void             DumpStatistics() const;

private:

    PoolAllocator *mAllocators[MAX_BUCKETS];

    static inline bool   IsSameSizeBlock(size_t sizea, size_t sizeb) { return GetBucket(sizea) == GetBucket(sizeb); }
    static inline size_t GetBucket(size_t size)                      { return size >> INCREMENT_SIZE_SHIFT; }
};

extern SmallObjectAllocator _gAllocator;

extern "C" void *soa_malloc(size_t size);
extern "C" void  soa_free(void *p);
extern "C" void *soa_realloc(void *p, size_t size);

// for zlib
extern "C" void *soa_zalloc(void *opaque, size_t items, size_t size);
extern "C" void  soa_zfree(void *opaque, void *p);

// for lua
extern "C" void *lua_soa_allocate(void *ud, void *ptr, size_t osize, size_t nsize);

#define CLASS_NEW_DELETE()                                                     \
inline void *operator new (size_t size) { return _gAllocator.Allocate(size); } \
inline void  operator delete (void *p)  { _gAllocator.Destroy(p); }

#endif  // _FAST_ALLOCATOR_2937830_H
