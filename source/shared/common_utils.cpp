#include "pch.h"
#include "shared/common_macro.h"
// uintptr_t search(uintptr_t key, size_t block_size, uintptr_t array, uintptr_t
// array_end){
//    do{
//        if(key == *reinterpret_cast<uintptr_t*>(array))
//            return array;
//        array += block_size;
//    } while(array != array_end);
//    return 0;
//}
/*
  The _ASSERT macro calls this function when the test fails.
*/
void regcall _assert(const char *assertion, const char *file,
                     int line) _DECL_NOTHROW {
    // QMessageLogger(file, line, nullptr).fatal("ASSERT: \"%s\" in file %s,
    // line %d", assertion, file, line);
}

/*
  The _ASSERT_X macro calls this function when the test fails.
*/
void regcall _assert_x(const char *where, const char *what, const char *file,
                       int line) _DECL_NOTHROW {
    // QMessageLogger(file, line, nullptr).fatal("ASSERT failure in %s: \"%s\",
    // file %s, line %d", where, what, file, line);
}

//void *regcall Malloc(size_t size) { return ::malloc(size); }

//void regcall Free(void *ptr) { ::free(ptr); }

//void *regcall Realloc(void *ptr, size_t size) { return ::realloc(ptr, size); }

//void *regcall ReallocAligned(void *oldptr, size_t newsize, size_t oldsize,
//                             size_t alignment) {
//    // fake an aligned allocation
//    void *actualptr = oldptr ? static_cast<void **>(oldptr)[-1] : nullptr;
//    if (alignment <= sizeof(void *)) {
//        // special, fast case
//        void **newptr =
//            static_cast<void **>(realloc(actualptr, newsize + sizeof(void *)));
//        if (!newptr) return 0;
//        if (newptr == actualptr) {
//            // realloc succeeded without reallocating
//            return oldptr;
//        }

//        *newptr = newptr;
//        return newptr + 1;
//    }

//    // malloc returns pointers aligned at least at sizeof(size_t) boundaries
//    // but usually more (8- or 16-byte boundaries).
//    // So we overallocate by alignment-sizeof(size_t) bytes, so we're guaranteed
//    // to find a somewhere within the first alignment-sizeof(size_t) that is
//    // properly aligned.

//    // However, we need to store the actual pointer, so we need to allocate
//    // actually size + alignment anyway.

//    void *real = realloc(actualptr, newsize + alignment);
//    if (!real) return 0;

//    uintptr faked = reinterpret_cast<uintptr>(real) + alignment;
//    faked &= ~(alignment - 1);
//    void **faked_ptr = reinterpret_cast<void **>(faked);

//    if (oldptr) {
//        ptrdiff oldoffset =
//            static_cast<char *>(oldptr) - static_cast<char *>(actualptr);
//        ptrdiff newoffset =
//            reinterpret_cast<char *>(faked_ptr) - static_cast<char *>(real);
//        if (oldoffset != newoffset)
//            memmove(faked_ptr, static_cast<char *>(real) + oldoffset,
//                    _Min(oldsize, newsize));
//    }

//    // now save the value of the real pointer at faked-sizeof(void*)
//    // by construction, alignment > sizeof(void*) and is a power of 2, so
//    // faked-sizeof(void*) is properly aligned for a pointer
//    faked_ptr[-1] = real;

//    return faked_ptr;
//}

//void regcall FreeAligned(void *ptr) {
//    if (!ptr) return;
//    void **ptr2 = static_cast<void **>(ptr);
//    free(ptr2[-1]);
//}

//void regcall BadAlloc() {
//    // _THROW(std::bad_alloc());
//}

//void *regcall MallocAligned(size_t size, size_t alignment) {
//    return ReallocAligned(0, size, 0, alignment);
//}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
void stdcall FreeLibraryThread(void *hDllHandle) {
    FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(hDllHandle), 0);
}

__int64 regcall FileSize(const wchar_t *name) {
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (!GetFileAttributesExW(name, GetFileExInfoStandard, &fad)) return -1;
    LARGE_INTEGER size;
    size.HighPart = fad.nFileSizeHigh;
    size.LowPart = fad.nFileSizeLow;
    return size.QuadPart;
}

#else

#endif
