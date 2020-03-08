#ifndef _STARTUP_UTILS_H
#define _STARTUP_UTILS_H
#include "shared/common_macro.h"
#ifdef WIN32
#include <utility>
#include <thread>
#include <chrono>
#ifdef __cplusplus
extern "C" {
#endif


typedef void(__cdecl *_PVFV)();
typedef int(__cdecl *_PIFV)(void);
typedef void(__cdecl *_PVFI)(int);
extern _PVFV *__onexitbegin;
extern _PVFV *__onexitend;
extern _PIFV __xi_a[], __xi_z[];
extern _PVFV __xc_a[], __xc_z[], __xp_a[], __xp_z[], __xt_a[], __xt_z[];

typedef void (*func_ptr) (void);
extern func_ptr __CTOR_LIST__[];
extern func_ptr __DTOR_LIST__[];


extern void _pei386_runtime_relocator (void);

void __cdecl _initterm(_PVFV *, _PVFV *);

static int
pre_c_init (void)
{
  _PVFV *onexitbegin;

  onexitbegin = (_PVFV *) malloc (32 * sizeof (_PVFV));
  __onexitend = __onexitbegin = (_PVFV *) (onexitbegin);

  if (onexitbegin == NULL)
    return 1;
  *onexitbegin = (_PVFV) NULL;
  return 0;
}
//#ifndef _MSC_VER
//void
//__do_global_dtors (void)
//{
//  static func_ptr *p = __DTOR_LIST__ + 1;

//  while (*p)
//    {
//      (*(p)) ();
//      p++;
//    }
//}

//void
//__do_global_ctors (void)
//{
//  unsigned long nptrs = (unsigned long) (ptrdiff_t) __CTOR_LIST__[0];
//  unsigned long i;

//  if (nptrs == (unsigned long) -1)
//    {
//      for (nptrs = 0; __CTOR_LIST__[nptrs + 1] != 0; nptrs++);
//    }

//  for (i = nptrs; i >= 1; i--)
//    {
//      __CTOR_LIST__[i] ();
//    }

//  atexit (__do_global_dtors);
//}
//#endif

void onattach(void * hMod = nullptr,void * pReserved = nullptr);

inline void ondetach(){
    _PVFV * onexitbegin = (_PVFV *)  (__onexitbegin);
    if (onexitbegin)
      {
        _PVFV *onexitend = (_PVFV *)  (__onexitend);
        while (--onexitend >= onexitbegin)
    if (*onexitend != NULL)
      (**onexitend) ();
        free (onexitbegin);
        __onexitbegin = __onexitend = (_PVFV *) NULL;
      }
}
int GetMainArgsW(int *_Argc, wchar_t ***_Argv, wchar_t ***_Env);
int GetMainArgs(int *_Argc, char ***_Argv, char ***_Env);
int regcall MainArgW(int argc, wchar_t *argv[], wchar_t *envp[] = nullptr);
int regcall MainArg(int argc, char *argv[], char *envp[] = nullptr);
int regcall WinMainGui(void * hInstance, void * hPrevInstance,
                       wchar_t *cmd, int nCmdShow);

#ifdef __cplusplus
}
#endif


#ifdef _STARTUP_GUI
inline int WinMainCRT(HMODULE module) {
    STARTUPINFO _StartInfo;
    int _argc;
    wchar_t **_argv;
    wchar_t **_Env;
    int ret = 0;
    if (__wgetmainargs(&_argc, &_argv, &_Env, FALSE, &_StartInfo) == 0) {
        //        wchar_t *cmd = GetCommandLineW();
        //        cmd = cmd + wcslen(_argv[0]) + sizeof(wchar_t);
        //        // if(_argc!=1)cmd=cmd+1;
        //        while (*cmd == L' ') cmd++;
        free(_argv);
        free(_Env);
        ret = WinMainGui(module ? GetModuleHandleW(nullptr) : module, nullptr,
                         nullptr /*cmd*/,
                         _StartInfo.dwFlags & STARTF_USESHOWWINDOW ? _StartInfo.wShowWindow
                                                                   : SW_SHOWDEFAULT);
    }
    return ret;
}
#endif

#ifdef _STARTUP_CONSOLE
//int _CRT_glob;
inline int MainCRTW() {
    onattach();
    int _argc;
    wchar_t** _argv;
    wchar_t** _Env;
    int ret=0;
    if (GetMainArgsW(&_argc, &_argv, &_Env) == 0) {
        ret = MainArgW(_argc, _argv, nullptr);
        free(_argv);
        free(_Env);
    }
    ondetach();
    return ret;
}
inline int MainCRT() {
    onattach();
    int _argc;
    char** _argv;
    char** _Env;
    int ret=0;
    if (GetMainArgs(&_argc, &_argv, &_Env) == 0) {
        ret = MainArg(_argc, _argv, _Env);
        free(_argv);
        free(_Env);
    }
    ondetach();
    return ret;
}
#endif

#endif
#endif
