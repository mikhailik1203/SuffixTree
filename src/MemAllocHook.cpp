#include "MemAllocHook.h"

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <dlfcn.h>

size_t mem_alloc::AllocHeader::gMemoryUsageCount = 0;


size_t mem_alloc::processMemUsage()
{
    return mem_alloc::AllocHeader::gMemoryUsageCount;
}

void * mem_alloc::MyAlloc(size_t cbSize)
{
    AllocHeader::gMemoryUsageCount += cbSize;
    // We allocate a header followed by the desired allocation
    void *p = malloc(sizeof(AllocHeader) + cbSize );
    AllocHeader *pHeader = (AllocHeader *)p;
    pHeader->size_ = cbSize;
    return (void *)((size_t)p + sizeof(AllocHeader));
}

void mem_alloc::MyDelete(void *p)
{
    AllocHeader *pHeader = (AllocHeader *)((size_t)p - sizeof(AllocHeader));
    AllocHeader::gMemoryUsageCount -= pHeader->size_;
    free((void *)((size_t)p - sizeof(AllocHeader)));
}

#if defined(MEM_USAGE_TEST_)
// the single override of the module's new operator:
void * operator new (size_t cbSize)
{

    void *p = mem_alloc::MyAlloc(cbSize);  // this line will show for all New operator calls<sigh>
    return p;
}

void * operator new (size_t cbSize, int nAnyIntParam, char *szFile, unsigned nLineNo)
{
    void *p = mem_alloc::MyAlloc(cbSize);  // this line will show for all New operator calls<sigh>
    return p;
}


void * operator new (size_t cbSize, const std::nothrow_t&)
{
    void *p = mem_alloc::MyAlloc(cbSize);  // this line will show for all New operator calls<sigh>
    return p;
}



void operator delete(void *p) noexcept
{
    mem_alloc::MyDelete(p);
}

void operator delete(void *p, const std::nothrow_t&) noexcept
{
    mem_alloc::MyDelete(p);
}


#endif
