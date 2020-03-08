#ifdef _MSC_VER
#include <intrin.h>
#include <stdint.h>
#include <exception>

namespace std {
    void __throw_length_error(const char*) {
        exit(1);
    }
    void __cdecl _Xlength_error(const char*) {
        exit(1);
    }
#ifdef _DEBUG
__thiscall _Lockit::~_Lockit() noexcept {
    //exit(1);
}
__thiscall _Lockit::_Lockit(int) noexcept {
    //exit(1);
};

#endif // DEBUG


}
void __cdecl operator delete(void* p, uintptr_t sz) { 
    ::operator delete(p);
}

_VCRTIMP void __cdecl __std_exception_destroy(
    _Inout_ __std_exception_data* _Data
) {
    exit(1);
};
_VCRTIMP void __cdecl __std_exception_copy(
    _In_  __std_exception_data const* _From,
    _Out_ __std_exception_data* _To
) {
    exit(1);
};

#ifdef __cplusplus
extern "C" {
#endif
    void __CxxFrameHandler4() {
        exit(1);
    }
#ifndef _fltused
    //int _fltused{0x9875};
#endif
#ifdef __cplusplus
}
#endif
#else
// non msvc vars
#endif
