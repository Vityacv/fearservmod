#include "pch.h"
#include "shared/common_macro.h"
#include "shared/string_utils.h"
#include "shared/memory_utils.h"
#include "shared/debug.h"
//#include "shared/unordered_map.hpp"

#include "executionhandler.h"
#include "splice/splice.h"
#include "shared/patch_handler.h"
#include "apphandler.h"
#include "sdkhandler.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


uint8_t * hOrigMod, * hModBase;


void regcall hookLdrGetDllHandleEx(SpliceHandler::reg *p) {
    auto &inst = *ExecutionHandler::instance();
    printf("test2");
    p->state = 1;
    p->argcnt = 5;
    p->tax = ((uintptr_t(__stdcall *)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t))p->origFunc)(
          p->v0, p->v1, p->v2, p->v3, p->v4);
    DBGLOG("YYYYYYYYYYYYY")
    if(p->v4 && (*reinterpret_cast<uintptr_t *>(p->v4) == inst.m_instance)) {
        DBGLOG("ZZZZZZZZZZZZ")
        *reinterpret_cast<uintptr_t *>(p->v4) = inst.m_instance;
    }
    DBGLOG("VVVVVVVVVVVVVVV")
}

void regcall hookLdrLoadDll(SpliceHandler::reg *p) {
    auto &inst = *ExecutionHandler::instance();
    p->state = 1;
    p->argcnt = 4;
    p->tax = ((uintptr_t(__stdcall *)(uintptr_t, uintptr_t, uintptr_t, uintptr_t))p->origFunc)(
          p->v0, p->v1, p->v2, p->v3);
    if( p->v3 && (*reinterpret_cast<uintptr_t *>(p->v3) == inst.m_instance)) {
        DBGLOG("????????")
        *reinterpret_cast<uintptr_t *>(p->v3) = inst.m_instance;
    }
}


ExecutionHandler::ExecutionHandler() {
    m_spliceHandler.reset(new SpliceHandler());
    m_patchHandler.reset(new PatchHandler());
    m_appHandler.reset(new AppHandler());
    m_sdkHandler.reset(new SdkHandler());
}

static ExecutionHandler *g_execHandler = nullptr;
ExecutionHandler *ExecutionHandler::instance() {
    ExecutionHandler **execHandler = &g_execHandler;
    if (!*execHandler) *execHandler = new ExecutionHandler();
    return *execHandler;
}

void ExecutionHandler::init() {
    auto& appHandler = *m_appHandler;
    appHandler.setSpliceHandler(m_spliceHandler.get());
    appHandler.setPatchHandler(m_patchHandler.get());
    appHandler.setSdkHandler(m_sdkHandler.get());
    m_sdkHandler->setAppHandler(m_appHandler.get());
    // auto hMod = GetModuleHandle(_T("ntdll"));
    // DBGLOG("test");
    // m_spliceHandler->spliceUp((void *)GetProcAddr(hMod, "LdrGetDllHandleEx"),
    //    (void *)::hookLdrGetDllHandleEx);
    // m_spliceHandler->spliceUp((void *)GetProcAddr(hMod, "LdrLoadDll"),
    //    (void *)::hookLdrLoadDll);
    // DBGLOG("test2");
    // return;
    if (appHandler.isServer()) {
        appHandler.serverPreinitPatches();
        appHandler.init();
    } else {
        // ExecutionHandler *handler = instance();
        appHandler.clientPreinitPatches();
        CloseHandle(CreateThread(0, 0x1000,
                                 reinterpret_cast<LPTHREAD_START_ROUTINE>(
                                     ExecutionHandler::handleClientThread),
                                 0, 0, 0));
    }
}

uintptr_t ExecutionHandler::handleClientThread() {
    ExecutionHandler *handler = instance();
    handler->appHandler()->initClient();
    return 0;
}

ExecutionHandler::~ExecutionHandler() { /*DBGLOG("destruct");*/
}
