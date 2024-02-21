#ifndef _ALLOCATOR_H
#define _ALLOCATOR_H

class PoolAllocator;

class SmallObjectAllocator
{
    enum { MAXIMUM_SIZE = 256,
           INCREMENT_SIZE = 8,
           MAX_BUCKETS = MAXIMUM_SIZE / INCREMENT_SIZE,
           HEADER_SIZE = 4,
           PAGE_SIZE = 4 * 1024 }; // 4k page

public:

    SmallObjectAllocator();
    ~SmallObjectAllocator();

    void            *Allocate(int size);
    void            *Reallocate(void *p, int size);
    void             Destroy(void *u);

    static unsigned  GetSize(const void *u); // gets size rounded up to the nearest INCREMENT_SIZE

    void             PrintStatistics() const;

private:

    PoolAllocator *mAllocators[MAX_BUCKETS];

    static inline unsigned IsSameSizeBlock(int sizea, int sizeb) { return GetBucket(sizea) == GetBucket(sizeb); }
    static inline unsigned GetBucket(int size)                   { return (size/* + INCREMENT_SIZE - 1*/) / INCREMENT_SIZE; }
};

extern SmallObjectAllocator _gAllocator;

void *soa_malloc(int size);
void  soa_free(void *p);
void *soa_realloc(void *p, int size);

extern "C" void *lua_soa_allocate(void *ud, void *ptr, size_t osize, size_t nsize);

#define CLASS_NEW_DELETE()                                                     \
inline void *operator new (size_t size) { return _gAllocator.Allocate(size); } \
inline void  operator delete (void *p)  { _gAllocator.Destroy(p); }

#endif  // _ALLOCATOR_H