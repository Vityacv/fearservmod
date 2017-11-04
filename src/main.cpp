#include "pch.h"
#include "splice.h"
#include "stl.h"
#include "fearsdk.h"
#include "feardata.h"
#include "app.h"
#include "handle.h"
#include "main.h"

void *GetIStringEditMgrAdr;
unsigned char *hStringEditRuntimeOrig;
extern "C" /*__declspec(dllexport)*/ uintptr_t GetIStringEditMgr() {
  return ((uintptr_t(__stdcall *)())GetIStringEditMgrAdr)();
}

#ifdef __cplusplus
extern "C" {
#endif

BOOLEAN WINAPI DllMain(IN HINSTANCE hDllHandle, IN DWORD nReason,
                       IN LPVOID Reserved) {
  BOOLEAN bSuccess = TRUE;

  //  Perform global initialization.

  switch (nReason) {
    case DLL_PROCESS_ATTACH:
      DisableThreadLibraryCalls(hDllHandle);
      hStringEditRuntimeOrig =
          (unsigned char *)LoadLibrary(_T("StringEditRuntime_.dll"));
      if (!hStringEditRuntimeOrig) {
        MessageBox(0,
                   _T("Rename original \"StringEditRuntime.dll\" to ")
                   _T("\"StringEditRuntime_.dll\" and try again."),
                   _T("Error"), MB_ICONERROR);
        TerminateProcess((HANDLE)-1, 0);
        break;
      }
      GetIStringEditMgrAdr = (void *)GetProcAddress(
          (HMODULE)hStringEditRuntimeOrig, "GetIStringEditMgr");
      if (!GetIStringEditMgrAdr) break;
      // handleThread(hDllHandle);
      CloseHandle(CreateThread(0, 0,
                               (LPTHREAD_START_ROUTINE)handleData::handleThread,
                               hDllHandle, 0, 0));
      break;

    case DLL_PROCESS_DETACH:
      delete handleData::instance();
      break;
  }
  return bSuccess;
}

#ifdef _crt

extern _PVFV *__onexitbegin;
extern _PVFV *__onexitend;

void _init_atexit() {
  // max_atexit_entries = 128;
  __onexitbegin = (_PVFV *)malloc(32 * sizeof(_PVFV));
  *(__onexitbegin) = (_PVFV)NULL;
  __onexitend = __onexitbegin;
}

void _doexit() {
  if (__onexitbegin) {
    while (--__onexitend >= __onexitbegin)
      /*
       * if current table entry is not
       * NULL, call thru it.
       */
      if (*__onexitend != NULL) (**__onexitend)();

    /*
     * free the block holding onexit table to
     * avoid memory leaks.  Also zero the ptr
     * variable so that it is clearly cleaned up.
     */

    free(__onexitbegin);

    __onexitbegin = NULL;
  }
}

BOOL WINAPI DllMainCRT(HINSTANCE hInst, DWORD reason, LPVOID imp) {
  if (reason == DLL_PROCESS_ATTACH) {
    _init_atexit();
    _initterm_e(__xi_a, __xi_z);
    _initterm(__xc_a, __xc_z);
  }

  BOOL ret = DllMain(hInst, reason, imp);

  if (reason == DLL_PROCESS_DETACH) {
    _doexit();
  }

  return ret;
}
#endif

#ifdef __cplusplus
}
#endif
