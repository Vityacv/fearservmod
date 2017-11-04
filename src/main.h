#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#ifdef UNICODE
#define __tgetmainargs __wgetmainargs
int __cdecl __wgetmainargs(int *_Argc, WCHAR ***_Argv, WCHAR ***_Env,
                           int _DoWildCard, STARTUPINFO *_StartInfo);
#else
#define __tgetmainargs __getmainargs
int __cdecl __getmainargs(int *_Argc, char ***_Argv, char ***_Env,
                          int _DoWildCard, STARTUPINFO *_StartInfo);
#endif

#ifdef _crt
typedef void(__cdecl *_PVFV)();
typedef int(__cdecl *_PIFV)(void);
typedef void(__cdecl *_PVFI)(int);
extern _PIFV __xi_a[], __xi_z[];
extern _PVFV __xc_a[], __xc_z[], __xp_a[], __xp_z[], __xt_a[], __xt_z[];

void __cdecl _initterm(_PVFV *, _PVFV *);
int __cdecl _initterm_e(_PIFV *, _PIFV *);
void _init_atexit();
void _doexit();
#endif
#ifdef __cplusplus
}
#endif
