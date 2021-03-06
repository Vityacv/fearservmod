
class appData;
class handleData;

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
};

#include "node.h"
struct IPData {
  uint32_t ip; // TODO: ip6 support?
  uint16_t port;
  uint32_t timestamp;
};

struct IPChunk {
  IPChunk *next;
  static constexpr unsigned countMax = 16;
  IPData buf[countMax];
  uintptr_t count;
  static uintptr_t foreach (nodeData<IPChunk> &Chunk, IPData * Block,
                            uintptr_t(*func)(uintptr_t index, IPChunk *Chunk,
                                             IPData *Block));
  static IPData *add(nodeData<IPChunk> &Chunk, IPData *Block);
};

class fearData {
public:
  CRITICAL_SECTION g_ipchunkSection;
  nodeData<IPChunk> g_ipchunk;
  appData *aData;
  ILTCommon *g_pCommonLT;
  IDatabaseMgr *g_pLTDatabase;
  ILTClient *g_pLTClient;
  ILTServer *g_pLTServer;
  ILTPhysics *g_pPhysicsLT;
  ILTModel *g_pModelLT;
  IGameSpyBrowser *g_pGameSpyBrowser;
  CWeaponDB *g_pWeaponDB;
  CServerDB *g_pServerDB;
  CInterfaceMgr *g_pInterfaceMgr;
  ClientConnectionMgr *g_pClientConnectionMgr;
  ServerConnectionMgr *g_pServerConnectionMgr;
  CServerMissionMgr **g_pServerMissionMgrAdr;
  CGameDatabaseMgr *g_pGameDatabaseMgr;
  ServerVoteMgr *g_pServerVoteMgr;
  CArsenal *g_pArsenal;
  void *g_pGameModeMgr_instance;
  void *g_pServerVoteMgr_instance;
  void *g_pServerVoteMgr_ClearVote;
  void *g_pServerConnectionMgr_instance;
  void *g_pGetGameClientData;
  void *g_pBootWithReason;
  void *g_pGetNetClientData;
  void *g_pGameServerShell;
  void *CArsenal_GetAmmoCount;
  void *CArsenal_SetAmmo;
  void *CArsenal_GetCurWeapon;
  void *CArsenal_DecrementAmmo;
  void *CCommandMgr_QueueCommand;
  void *pCmdMgr;
  HOBJECT startPtObj = 0;
  HCATEGORY m_hMissionCat;
  HCATEGORY m_hModelsCat;
  HCATEGORY m_hCatGlobal;
  HCATEGORY m_hCatWeapons;
  HCATEGORY m_hCatGear;
  HCATEGORY m_hCatAmmo;
  HRECORD m_hRecGlobal;
  HRECORD m_hGlobalRecord;
  HRECORD m_hFastForward;
  HRECORD m_hPlayer;
  HWEAPON m_hUnarmedRecord;
  unsigned char randomWepTable[20];
  unsigned char currentRandomWeaponInd;
  HAMMO m_hUnarmedRecordAmmo;
  unsigned randomWeaponTime = 0;

  HWEAPON currentRandomWeapon;
  unsigned ILTServer_GetRealTimeMS;
  unsigned ILTServer_GetClientAddr;
  unsigned GameClientData_m_nClientMoveCode;
  unsigned ObjectCreateStruct_m_Name;
  unsigned CPlayerObj_m_Inventory;
  unsigned CPlayerObj_m_Arsenal;
  unsigned CPlayerObj_m_hClient;
  unsigned CPlayerObj_m_eStandingOnSurface;
  unsigned CPlayerObj_m_vLastVelocity;
  unsigned CPlayerObj_m_PlayerRigidBody;
  unsigned CPlayerObj_m_bUseLeash;
  unsigned CPlayerObj_m_vLastClientPos;
  unsigned CPlayerObj_m_fMoveMultiplier;
  unsigned CPlayerObj_m_fJumpMultiplier;
  unsigned CPlayerObj_DropCurrentWeapon;
  unsigned CPlayerObj_m_ePlayerState;
  unsigned CPlayerObj_m_fLeashLen;
  unsigned CPlayerObj_m_fLeashSpringRate;
  unsigned CPlayerObj_m_fLeashSpring;
  unsigned CPlayerObj_m_fLeashScale;
  unsigned CPlayerObj_m_nLastPositionForcedTime;
  unsigned CArsenal_m_hCurWeapon;
  unsigned CArsenal_m_pPlayer;
  unsigned GameModeMgr_ServerSettings;
  unsigned CCharacter_m_bOnGround;
  unsigned CCharacter_m_hLadderObject;
  unsigned ClientConnectionMgr_IGameSpyBrowser;
  unsigned char CArsenal_m_pAmmo;
  unsigned char uDT_CRUSH = 0;
  unsigned char ukPEDropWeapon;
  unsigned char ukPENextSpawnPoint;
  unsigned char ukPEPrevSpawnPoint;
  unsigned char *g_pScreenMultiAdr;
  unsigned char *g_pScreenMulti = 0;
  bool freeMovement = 0;
  unsigned char isXP2 = 0;
  void *CPlayerObj_UpdateMovement = 0;
  void *CPlayerObj_m_PlayerRigidBody_Update;
  void *CPlayerObj_TeleportClientToServerPos;
  void *CGrenadeProximity_IsEnemy;
  void *CPlayerObj_ChangeWeapon;
  void *g_pWeaponDB_GetWeaponData;
  void *g_pWeaponDB_GetAmmoData;
  void *g_pWeaponDB_GetInt32;
  void *g_pWeaponDB_GetBool;
  void *g_pLTServer_HandleToObject;
  void *g_pLTServer_GetClientObject;
  void *g_pGameServerShell_IsPaused;
  void *g_pGameSpyBrowser_RequestServerList = 0;
  void *g_pGameSpyBrowser_RequestServerListAdr = 0;
  void *g_pScreenMulti_RequestServerDetails = 0;
  LTVector checkPointPos;
  unsigned checkPointState;
  void *unknownStruct1;
  void *unknownFunc2;
  void *getStructFromString1;
  void fearDataInit();
  void fearDataInitServ();
  void Update();

  bool checkPlayerStatus(GameClientData *pGameClientData);
  GameClientData *getGameClientData(HCLIENT client);
  void BootWithReason(GameClientData *, unsigned char code, char *reason);
  char *getCurrentLevelName();
  uint32_t getRealTimeMS();
  LTRESULT getClientAddr(HCLIENT hClient, uint8_t pAddr[4], uint16_t *pPort);
  void *HandleToObject(HOBJECT hObj);
  CPlayerObj *GetPlayerFromHClient(HCLIENT hClient);
  HWEAPONDATA GetWeaponData(HWEAPON hWep);
  HAMMODATA GetAmmoData(HAMMO);
  HLOCALOBJ GetClientObject(HCLIENT hClient);
  HRECORD getModelStruct(char *model);
  IDatabaseMgr *getDatabaseMgr();
  void updateMovement(CPlayerObj *pPlayerObj);
  inline void handlePlayerPositionMessage(CPlayerObj *pPlayerObj,
                                          CAutoMessageBase_Read *pMsg);
  void getLTServerClient(unsigned char *baseMod, uintptr_t szMod);
  void setExeType(bool state);
  void addInternetServer(char *str);
  static void regcall hookUDPRecvfrom(reg *p);
  static void regcall hookUDPConnReq(reg *p);
  static void regcall hookCheckUDPDisconnect(reg *p);
  void regcall setRespawn(HOBJECT hObjResp);
  static void stdcall requestMasterServer(bool, unsigned short, char const *);
  static void regcall fearDataUpdate();
  static void regcall hookOnConnectServer(reg *p);
  static void regcall hookGetScreenMulti(reg *p);
  static void regcall hookCheckpointEvent(reg *p);
  static void regcall hookRespawnStartPoint(reg *p);
  static void regcall hookPatchHoleKillHives(reg *p);
  static void regcall hookOnMapLoaded(reg *p);
  static void regcall hookConnectUDPClient(reg *p);
  static void regcall hookUseSkin1(reg *p);
  static void regcall hookLoadMaps1(reg *p);
  static void regcall hookLoadMaps2(reg *p);
  static void regcall hookCGrenadeProximity_HandleActivateMsg(reg *p);
  ~fearData();
  playerData pPlayerData[64];

  float getFloatDB(HRECORD hRecord, const char *pszAtt,
                   uint32_t nValueIndex = 0, float fDef = 0.0f) {
    return g_pLTDatabase->GetFloat(g_pLTDatabase->GetAttribute(hRecord, pszAtt),
                                   nValueIndex, fDef);
  }
};
