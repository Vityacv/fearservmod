#ifdef __cplusplus
extern "C" {
#endif
void hookOnConnectServerRet();

extern char **wepsmp;
extern char **specmp;
void __fastcall hookWindowProc(reg *p);
// void __fastcall hookWindowProc(/*reg * p*/HWND hwnd, UINT uMsg, WPARAM
// wParam, LPARAM lParam);
class handleData;
class appData {
 public:
  ~appData();
  handleData *hData;
  fearData *pSdk;
  unsigned char *gDatabase;
  uintptr_t gDatabaseSz;
  unsigned char *gServer;
  uintptr_t gServerSz;
  unsigned char *gServerExe;
  uintptr_t gServerExeSz;
  unsigned char *gEServer;
  uintptr_t gEServerSz;
  unsigned char *gClient;
  uintptr_t gClientSz;
  unsigned char *gFearExe;
  uintptr_t gFearExeSz;
  unsigned storyModeCnt;
  void *hInstance;
  bool bRandWep=0;
  bool bFlashlight = 0;
  bool bExeType = 0;
  bool bCoopDoSpawn = 0;
  bool bDoGameSpy = 0;
  bool bConfigInitialized = 0;
  TCHAR * iniBuffer=0;
  char * strNs1;
  char * strNs2;
  char * strMasterAvail;
  char * strMaster;
  char * strMotd;
  unsigned char bPreventNoclip = 0;
  bool bIgnoreUnusedMsgID = 0;
  bool bSyncObjects = 0;
  unsigned char bCoop = 0;
  bool bBotsMP = 0;
  char flagPatchHoleKillHives=-1;
  char **aSkinStr;
  unsigned char *aGameClientStruct;
  unsigned char *aFlashlight[1];
  unsigned char *aPatchHoleKillHives;
  unsigned char *aFreeMovement;
  //unsigned char *doConnectIpAdrTramp;
  unsigned char *aIsMultiplayerGameClient;
  unsigned char *aSPModeSpawn;
  unsigned char *aRunGameModeXP;
  unsigned char *aServTeleport;
  unsigned char *aStoryModeStruct;
  unsigned char skinState;
  unsigned char bSettingsLoaded=0;
  unsigned timerGamespyServ;
  void init();
  void initClient();
  void configParse(char *pathCfg);
  void configHandle();
  void regcall setFlashlight(bool state);
  void regcall setCoopDoSpawn(bool state);
  void regcall setPatchHoleKillHives(bool state);
  void regcall setMpGame(bool state);
};
extern appData hDataGlobal;

#ifdef UNICODE
#define __tgetmainargs __wgetmainargs
int __cdecl __wgetmainargs(int *_Argc, WCHAR ***_Argv, WCHAR ***_Env,
                           int _DoWildCard, STARTUPINFO *_StartInfo);
#else
#define __tgetmainargs __getmainargs
int __cdecl __getmainargs(int *_Argc, char ***_Argv, char ***_Env,
                          int _DoWildCard, STARTUPINFO *_StartInfo);
#endif
//extern uintptr_t GetIStringEditMgr();
#ifdef __cplusplus
}
#endif
