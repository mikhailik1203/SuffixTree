#pragma once

#include <string>

namespace mem_alloc{
    struct AllocHeader 
    {
        static size_t gMemoryUsageCount;
        size_t size_;
    };

    void * MyAlloc(size_t cbSize, char *, UINT);
    void MyDelete(void *p);
    size_t processMemUsage();
}

#if defined(MEM_USAGE_TEST_)
void * _cdecl operator new (size_t cbSize);
void * _cdecl operator new (size_t cbSize, int nAnyIntParam, char *szFile = __FILE__, UINT nLineNo = __LINE__);
void  _cdecl operator delete(void *p);
#endif

