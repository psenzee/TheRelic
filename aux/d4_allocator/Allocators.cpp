#include "allocator.h"
#include "allocators.h"

#include <string.h>
#include <stdio.h>

#include <vector>
#include <new>
#include <exception>

class Page
{
public:
    inline Page(size_t items, size_t item_size) : m_item_size(item_size) { m_end = (m_current = m_data = new char [items * item_size]) + items * item_size; memset(m_current, 0, items * item_size); }
    inline ~Page()                              { delete [] m_data; }
    inline void *Create()                       { void *data = m_current; m_current += m_item_size; return data; }
    inline bool  Full()                   const { return m_current >= m_end; }
private:
    char   *m_data, *m_current, *m_end;
    size_t  m_item_size;
};

class Page;

class PageAllocator
{
    std::vector<Page *>  m_pages;
    Page                *m_current;

public:

    size_t pages, items_per_page, item_size;

    PageAllocator(size_t items_per_page, size_t item_size);
    ~PageAllocator();

    void *Create();
};

PageAllocator::PageAllocator(size_t items_per_page, size_t item_size) 
    : m_current(0), pages(0), items_per_page(items_per_page), item_size(item_size)
{
}

PageAllocator::~PageAllocator()
{
    for (std::vector<Page *>::iterator i = m_pages.begin(), e = m_pages.end(); i != e; ++i)
        if (*i) delete *i;
}

void *PageAllocator::Create()
{
    if (!m_current || m_current->Full()) { pages++; m_pages.push_back(m_current = new Page(items_per_page, item_size)); }
    return m_current->Create();
}

size_t PoolAllocator::m_item_size(size_t item_size)
{
    return sizeof(Item) > item_size ? sizeof(Item) : item_size;
}

PoolAllocator::PoolAllocator(size_t page_size, size_t item_size) 
    : allocator(0), m_free(0), count(0), free(0), allocated(0), freed(0)
{
    allocator = new PageAllocator(page_size / item_size, m_item_size(item_size));
}

PoolAllocator::~PoolAllocator()
{
    delete allocator;
}

void *PoolAllocator::Create()
{
    void *item = (void *)m_free;
    if (m_free) { m_free = m_free->next; free--; }
    else item = allocator->Create();
    count++;
    allocated++;
    return item;
}

void PoolAllocator::Destroy(void *u)
{
    Item *item = (Item *)u;
    if (item->magic == FREE_MAGIC)
        throw std::exception("already deleted!!");
    item->magic = FREE_MAGIC;
    item->next  = m_free;
    m_free      = item;
    count--;
    free++;
    freed++;
}

void PoolAllocator::PrintStatistics() const
{
    printf("\n"
            "Allocator stats:        \n"
            "  outstanding objects %d\n"
            "  total allocated     %d\n"
            "  total freed         %d\n"
            "  free list count     %d\n", count, allocated, freed, free);
}

ReferenceAllocator::ReferenceAllocator(size_t, size_t item_size) 
    : count(0), allocated(0), freed(0), item_size(item_size)
{
}

void *ReferenceAllocator::Create()
{ 
    allocated++; 
    count++; 
    return malloc(item_size);
}

void ReferenceAllocator::Destroy(void *u)
{ 
    freed++;
    count--;
    free(u);
}

void ReferenceAllocator::PrintStatistics() const
{
    printf("\n"
            "Allocator stats:        \n"
            "  outstanding objects %d\n"
            "  total allocated     %d\n"
            "  total freed         %d\n", count, allocated, freed);
}

SmallObjectAllocator::SmallObjectAllocator()
{
    for (int i = 0; i < MAX_BUCKETS; i++)
        mAllocators[i] = new PoolAllocator(PAGE_SIZE, i * INCREMENT_SIZE + INCREMENT_SIZE + HEADER_SIZE);
}

SmallObjectAllocator::~SmallObjectAllocator()
{
    for (int i = 0; i < MAX_BUCKETS; i++)
    {
        delete mAllocators[i];
        mAllocators[i] = 0;
    }
}

unsigned SmallObjectAllocator::GetSize(const void *u) // gets size rounded up to the nearest INCREMENT_SIZE
{
    const void *data  = (const char *)u - HEADER_SIZE;
    unsigned index = *(const unsigned *)data;
    return index * INCREMENT_SIZE;
}

void *SmallObjectAllocator::Allocate(int size)
{
    if (!size) return 0;
    unsigned index = GetBucket(size);
    void  *data  = index < MAX_BUCKETS ? mAllocators[index]->Create() : malloc(size + HEADER_SIZE);
    *(unsigned *)data = index;
    return (char *)data + HEADER_SIZE;
}

void *SmallObjectAllocator::Reallocate(void *p, int size)
{
    if (!size) { Destroy(p); return 0; }
    if (!p) return Allocate(size);
    if (IsSameSizeBlock(GetSize(p), size))
        return p;
    void *nmem = Allocate(size);
    int osize = GetSize(p);
    int sz = size < osize ? size : osize;
    memcpy(nmem, p, sz);
    Destroy(p);
    return nmem;
}

void SmallObjectAllocator::Destroy(void *u)
{
    if (u)
    {
        void *data  = (char *)u - HEADER_SIZE;
        unsigned index = *(unsigned *)data;
        if (index >= MAX_BUCKETS) free(data);
        else                      mAllocators[index]->Destroy(data);
    }
}

void SmallObjectAllocator::PrintStatistics() const
{
    for (int i = 0; i < MAX_BUCKETS; i++)
    {
        PoolAllocator *pa = mAllocators[i];
        if (pa->allocated)
        {
            printf("Objects sized %d-%d -",
                i * INCREMENT_SIZE, i * INCREMENT_SIZE + INCREMENT_SIZE - 1);
            pa->PrintStatistics();
        }
    }
}

SmallObjectAllocator _gAllocator;

void *soa_malloc(int size)            { return _gAllocator.Allocate(size); }
void  soa_free(void *p)               { _gAllocator.Destroy(p); }
void *soa_realloc(void *p, int size)  { return _gAllocator.Reallocate(p, size); }

extern "C" void *lua_soa_allocate(void *ud, void *ptr, size_t osize, size_t nsize)
{
    // ignore ud and osize
    return soa_realloc(ptr, nsize);
}
