#ifndef _EXECUTIONHANDLER_H
#define _EXECUTIONHANDLER_H

class AppHandler;
class SpliceHandler;
class PatchHandler;
class SdkHandler;
class ExecutionHandler {
    ExecutionHandler();
    std::shared_ptr<AppHandler> m_appHandler;
    std::shared_ptr<SpliceHandler> m_spliceHandler;
    std::shared_ptr<PatchHandler> m_patchHandler;
    std::shared_ptr<SdkHandler> m_sdkHandler;

   public:
    uintptr_t m_instance;
    uint8_t * m_origMod;
    void regcall setModuleInstance(uintptr_t instance) {
        m_instance = instance;
    }
    static ExecutionHandler *regcall instance();
    static uintptr_t stdcall handleClientThread();
    void init();
    AppHandler *regcall appHandler() { return m_appHandler.get(); }
    SdkHandler *regcall sdkHandler() { return m_sdkHandler.get(); }
    ~ExecutionHandler();
};
#endif
