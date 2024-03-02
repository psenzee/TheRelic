#include "Allocator.h"
#include "Allocators.h"

#include <string.h>
#include <stdio.h>

#include <new>

#include <vector>
#include <exception>
    
class Page;
    
class PageList : public std::vector<Page *> {};        

class Page
{
public:
    inline Page(size_t items, size_t item_size) : m_item_size(item_size)
    {
        m_end = (m_current = m_data = new char [items * item_size]) + items * item_size;
        memset(m_current, 0, items * item_size);
    }
    inline ~Page()                              { delete [] m_data; }
    inline void *Create()                       { void *data = m_current; m_current += m_item_size; return data; }
    inline bool  Full()                   const { return m_current >= m_end; }
private:
    char   *m_data, *m_current, *m_end;
    size_t  m_item_size;
};

class PageAllocator
{
    PageList   m_pages;
    Page      *m_current;

public:

    size_t pages, items_per_page, item_size;

    PageAllocator(size_t items_per_page, size_t item_size);
    ~PageAllocator();

    void   *Create();
    inline size_t  GetPageCount() const { return m_pages.size(); }
};

PageAllocator::PageAllocator(size_t items_per_page, size_t item_size) 
    : m_current(0), pages(0), items_per_page(items_per_page), item_size(item_size)
{
}

PageAllocator::~PageAllocator()
{
    for (PageList::iterator i = m_pages.begin(), e = m_pages.end(); i != e; ++i)
        if (*i) delete *i;
    m_pages.clear();
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
    {
#ifdef WIN32
        throw std::exception("already deleted!!");
#else
        throw std::exception();        
#endif
    }
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
            "  outstanding objects %lu\n"
            "  total allocated     %lu\n"
            "  total freed         %lu\n"
            "  free list count     %lu\n", count, allocated, freed, free);
}

size_t PoolAllocator::GetPageCount() const
{
    return allocator->GetPageCount();
}

void PoolAllocator::DumpStatistics(FILE *file) const
{
    fprintf(file, "MEM: size %3lu outstanding %5lu total %5lu freed %5lu free %5lu | pages %5lu\n", allocator->item_size, count, allocated, freed, free, allocator->GetPageCount());
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
            "  outstanding objects %lu\n"
            "  total allocated     %lu\n"
            "  total freed         %lu\n", count, allocated, freed);
}

SmallObjectAllocator::SmallObjectAllocator()
{
    for (int i = 0; i < MAX_BUCKETS; i++)
        mAllocators[i] = new PoolAllocator(ALLOC_PAGE_SIZE, i * INCREMENT_SIZE + INCREMENT_SIZE + HEADER_SIZE);
}

SmallObjectAllocator::~SmallObjectAllocator()
{
    for (int i = 0; i < MAX_BUCKETS; i++)
    {
        delete mAllocators[i];
        mAllocators[i] = 0;
    }
}

size_t SmallObjectAllocator::GetSize(const void *u) // gets size rounded up to the nearest INCREMENT_SIZE
{
    const void *data  = (const char *)u - HEADER_SIZE;
    size_t index = *(const size_t *)data;
    return index * INCREMENT_SIZE;
}

void *SmallObjectAllocator::Allocate(size_t size)
{
    if (!size) return 0;
    size_t index = GetBucket(size);
    void  *data  = 0;
    if (index < MAX_BUCKETS) {
        data = mAllocators[index]->Create();
    } else {
        data = malloc(size + HEADER_SIZE);
    }
    *(size_t *)data = index;
    return (char *)data + HEADER_SIZE;
}

void *SmallObjectAllocator::Reallocate(void *p, size_t size)
{
    if (!size) { Destroy(p); return 0; }
    if (!p) return Allocate(size);
    if (IsSameSizeBlock(GetSize(p), size))
        return p;
    void *nmem = Allocate(size);
    size_t osize = GetSize(p);
    size_t sz = size < osize ? size : osize;
    try {
        memcpy(nmem, p, sz);
    } catch (...) {
        printf("SOA ERROR! exception in memcpy in Reallocate! osize=%lu, nsize=%lu", osize, size);
    }
    try {
        Destroy(p);
    } catch (...) {
        printf("SOA ERROR! exception in Destroy in Reallocate! osize=%lu, nsize=%lu", osize, size);
    }
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

void SmallObjectAllocator::DumpStatistics() const
{
#ifdef WIN32    
    FILE *file = fopen("allocations.txt", "w");
    fprintf(file, "SmallObjectAllocator allocations -\n");
    size_t pages = 0;
    for (int i = 0; i < MAX_BUCKETS; i++)
    {
        PoolAllocator *pa = mAllocators[i];
        pa->DumpStatistics(file);
        pages += pa->GetPageCount();
    }
    fprintf(file, "Pages: %u\n", pages);
    fclose(file);
#endif
}

SmallObjectAllocator _gAllocator;

extern "C" void *soa_malloc(size_t size)            { return _gAllocator.Allocate(size); }
extern "C" void  soa_free(void *p)                  { _gAllocator.Destroy(p); }
extern "C" void *soa_realloc(void *p, size_t size)  { return _gAllocator.Reallocate(p, size); }

extern "C" void *lua_soa_allocate(void *ud, void *ptr, size_t osize, size_t nsize)
{
    // ignore ud and osize
    return soa_realloc(ptr, nsize);
}

extern "C" void *soa_zalloc(void *opaque, size_t items, size_t size)
{ 
    return soa_malloc(items * size);
}

extern "C" void soa_zfree(void *opaque, void *p)
{ 
    soa_free(p);
}
