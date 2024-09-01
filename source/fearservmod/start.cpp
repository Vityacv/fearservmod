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




void * m_timeBeginPeriod;
void * m_timeGetTime;
void * m_timeEndPeriod;
#include "shared/debug.h"



extern "C" void __stdcall hideDll(HMODULE hmod);

bool stdcall DllMain(HINSTANCE hDllHandle, uint32_t nReason,
                     void* Reserved) {
    bool bSuccess = true;
    switch (nReason) {
        case DLL_PROCESS_ATTACH: {
            //onattach(hDllHandle,Reserved);
            DisableThreadLibraryCalls(hDllHandle);

            // ReplaceModuleBaseAddress(hDllHandle, GetModuleHandle(L"system32\\winmm.dll"));
            // RemoveDllFromOtherLists(hDllHandle);
            // ChangeDllBaseAddress(hDllHandle, GetModuleHandle(L"system32\\winmm.dll"));
            // RemoveModuleFromPEB(hDllHandle);
            // ReplaceModuleBaseAddress2(hDllHandle);
            // printf("TRYING TO HIDE?");
            // HMODULE hMod = LoadLibrary(L"C:\\Windows\\system32\\winmm.dll");
            // auto hdll2= GetModuleHandle(L"C:\\Windows\\system32\\winmm.dll");
            hideDll(hDllHandle);
            // hideDll(hDllHandle,hdll2);
            // break;
            // printf("TRYING TO LOAD?");
            
            // printf("LOADED?");
            // m_timeBeginPeriod = reinterpret_cast<uintptr_t*>(GetProcAddress(hMod, "timeBeginPeriod"));
            // m_timeGetTime = reinterpret_cast<uintptr_t*>(GetProcAddress(hMod, "timeGetTime"));
            // m_timeEndPeriod = reinterpret_cast<uintptr_t*>(GetProcAddress(hMod, "timeEndPeriod"));
            // break;
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

//extern "C" void* GetIStringEditMgrMy() { return GetIStringEditMgr(); }
// extern "C" uintptr_t timeBeginPeriodMy() { 
// // return ((uintptr_t (__stdcall *)(UINT uPeriod)) m_timeBeginPeriod)(uPeriod); 
// return ((uintptr_t (__stdcall *)()) m_timeBeginPeriod)();
// }
// extern "C" uintptr_t timeGetTimeMy() { return ((uintptr_t (__stdcall *)()) m_timeGetTime)(); }
// extern "C" uintptr_t timeEndPeriodMy() { 
// // return ((uintptr_t (__stdcall *)(UINT uPeriod)) m_timeEndPeriod)(uPeriod); 
// return ((uintptr_t (__stdcall *)()) m_timeEndPeriod)();
// }
