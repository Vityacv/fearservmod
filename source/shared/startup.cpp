#include "pch.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "shared/startup_utils.h"
#include "shared/string_utils.h"
#include <vector>
#include "stack_container/stack_container.h"
#ifdef __cplusplus
extern "C" {
#endif
extern const PIMAGE_TLS_CALLBACK __dyn_tls_init_callback;

int __cdecl __wgetmainargs(int *_Argc, wchar_t ***_Argv, wchar_t ***_Env,
                           int _DoWildCard, STARTUPINFO *_StartInfo);

int GetMainArgsW(int *_Argc, wchar_t ***_Argv, wchar_t ***_Env) {
    STARTUPINFO _StartInfo;
    return __wgetmainargs(_Argc, _Argv, _Env, /*_CRT_glob*/0, &_StartInfo);
}

inline char ** constructCharArray(wchar_t ** pstr) {
    size_t sz = 0, cnt = 0;
    static constexpr size_t elementsCount = 1000;
    StackVector<size_t, elementsCount> sizes;
    for(; pstr[cnt]; ++cnt) {
        size_t current = StringUtil::getUtf16Size(pstr[cnt]);
        sizes->push_back(current);
        sz += current;
    }
    char ** p = static_cast<char **>(malloc(sz*sizeof(wchar_t) + sizeof(void *) * cnt+1));
    p[cnt] = nullptr;
    size_t offset = 0;
    for(size_t i = 0; i != cnt; ++i) {
        char * buf = reinterpret_cast<char *>(p) + (sizeof(void *) * (cnt+1)) + offset;
        size_t stringSz = sizes[i];
        StringUtil::fromNative(pstr[i], buf, stringSz);
        p[i] = buf;
        offset += stringSz*sizeof(wchar_t);
    }
    return p;
}

int GetMainArgs(int *_Argc, char ***_Argv, char ***_Env) {

    STARTUPINFO _StartInfo;
    wchar_t **_argv;
    wchar_t **_env;
    int result = __wgetmainargs(_Argc, &_argv, &_env, /*_CRT_glob*/0, &_StartInfo);
    if(result == 0) {
        {
            *_Argv = constructCharArray(_argv);
        }
        {
            *_Env = constructCharArray(_env);
        }
        free(_argv);
        free(_env);
    }
    return result;
}

#ifndef _MSC_VER
//void * mingw_app_type;
//void _pei386_runtime_relocator() {}
//void _gnu_exception_handler() {}
#else
void onattach(void * hMod, void * pReserved) {
    pre_c_init();
    _initterm ((_PVFV *)(void *)__xi_a, (_PVFV *)(void *) __xi_z);
    _initterm(__xc_a, __xc_z);
    if (__dyn_tls_init_callback != NULL)
    {
      __dyn_tls_init_callback (hMod, DLL_THREAD_ATTACH, pReserved);
    }

}
#endif
#ifdef __cplusplus
}
#endif

