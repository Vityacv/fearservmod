#ifndef _APPHANDLER_H
#define _APPHANDLER_H
#include "shared/common_macro.h"
#include "shared/memory_utils.h"
#include "shared/string_utils.h"
#include "shared/debug.h"
#ifdef __cplusplus
extern "C" {
#endif
extern volatile uint8_t *g_doConnectIpAdrTramp;
extern void hookOnConnectServerRet();
#ifdef __cplusplus
}
#endif
class SdkHandler;
class AppHandler {
    wchar_t *m_iniBuffer = 0;
    char *m_strMaster;
    char *m_strNs1;
    char *m_strNs2;
    char *m_strMasterAvail;
    char *m_strMotd;


    uint8_t *m_storyModeStruct;
    uint8_t *m_flashlightAdr[2];
    uint8_t m_flashlight1[5] = {0x31, 0xC0, 0x90, 0x90, 0x90};
    uint8_t m_flashlight2[5] = {0x31, 0xC0, 0x90, 0x90, 0x90};
    uint8_t m_coopDoSpawn[5] = {0x31, 0xC0, 0x90, 0x90, 0x90};
    uint8_t *m_SPModeSpawn;
    uint8_t *m_patchHoleKillHives;
    bool m_bFlashlight = 0;
    bool m_bSyncObjects = 0;
    bool m_bBotsMP = 0;
    bool m_bConfigInitialized = 0;
    bool m_bShowIntro = 0;
    bool m_bSettingsLoaded = 0;
    bool m_bCoopDoSpawn = 0;
    PatchHandler *m_patchHandler;
    SpliceHandler *m_spliceHandler;
    SdkHandler *m_sdkHandler;

   public:
    bool m_isHttpMaster = false;
    char** m_skinStr;
    int m_preventNoclip = 0;
    unsigned m_storyModeCnt;
    char flagPatchHoleKillHives=-1;
    bool m_bCoop = 0;
    bool m_bRandWep = 0;
    bool m_bExeType;
    uint8_t skinState;
    bool bCustomSkins = 0;
    bool m_bDisableLights = false;
    uint8_t *m_isMultiplayerGameClient;
    uint8_t *m_RunGameModeXP;
    uint8_t *m_gameClientStruct;
    uint8_t *m_database;
    uintptr_t m_databaseSz;
    uint8_t *m_Server;
    uintptr_t m_ServerSz;
    uint8_t *m_eServer;
    uintptr_t m_eServerSz;
    bool m_haved3d;
    uint8_t *m_Exec;
    uintptr_t m_ExecSz;
    uint8_t *m_Client;
    uintptr_t m_ClientSz;
    int m_playSound = 0;
    AppHandler();
    char *regcall strMaster() { return m_strMaster; }
    SpliceHandler *regcall spliceHandler() { return m_spliceHandler; }
    void regcall setSpliceHandler(SpliceHandler *handler) {
        m_spliceHandler = handler;
    }
    PatchHandler *regcall patchHandler() { return m_patchHandler; }
    void regcall setPatchHandler(PatchHandler *handler) {
        m_patchHandler = handler;
    }
    SdkHandler *regcall sdkHandler() { return m_sdkHandler; }
    void regcall setSdkHandler(SdkHandler *handler) { m_sdkHandler = handler; }
    bool regcall isServer() { return m_eServer != nullptr; }
    static void regcall hookSwitchToSP(SpliceHandler::reg *p);
    static void regcall hookSwitchToMP(SpliceHandler::reg *p);
    static void regcall hookSwitchToModeXP(SpliceHandler::reg *p);
    static void regcall hookStoryModeOn(SpliceHandler::reg *p);
    static void regcall hookDoorTimer(SpliceHandler::reg *p);
    static void regcall hookStoryModeOff(SpliceHandler::reg *p);
    static void regcall hookfRateFix(SpliceHandler::reg *p);
    static void regcall hookOnAddClient_CheckNickname(SpliceHandler::reg *p);
    static void regcall hookEnterSlowMo(SpliceHandler::reg *p);
    static void regcall hookRandomWeapon(SpliceHandler::reg *p);
    static void regcall hookComposeArchives(SpliceHandler::reg *p);
    static void regcall hookLoadObject(SpliceHandler::reg *p);
    static void regcall hookGameMode(SpliceHandler::reg *p);
    static void regcall hookGameMode2(SpliceHandler::reg *p);
    static void regcall hookPickupRndWeapon(SpliceHandler::reg *p);
    void regcall configParse(char * pathCfg);
    static void regcall hookConfigLoad(SpliceHandler::reg *p);
    static void regcall hookConfigLoad2(SpliceHandler::reg *p);
    static void regcall hookLoadGameServer(SpliceHandler::reg *p);
    static void regcall hookSetObjFlags(SpliceHandler::reg *p);
    static void regcall hookStoryModeView(SpliceHandler::reg *p);
    static void regcall hookClientGameMode(SpliceHandler::reg *p);
    static void regcall hookModelMsg(SpliceHandler::reg *p);
    static void regcall hookPlayWave(SpliceHandler::reg *p);
    static void regcall hookClientSettingsLoad(SpliceHandler::reg *p);
    static void regcall hookMID(SpliceHandler::reg *p);
    void regcall setMotd(uint8_t * mod, uintptr_t modSz);
    void regcall configHandle();
    void regcall setCoopDoSpawn(bool state);
    void regcall patchClientServer(uint8_t *mod, uint32_t modSz);
    void regcall loadConfig();
    void regcall patchEndpoints(uint8_t * mod, uint32_t modSz);
    void regcall init();
    void regcall clientPreinitPatches();
    void regcall serverPreinitPatches();
    void regcall initClient();
    uintptr_t regcall getCfgInt(char *pathCfg, char *valStr);
    size_t regcall getGlobalCfgString(wchar_t *pathCfg, wchar_t *valStr,
                                      wchar_t *strDefault, wchar_t *buf,
                                      size_t nSize);
    size_t regcall getGlobalCfgInt(wchar_t *pathCfg, wchar_t *valStr);
    void regcall setMpGame(bool state);
    void regcall setFlashlight(bool state);

};
#endif
