#pragma once

#include <string>


namespace mem_alloc{
    struct AllocHeader 
    {
        static size_t gMemoryUsageCount;
        size_t size_;
    };

    void * MyAlloc(size_t cbSize);
    void MyDelete(void *p);
    size_t processMemUsage();
}

#if defined(MEM_USAGE_TEST_)
void * operator new (size_t cbSize);
void * operator new (size_t cbSize, const std::nothrow_t&);
void * operator new (size_t cbSize, int nAnyIntParam, char *szFile, unsigned nLineNo);
void  operator delete(void *p) noexcept;
void  operator delete(void *p, const std::nothrow_t&) noexcept;


void *__wrap_malloc(size_t size);
void __wrap_free(void *ptr);

#endif

