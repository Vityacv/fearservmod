#include "pch.h"
#include "memory.h"
#include "conv.h"
#include "splice.h"
#include "stl.h"
#include "fearsdk.h"
#include "feardata.h"

#include "app.h"
#include "handle.h"

void *GetIStringEditMgrAdr;
unsigned char *hStringEditRuntimeOrig;
extern "C" /*__declspec(dllexport)*/ uintptr_t GetIStringEditMgr() {
  return ((uintptr_t(__stdcall *)())GetIStringEditMgrAdr)();
}


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
                   _T("Rename original \"StringEditRuntime.dll\" to \"StringEditRuntime_.dll\" and try again."),
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
  return  bSuccess;
}
