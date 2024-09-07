#include "pch.h"
#include "shared/common_macro.h"
#include "executionhandler.h"
// extern "C" void* cdeclcall GetIStringEditMgr();
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>

struct __LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
};

#endif



struct __PEB_LDR_DATA {
    ULONG Length;
    BOOLEAN Initialized;
    HANDLE SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
};


struct __PEB {
    BOOLEAN InheritedAddressSpace;
    BOOLEAN ReadImageFileExecOptions;
    BOOLEAN BeingDebugged;
    BOOLEAN Spare;
    HANDLE Mutant;
    PVOID ImageBaseAddress;
    __PEB_LDR_DATA * Ldr;
};

#include "shared/debug.h"



bool stdcall DllMain(HINSTANCE hDllHandle, uint32_t nReason,
                     void* Reserved) {
    bool bSuccess = true;
    switch (nReason) {
        case DLL_PROCESS_ATTACH: {
            //onattach(hDllHandle,Reserved);
            DisableThreadLibraryCalls(hDllHandle);

            ExecutionHandler* execHandler = ExecutionHandler::instance();
            execHandler->setModuleInstance(reinterpret_cast<uintptr_t>(hDllHandle));
            execHandler->init();
            break;
        }

        case DLL_PROCESS_DETACH:
            //ondetach();
            delete ExecutionHandler::instance();
            break;
    }
    return bSuccess;
}
