#ifndef _SDKHANDLER_H
#define _SDKHANDLER_H
#include "shared/common_macro.h"
//#include "shared/stl.h"
#include "sdk.h"
class AppHandler;
#include "synchapi.h"
struct playerData {
    float fMovementMultiplier;
    unsigned lastMoveTimeMS;
    unsigned thisMoveTimeMS;
    LTVector lastPos;
    LTVector lastvPathFire;
    unsigned char movePacketCnt;
    unsigned moveLimitExceedCnt;
    unsigned leashBrokenExceedCnt;
    HWEAPON lastWep;
    unsigned onChangeWeaponTime;
    HWEAPON onChangeWeaponHWEAPON;
    CWeapon *lastFireWeapon;
    bool lastFireWeaponDidReload;
    unsigned char lastFireWeaponReload;
    unsigned lastFireWeaponIgnored;
    unsigned lastFireWeaponReloadLength;
    unsigned lastFireWeaponClipMax;
    unsigned lastFireWeaponClipAmmo;
    unsigned lastFireWeaponTimestamp;
    unsigned lastFireWeaponDelay;
    unsigned lastVoteTime;
    unsigned char bResetToUnarmed;
    unsigned moveLimitLastTimeMS;
    unsigned leashBrokenTimeMS;
    bool bLadderInUse;
    bool bIsDead;
    unsigned lastFireWeaponAccuracyFail;
    unsigned char lastFireWeaponAccuracyFailCnt;
    unsigned hAnim;
    unsigned hAnimPenult;
    bool isAimed;
    bool isAnim;
    LTRotation camRot;
};

template <typename T>
void shuffle(T *array, size_t n) {
    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}


//#include "node.h"

// struct IPChunk {
//    IPChunk *next;
//    static constexpr unsigned countMax = 16;
//    IPData buf[countMax];
//    uintptr_t count;
//    static uintptr_t foreach (nodeData<IPChunk> &Chunk, IPData * Block,
//                              uintptr_t(*func)(uintptr_t index, IPChunk
//                              *Chunk,
//                                               IPData *Block));
//    static IPData *add(nodeData<IPChunk> &Chunk, IPData *Block);
//};

// class SdkHandler {
//    static constexpr uint8_t m_mapsIndex[] = {0,  1,  2,  3,  4,  5,  6,  7,
//    8,  9,
//                            10, 11, 12, 13, 14, 15, 16, 17,         18, 19,
//                            20, 21, 57, 50, 51, 62, 63, 64,         59, 60,
//                            61, 58, 54, 55, 56, 31, 32, 33,         34, 35,
//                            36, 37, 38, 39, 40, 41, 42, /*43,*/ 44, 45, 46,
//                            47, 23, 24, 25, 22}; //,26,27,28,29,30,52,53};;
// public:
//    CRITICAL_SECTION g_ipchunkSection;
//    //nodeData<IPChunk> g_ipchunk;
//    AppHandler * m_appHandler;
//    ILTCommon *g_pCommonLT;
//    IDatabaseMgr *g_pLTDatabase;
//    ILTClient *g_pLTClient;
//    ILTServer *g_pLTServer;
//    ILTPhysics *g_pPhysicsLT;
//    ILTModel *g_pModelLT;
//    IGameSpyBrowser *g_pGameSpyBrowser;
//    CWeaponDB *g_pWeaponDB;
//    CServerDB *g_pServerDB;
//    CInterfaceMgr *g_pInterfaceMgr;
//    ClientConnectionMgr *g_pClientConnectionMgr;
//    ServerConnectionMgr *g_pServerConnectionMgr;
//    CServerMissionMgr **g_pServerMissionMgrAdr;
//    CGameDatabaseMgr *g_pGameDatabaseMgr;
//    ServerVoteMgr *g_pServerVoteMgr;
//    CArsenal *g_pArsenal;
//    void *g_pGameModeMgr_instance;
//    void *g_pServerVoteMgr_instance;
//    void *g_pServerVoteMgr_ClearVote;
//    void *g_pServerConnectionMgr_instance;
//    void *g_pGetGameClientData;
//    void *g_pBootWithReason;
//    void *g_pGetNetClientData;
//    void *g_pGameServerShell;
//    void *CArsenal_GetAmmoCount;
//    void *CArsenal_SetAmmo;
//    void *CArsenal_GetCurWeapon;
//    void *CArsenal_DecrementAmmo;
//    void *CCommandMgr_QueueCommand;
//    void *pCmdMgr;
//    HOBJECT startPtObj = 0;
//    HCATEGORY m_hMissionCat;
//    HCATEGORY m_hModelsCat;
//    HCATEGORY m_hCatGlobal;
//    HCATEGORY m_hCatWeapons;
//    HCATEGORY m_hCatGear;
//    HCATEGORY m_hCatAmmo;
//    HRECORD m_hRecGlobal;
//    HRECORD m_hGlobalRecord;
//    HRECORD m_hFastForward;
//    HRECORD m_hPlayer;
//    HWEAPON m_hUnarmedRecord;
//    unsigned char randomWepTable[20];
//    unsigned char currentRandomWeaponInd;
//    HAMMO m_hUnarmedRecordAmmo;
//    unsigned randomWeaponTime = 0;
//    HWEAPON currentRandomWeapon;
//    unsigned ILTServer_GetRealTimeMS;
//    unsigned ILTServer_GetClientAddr;
//    unsigned GameClientData_m_nClientMoveCode;
//    unsigned ObjectCreateStruct_m_Name;
//    unsigned CPlayerObj_m_Inventory;
//    unsigned CPlayerObj_m_Arsenal;
//    unsigned CPlayerObj_m_hClient;
//    unsigned CPlayerObj_m_eStandingOnSurface;
//    unsigned CPlayerObj_m_vLastVelocity;
//    unsigned CPlayerObj_m_PlayerRigidBody;
//    unsigned CPlayerObj_m_bUseLeash;
//    unsigned CPlayerObj_m_vLastClientPos;
//    unsigned CPlayerObj_m_fMoveMultiplier;
//    unsigned CPlayerObj_m_fJumpMultiplier;
//    unsigned CPlayerObj_DropCurrentWeapon;
//    unsigned CPlayerObj_m_ePlayerState;
//    unsigned CPlayerObj_m_fLeashLen;
//    unsigned CPlayerObj_m_fLeashSpringRate;
//    unsigned CPlayerObj_m_fLeashSpring;
//    unsigned CPlayerObj_m_fLeashScale;
//    unsigned CPlayerObj_m_nLastPositionForcedTime;
//    unsigned CArsenal_m_hCurWeapon;
//    unsigned CArsenal_m_pPlayer;
//    unsigned GameModeMgr_ServerSettings;
//    unsigned CCharacter_m_bOnGround;
//    unsigned CCharacter_m_hLadderObject;
//    unsigned ClientConnectionMgr_IGameSpyBrowser;
//    unsigned char CArsenal_m_pAmmo;
//    unsigned char uDT_CRUSH = 0;
//    unsigned char ukPEDropWeapon;
//    unsigned char ukPENextSpawnPoint;
//    unsigned char ukPEPrevSpawnPoint;
//    unsigned char *g_pScreenMultiAdr;
//    unsigned char *g_pScreenMulti = 0;
//    bool freeMovement = 0;
//    unsigned char isXP2 = 0;
//    void *CPlayerObj_UpdateMovement = 0;
//    void *CPlayerObj_m_PlayerRigidBody_Update;
//    void *CPlayerObj_TeleportClientToServerPos;
//    void *CGrenadeProximity_IsEnemy;
//    void *CPlayerObj_ChangeWeapon;
//    void *g_pWeaponDB_GetWeaponData;
//    void *g_pWeaponDB_GetAmmoData;
//    void *g_pWeaponDB_GetInt32;
//    void *g_pWeaponDB_GetBool;
//    void *g_pLTServer_HandleToObject;
//    void *g_pLTServer_GetClientObject;
//    void *g_pGameServerShell_IsPaused;
//    void *g_pGameSpyBrowser_RequestServerList = 0;
//    void *g_pGameSpyBrowser_RequestServerListAdr = 0;
//    void *g_pScreenMulti_RequestServerDetails = 0;
//    LTVector checkPointPos;
//    unsigned checkPointState;
//    void *unknownStruct1;
//    void *unknownFunc2;
//    void *getStructFromString1;
//    void fearDataInit();
//    void fearDataInitServ();
//    void Update();

//    bool checkPlayerStatus(GameClientData *pGameClientData);
//    GameClientData *getGameClientData(HCLIENT client);
//    void BootWithReason(GameClientData *, unsigned char code, char *reason);
//    char *getCurrentLevelName();
//    uint32_t getRealTimeMS();
//    LTRESULT getClientAddr(HCLIENT hClient, uint8_t pAddr[4], uint16_t
//    *pPort); void *HandleToObject(HOBJECT hObj); CPlayerObj * regcall
//    GetPlayerFromHClient(HCLIENT hClient); HWEAPONDATA regcall
//    GetWeaponData(HWEAPON hWep); HAMMODATA regcall GetAmmoData(HAMMO);
//    HLOCALOBJ GetClientObject(HCLIENT hClient);
//    HRECORD regcall hookLoadMaps(uintptr_t index, uintptr_t *all);
//    HRECORD getModelStruct(char *model);
//    IDatabaseMgr *getDatabaseMgr();
//    void updateMovement(CPlayerObj *pPlayerObj);
//    inline void handlePlayerPositionMessage(CPlayerObj *pPlayerObj,
//                                            CAutoMessageBase_Read *pMsg);
//    void getLTServerClient(unsigned char *baseMod, uintptr_t szMod);
//    void setExeType(bool state);
//    void addInternetServer(char *str);
//    static void regcall hookUDPRecvfrom(SpliceHandler::reg *p);
//    static void regcall hookUDPConnReq(SpliceHandler::reg *p);
//    static void regcall hookCheckUDPDisconnect(SpliceHandler::reg *p);
//    void regcall setRespawn(HOBJECT hObjResp);
//    static void stdcall requestMasterServer(bool, unsigned short, char const
//    *); static void regcall fearDataUpdate(); void regcall fearDataInit();
//    static void regcall hookOnConnectServer(SpliceHandler::reg *p);
//    static void regcall hookGetScreenMulti(SpliceHandler::reg *p);
//    static void regcall hookCheckpointEvent(SpliceHandler::reg *p);
//    static void regcall hookRespawnStartPoint(SpliceHandler::reg *p);
//    static void regcall hookPatchHoleKillHives(SpliceHandler::reg *p);
//    static void regcall hookOnMapLoaded(SpliceHandler::reg *p);
//    static void regcall hookConnectUDPClient(SpliceHandler::reg *p);
//    static void regcall hookUseSkin1(SpliceHandler::reg *p);
//    static void regcall hookLoadMaps1(SpliceHandler::reg *p);
//    static void regcall hookLoadMaps2(SpliceHandler::reg *p);
//    static void regcall
//    hookCGrenadeProximity_HandleActivateMsg(SpliceHandler::reg *p);
//    SdkHandler();
//    ~SdkHandler();
//    playerData pPlayerData[64];

//    float getFloatDB(HRECORD hRecord, const char *pszAtt,
//                     uint32_t nValueIndex = 0, float fDef = 0.0f) {
//        return g_pLTDatabase->GetFloat(g_pLTDatabase->GetAttribute(hRecord,
//        pszAtt),
//                                       nValueIndex, fDef);
//    }
//};


class SdkHandler {
    AppHandler *m_appHandler;
    ILTCommon *g_pCommonLT;
    ILTClient *g_pLTClient;

    ILTPhysics *g_pPhysicsLT;
    ILTModel *g_pModelLT;
    IGameSpyBrowser *g_pGameSpyBrowser;
    CServerDB *g_pServerDB;
    CInterfaceMgr *g_pInterfaceMgr;
    ClientConnectionMgr *g_pClientConnectionMgr;
    ServerConnectionMgr *g_pServerConnectionMgr;
    CGameDatabaseMgr *g_pGameDatabaseMgr;
    ServerVoteMgr *g_pServerVoteMgr;
    CArsenal *g_pArsenal;
    void *g_pServerVoteMgr_instance;
    void *g_pServerVoteMgr_ClearVote;
    void *g_pServerConnectionMgr_instance;
    void *g_pGetGameClientData;
    void *g_pBootWithReason;
    void *g_pGameServerShell;
    HOBJECT startPtObj = 0;
    HCATEGORY m_hMissionCat;
    HCATEGORY m_hModelsCat;
    HCATEGORY m_hCatGlobal;
    HRECORD m_hRecGlobal;
    HRECORD m_hPlayer;
    uint32_t ILTServer_GetRealTimeMS;
    uint32_t ILTServer_GetClientAddr;
    uint32_t GameClientData_m_nClientMoveCode;
    uint32_t CPlayerObj_m_hClient;
    uint32_t CPlayerObj_m_eStandingOnSurface;
    uint32_t CPlayerObj_m_vLastVelocity;
    uint32_t CPlayerObj_m_PlayerRigidBody;
    uint32_t CPlayerObj_m_bUseLeash;
    uint32_t CPlayerObj_m_vLastClientPos;
    uint32_t CPlayerObj_m_fMoveMultiplier;
    uint32_t CPlayerObj_m_fJumpMultiplier;
    uint32_t CPlayerObj_DropCurrentWeapon;
    uint32_t CPlayerObj_m_fLeashLen;
    uint32_t CPlayerObj_m_fLeashSpringRate;
    uint32_t CPlayerObj_m_fLeashSpring;
    uint32_t CPlayerObj_m_fLeashScale;
    uint32_t CPlayerObj_m_nLastPositionForcedTime;
    uint32_t CArsenal_m_pPlayer;
    uint32_t GameModeMgr_ServerSettings;
    uint32_t CCharacter_m_hLadderObject;
    uint32_t ClientConnectionMgr_IGameSpyBrowser;
    uint32_t m_timerGamespyServ;
    uint8_t CArsenal_m_pAmmo;
    uint8_t uDT_CRUSH = 0;
    uint8_t ukPEDropWeapon;
    uint8_t *g_pScreenMultiAdr;
    uint8_t *g_pScreenMulti = 0;
    bool m_isDoGameSpy = 0;
    bool m_isMasterServerRequestWorking = false;

    void *CPlayerObj_m_PlayerRigidBody_Update;
    void *CPlayerObj_TeleportClientToServerPos;
    void *g_pWeaponDB_GetWeaponData;
    void *g_pWeaponDB_GetAmmoData;
    void *g_pLTServer_HandleToObject;
    void *g_pLTServer_GetClientObject;
    void *g_pGameServerShell_IsPaused;
    void *g_pGameSpyBrowser_RequestServerList = 0;
    void *g_pGameSpyBrowser_RequestServerListAdr = 0;
    void *g_pScreenMulti_RequestServerDetails = 0;
    void *unknownStruct1;
    void *unknownFunc2;
    void *getStructFromString1;
    uint8_t * g_doConnectIpAdrExit;

    static constexpr uint8_t m_mapsIndex[] = {0,  1,  2,  3,  4,  5,
                                              6,  7,  8,  9,  10, 11,
                                              12, 13, 14, 15, 16, 17,
                                              18, 19, 20, 21, 57, 50,
                                              51, 62, 63, 64, 59, 60,
                                              61, 58, 54, 55, 56, 31,
                                              32, 33, 34, 35, 36, 37,
                                              38, 39, 40, 41, 42, /*43,*/ 44,
                                              45, 46, 47, 23, 24, 25,
                                              22};  //,26,27,28,29,30,52,53};;
   public:
    std::shared_ptr<CRITICAL_SECTION> g_ipchunkSection;
    std::unordered_map<uint32_t, std::unordered_map<uint16_t, uint32_t>> m_ipData;
    uint8_t ukPEPrevSpawnPoint;
    uint8_t ukPENextSpawnPoint;
    HRECORD m_hGlobalRecord;
    CServerMissionMgr **g_pServerMissionMgrAdr;
    void *g_pGameModeMgr_instance;
    unsigned checkPointState;
    uint32_t CPlayerObj_m_Inventory;
    void *CArsenal_SetAmmo;
    void *CArsenal_DecrementAmmo;
    void *g_pWeaponDB_GetInt32;
    void *CPlayerObj_ChangeWeapon;
    void *CGrenadeProximity_IsEnemy;
    void *CArsenal_GetAmmoCount;
    void *g_pWeaponDB_GetBool;
    void *CArsenal_GetCurWeapon;
    CWeaponDB *g_pWeaponDB;
    uint32_t CArsenal_m_hCurWeapon;
    uint32_t CCharacter_m_bOnGround;
    uint32_t CPlayerObj_m_Arsenal;
    LTVector checkPointPos;
    uint32_t CPlayerObj_m_ePlayerState;
    HCATEGORY m_hCatGear;
    HAMMO m_hUnarmedRecordAmmo;
    void *pCmdMgr;
    ILTServer *g_pLTServer;
    void *CCommandMgr_QueueCommand;
    HCATEGORY m_hCatWeapons;
    HCATEGORY m_hCatAmmo;
    IDatabaseMgr *g_pLTDatabase;
    uint8_t m_randomWepTable[20];
    uint8_t m_currentRandomWeaponInd=17;
    uint32_t m_randomWeaponTime = 0;
    HWEAPON m_currentRandomWeapon;
    HRECORD m_hSlowMoHandles[20];
    uint8_t m_randomSlowmoTable[20];
    uint8_t m_currentRandomSlowMoInd=13;
    HWEAPON m_hUnarmedRecord;
    uint32_t ObjectCreateStruct_m_Name;
    void *g_pGetNetClientData;
    uint8_t isXP2 = 0;
    unsigned char m_uDT_CRUSH = 0;
    bool m_bfreeMovement = 0;
    void *CPlayerObj_UpdateMovement = 0;
    bool IsMasterServerRequestWorking() {
        return m_isMasterServerRequestWorking;
    }
    void regcall setMasterServerRequestWorking(bool state) {
        m_isMasterServerRequestWorking = state;
    }
    AppHandler *appHandler() { return m_appHandler; }
    void regcall setAppHandler(AppHandler *handler) { m_appHandler = handler; }
    static bool regcall isValidIpAddressPort(char *ipAddress);
    bool regcall checkPlayerStatus(GameClientData *pGameClientData);
    GameClientData *regcall getGameClientData(void *client);
    void regcall BootWithReason(GameClientData *pGameClientData,
                                unsigned char code, char *reason);
    void *regcall HandleToObject(void *hObj);
    void *regcall GetClientObject(void *hClient);

    CPlayerObj *regcall GetPlayerFromHClient(void *hClient);
    void *regcall GetWeaponData(void *hWep);
    void *regcall GetAmmoData(void *hAmmo);
    char *regcall getCurrentLevelName();
    uint32_t regcall getRealTimeMS();
    unsigned regcall getClientAddr(void *hClient, uint8_t pAddr[],
                                   uint16_t *pPort);
    IDatabaseMgr *regcall getDatabaseMgr();
    void regcall getLTServerClient(unsigned char *baseMod, uintptr_t szMod);

    void *regcall getModelStruct(char *model);
    void regcall handlePlayerPositionMessage(CAutoMessageBase_Read *pMsg);
    static void updateMovement();
    void regcall updateMovement(CPlayerObj *pPlayerObj);
    void regcall handlePlayerPositionMessage(CPlayerObj *pPlayerObj,
                                             CAutoMessageBase_Read *pMsg);
    void regcall initServer();
    void regcall initClient();
    void regcall Update();
    void regcall setRespawn(HOBJECT hObjResp);
    void regcall setExeType(bool state);
    static void cdeclcall requestMasterServerCallback(const char *pBuffer,
                                                      const unsigned nBufferLen,
                                                      void *pCallbackParam);
    static void stdcall requestMasterServer(bool, unsigned short, const char *);
    void regcall addInternetServer(char *str);
    static void regcall SdkHandlerUpdate();
    static void *regcall hookLoadMaps(uintptr_t index, uintptr_t *all);
    static void regcall hookGetScreenMulti(SpliceHandler::reg *p);
    static void regcall hookCheckpointEvent(SpliceHandler::reg *p);
    static void regcall hookPatchHoleKillHives(SpliceHandler::reg *p);
    static void regcall hookOnMapLoaded(SpliceHandler::reg *p);
    static void regcall hookUDPRecvfrom(SpliceHandler::reg *p);
    static void regcall hookUDPConnReq(SpliceHandler::reg *p);
    static void regcall hookCheckUDPDisconnect(SpliceHandler::reg *p);
    static void regcall hookRespawnStartPoint(SpliceHandler::reg *p);
    static void regcall hookOnConnectServer(SpliceHandler::reg *p);
    static void regcall hookUseSkin1(SpliceHandler::reg *p);
    static void regcall hookLoadMaps1(SpliceHandler::reg *p);
    static void regcall hookLoadMaps2(SpliceHandler::reg *p);
    static void regcall hookCGrenadeProximity_HandleActivateMsg(SpliceHandler::reg *p);
    static char *regcall getCurrentLevelNameStatic();
    SdkHandler();
    ~SdkHandler();
    playerData pPlayerData[64];
};
#endif
