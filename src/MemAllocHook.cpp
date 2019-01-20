#include "stdafx.h"
#include "MemAllocHook.h"

size_t mem_alloc::AllocHeader::gMemoryUsageCount = 0;

size_t mem_alloc::processMemUsage()
{
    return mem_alloc::AllocHeader::gMemoryUsageCount;
}

void * mem_alloc::MyAlloc(size_t cbSize, char *, UINT)
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
    // we need to free our allocator too
    AllocHeader *pHeader = (AllocHeader *)((size_t)p - sizeof(AllocHeader));
    AllocHeader::gMemoryUsageCount -= pHeader->size_;
    free((void *)((size_t)p - sizeof(AllocHeader)));
}

#if defined(MEM_USAGE_TEST_)
// the single override of the module's new operator:
void * _cdecl operator new (size_t cbSize)
//void * _cdecl operator new (size_t cbSize, char *szFile = __FILE__, UINT nLineNo = __LINE__)
{
//#define USEWORKAROUND 1 // uncomment this to get the caller addr
#if USEWORKAROUND
    UINT *EbpRegister ; // the Base Pointer: the stack frame base
    _asm { mov EbpRegister, ebp};
    UINT CallerAddr = *(((size_t *)EbpRegister)+1) ; // the return addr
// if you get a leak, you'll get something like:
//          d:\dev\vc\overnew\overnew.cpp(10189270)
// Break into the debugger. Take the # in parens, put it in Watch window: turn on hex display->it shows addr of caller
// Go to disassembly, put the address in the Address bar hit enter. Bingo: you're at the caller that didn't free!

    //    CallerAddr -=  (size_t)g_hinstDll;    // you can get a relative address if you like: look at the link map
    void *p = MyAlloc(cbSize, __FILE__, CallerAddr); 
    OutputDebugStringf("Op new %x CallerAddr = %x", p, CallerAddr);
#else
    void *p = mem_alloc::MyAlloc(cbSize, __FILE__, __LINE__);  // this line will show for all New operator calls<sigh>
#endif
    return p;
}

void * _cdecl operator new (size_t cbSize, int nAnyIntParam, char *szFile, UINT nLineNo)
{
    void *p = mem_alloc::MyAlloc(cbSize, szFile, nLineNo);  // this line will show for all New operator calls<sigh>
    return p;
}

void  _cdecl operator delete(void *p)
{
    mem_alloc::MyDelete(p);
}

#endif