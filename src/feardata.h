
class appData;
class handleData;

class fearData {
 public:
  appData *aData;
  ILTCommon *g_pCommonLT;
  IDatabaseMgr *g_pLTDatabase;
  ILTClient *g_pLTClient;
  ILTServer *g_pLTServer;
  IGameSpyBrowser *g_pGameSpyBrowser;
  CInterfaceMgr *g_pInterfaceMgr;
  ClientConnectionMgr *g_pClientConnectionMgr;
  CServerMissionMgr **g_pServerMissionMgrAdr;
  CGameDatabaseMgr *g_pGameDatabaseMgr;
  HCATEGORY m_hMissionCat;
  HCATEGORY m_hModelsCat;
  unsigned char *g_pScreenMultiAdr;
  unsigned char *g_pScreenMulti = 0;
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
  char *getCurrentLevelName();
  HRECORD getModelStruct(char *model);
  IDatabaseMgr *getDatabaseMgr();
  void getLTServerClient(unsigned char *baseMod, uintptr_t szMod);
  void setExeType(bool state);
  void addInternetServer(char *str);
  static void stdcall requestMasterServer(bool, unsigned short, char const *);
  static void regcall fearDataUpdate();
  static void regcall hookOnConnectServer(reg *p);
  static void regcall hookGetScreenMulti(reg *p);
  static void regcall hookCheckpointEvent(reg *p);
  static void regcall hookRespawnStartPoint(reg *p);
  static void regcall hookOnMapLoaded(reg *p);
  static void regcall hookUseSkin1(reg *p);
  static void regcall hookLoadMaps1(reg *p);
  static void regcall hookLoadMaps2(reg *p);
  ~fearData();
};
