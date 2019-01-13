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
extern "C" uintptr_t GetIStringEditMgr();

BOOLEAN WINAPI DllMain(IN HINSTANCE hDllHandle, IN DWORD nReason,
                       IN LPVOID Reserved) {
  BOOLEAN bSuccess = TRUE;

  //  Perform global initialization.

  switch (nReason) {
    case DLL_PROCESS_ATTACH:
    DisableThreadLibraryCalls(hDllHandle);
    GetIStringEditMgrAdr= (void *)GetIStringEditMgr;
     handleData::handleInit(hDllHandle);
      // CloseHandle(CreateThread(0, 0x1000,
      //                          (LPTHREAD_START_ROUTINE)handleData::handleThread,
      //                          0, 0, 0));
    // {
    //   DisableThreadLibraryCalls(hDllHandle);
    //   HANDLE hThr = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)loadOrig, hDllHandle, 0, 0);
    //   if(hThr){
    //     WaitForSingleObject(hThr,INFINITE);
    //     CloseHandle(hThr);
    //   }
    // }
      break;

    case DLL_PROCESS_DETACH:
      delete handleData::instance();
      break;
  }
  return  bSuccess;
}

extern "C" uintptr_t GetIStringEditMgrMy() {
  return ((uintptr_t(__stdcall *)())GetIStringEditMgrAdr)();
}

// void regcall loadOrig(void * arg) {
//       hStringEditRuntimeOrig =
//           (unsigned char *)LoadLibrary(_T("StringEditRuntime_.dll"));
//       if (!hStringEditRuntimeOrig) {
//         MessageBox(0,
//                    _T("Rename original \"StringEditRuntime.dll\" to \"StringEditRuntime_.dll\" and try again."),
//                    _T("Error"), MB_ICONERROR);
//         TerminateProcess((HANDLE)-1, 0);
//         return;
//       }
//       GetIStringEditMgrAdr = (void *)GetProcAddress(
//           (HMODULE)hStringEditRuntimeOrig, "GetIStringEditMgr");
//       if (!GetIStringEditMgrAdr) return;
//       // handleThread(arg);
//       CloseHandle(CreateThread(0, 0,
//                                (LPTHREAD_START_ROUTINE)handleData::handleThread,
//                                arg, 0, 0));
// }

