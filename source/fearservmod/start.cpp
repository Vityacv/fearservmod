#include "pch.h"
#include "shared/common_macro.h"
#include "executionhandler.h"
extern "C" void* cdeclcall GetIStringEditMgr();
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


bool stdcall DllMain(HINSTANCE hDllHandle, uint32_t nReason,
                     void* Reserved) {
    bool bSuccess = true;
    switch (nReason) {
        case DLL_PROCESS_ATTACH: {
            //onattach(hDllHandle,Reserved);
            DisableThreadLibraryCalls(hDllHandle);
            ExecutionHandler* execHandler = ExecutionHandler::instance();
            execHandler->setModuleInstance(reinterpret_cast<uintptr_t>(hDllHandle));
            break;
        }

        case DLL_PROCESS_DETACH:
            //ondetach();
            delete ExecutionHandler::instance();
            break;
    }
    return bSuccess;
}

extern "C" void* GetIStringEditMgrMy() { return GetIStringEditMgr(); }
