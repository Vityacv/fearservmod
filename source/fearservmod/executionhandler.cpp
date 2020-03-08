#include "pch.h"
#include "shared/common_macro.h"
#include "shared/memory_utils.h"
#include "shared/string_utils.h"
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
ExecutionHandler::ExecutionHandler() {
    m_spliceHandler.reset(new SpliceHandler());
    m_patchHandler.reset(new PatchHandler());
    m_appHandler.reset(new AppHandler());
    m_sdkHandler.reset(new SdkHandler());
    auto& appHandler = *m_appHandler;
    appHandler.setSpliceHandler(m_spliceHandler.get());
    appHandler.setPatchHandler(m_patchHandler.get());
    appHandler.setSdkHandler(m_sdkHandler.get());
    m_sdkHandler->setAppHandler(m_appHandler.get());
    if (appHandler.isServer()) {
        appHandler.init();
    } else {
        CloseHandle(CreateThread(0, 0x1000,
                                 reinterpret_cast<LPTHREAD_START_ROUTINE>(
                                     ExecutionHandler::handleClientThread),
                                 0, 0, 0));
    }
}

static ExecutionHandler *g_execHandler = nullptr;
ExecutionHandler *ExecutionHandler::instance() {
    ExecutionHandler **execHandler = &g_execHandler;
    if (!*execHandler) *execHandler = new ExecutionHandler();
    return *execHandler;
}

uintptr_t ExecutionHandler::handleClientThread() {
    ExecutionHandler *handler = instance();
    handler->appHandler()->initClient();
    return 0;
}

ExecutionHandler::~ExecutionHandler() { /*DBGLOG("destruct");*/
}
