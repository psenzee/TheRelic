#ifndef _ALLOCATORS_H
#define _ALLOCATORS_H

#include <string.h>
#include <vector>
#include <new>
#include <exception>

class PageAllocator;

class PoolAllocator
{
    struct Item
    {
        int   magic;
        Item *next;
    };

    Item *m_free;
    enum  { FREE_MAGIC = 0x0A11F0EE };

    static size_t m_item_size(size_t item_size);

    PageAllocator *allocator;

public:

    size_t count, allocated, freed, free;

    PoolAllocator(size_t page_size, size_t item_size);
    ~PoolAllocator();

    void *Create();
    void  Destroy(void *u);

    void  PrintStatistics() const;
};

class ReferenceAllocator
{
public:

    size_t count, allocated, freed, item_size;

    ReferenceAllocator(size_t, size_t item_size);

    void *Create();
    void  Destroy(void *u);

    void  PrintStatistics() const;
};

#endif  // _ALLOCATORS_H