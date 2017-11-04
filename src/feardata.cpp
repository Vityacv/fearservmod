#include "pch.h"
#include "intrin.h"
#include "memory.h"
#include "splice.h"
#include "splicealloc.h"
#include "stl.h"
#include "fearsdk.h"
#include "feardata.h"
#include "app.h"
#include "handle.h"
#include "main.h"

char *fearData::getCurrentLevelName() {
  if (g_pServerMissionMgrAdr && *g_pServerMissionMgrAdr) {
    CServerMissionMgr *g_pServerMissionMgr = *g_pServerMissionMgrAdr;
    HRECORD hMission = g_pLTDatabase->GetRecordByIndex(
        m_hMissionCat, g_pServerMissionMgr->GetCurrentMission());  //+0x98 \\38
    if (!hMission) return nullptr;
    return *(char **)hMission;
  }
  return nullptr;
}

IDatabaseMgr *fearData::getDatabaseMgr() {
  return ((IDatabaseMgr * (__stdcall *)())
              GetProcAddress((HMODULE)aData->gDatabase, "GetIDatabaseMgr"))();
}

void fearData::getLTServerClient(unsigned char *baseMod, uintptr_t szMod) {
  {
    unsigned char *tmp = baseMod;
    for (unsigned i = 0; i != 2; i++) {
      tmp=scanBytes(
                         (unsigned char *)tmp, (szMod+baseMod)-tmp,
                         (char *)"6A6968????????68????????E8????????68????????E8????????83C410");
      if (!tmp) break;
      if (!memcmp((char *)*(uintptr_t *)((unsigned char *)tmp + 3), "ILTClient",
                  9))
        g_pLTClient = (ILTClient *)*(uintptr_t *)((unsigned char *)tmp + 8);
      else if (!memcmp((char *)*(uintptr_t *)((unsigned char *)tmp + 3),
                       "ILTServer", 9))
        g_pLTServer = (ILTServer *)*(uintptr_t *)((unsigned char *)tmp + 8);
      tmp++;
    }
  }
  if (!g_pLTServer) {
    unsigned char *tmp = 0;
    tmp = scanBytes((unsigned char *)baseMod, szMod,
                    (char *)"6A69E9????????68????????E8");
    g_pLTServer = (ILTServer *)*(uintptr_t *)((unsigned char *)tmp + 8);
  }
}

unsigned char mapsIndex[] = {0,  1,  2,  3,  4,  5,  6,  7,          8,  9,
                             10, 11, 12, 13, 14, 15, 16, 17,         18, 19,
                             20, 21, 57, 50, 51, 62, 63, 64,         59, 60,
                             61, 58, 54, 55, 56, 31, 32, 33,         34, 35,
                             36, 37, 38, 39, 40, 41, 42, /*43,*/ 44, 45, 46,
                             47, 23, 24, 25, 22};  //,26,27,28,29,30,52,53};

HRECORD regcall hookLoadMaps(uintptr_t index, uintptr_t *all) {
  fearData *pSdk = &handleData::instance()->pSdk;
  if (pSdk->g_pServerMissionMgrAdr && *pSdk->g_pServerMissionMgrAdr) {
    CServerMissionMgr *g_pServerMissionMgr = *pSdk->g_pServerMissionMgrAdr;
    *all = pSdk->g_pLTDatabase->GetNumRecords(pSdk->m_hMissionCat);
    uintptr_t i = index;
    if (*all == 65) {  // FEARXP2 GOG
      i = mapsIndex[index];
      *all = sizeof(mapsIndex);
    }
    return pSdk->g_pLTDatabase->GetRecordByIndex(pSdk->m_hMissionCat, i);
  }
  return nullptr;
}

HRECORD fearData::getModelStruct(char *model) {
  return g_pLTDatabase->GetRecord(m_hModelsCat, model);
}

void fearData::fearDataInitServ() {
  unsigned char *gServer = aData->gServer;
  unsigned char *gEServer = aData->gEServer;
  uintptr_t gEServerSz = aData->gEServerSz;
  uintptr_t gServerSz = aData->gServerSz;

  aData->gDatabase = (unsigned char *)GetModuleHandle(_T(GDB_DLL_NAME));
  aData->gDatabaseSz = GetModuleSize((HMODULE)aData->gDatabase);
  g_pLTDatabase = getDatabaseMgr();
  getLTServerClient(gEServer, gEServerSz);
  {
    unsigned char *tmp = 0;
    tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"8B0D??????????6A00E8????????84C075??85??74??8B??8B");
    if (tmp) {
      g_pServerMissionMgrAdr = (CServerMissionMgr **)*(uintptr_t *)(tmp + 2);
    }
  }
  g_pGameDatabaseMgr = (CGameDatabaseMgr *)*(uintptr_t *)*(
      uintptr_t *)(scanBytes((unsigned char *)gServer, gServerSz,
                             (char *)"8B0D??????????E8????????85C074??80380074?"
                                     "???8D????85") +
                   2);
  this->m_hMissionCat = g_pLTDatabase->GetCategory(
      *(HDATABASE *)((unsigned char *)g_pGameDatabaseMgr + 0x20),
      "Missions/Missions");

  m_hModelsCat = g_pLTDatabase->GetCategory(
      *(HDATABASE *)((unsigned char *)g_pGameDatabaseMgr + 0x20),
      "Character/Models");
}

void fearData::fearDataInit() {
  unsigned char *gClient = aData->gClient;
  unsigned char *gFearExe = aData->gFearExe;
  uintptr_t gFearExeSz = aData->gFearExeSz;
  uintptr_t gClientSz = aData->gClientSz;
  aData->gDatabase = (unsigned char *)GetModuleHandle(_T(GDB_DLL_NAME));
  aData->gDatabaseSz = GetModuleSize((HMODULE)aData->gDatabase);
  g_pLTDatabase = getDatabaseMgr();
  getLTServerClient(gFearExe, gFearExeSz);
  {
    void *tmp = scanBytes((unsigned char *)gClient, gClientSz,
                          (char *)"8B0D????????83C40481C1????????56894C242CE8");
    g_pInterfaceMgr =
        (CInterfaceMgr *)*(uintptr_t *)*(uintptr_t *)((unsigned char *)tmp + 2);
    while (true)
      if (!g_pInterfaceMgr->GetGameState())
        Sleep(50);
      else
        break;
  }
  {
    void *tmp = scanBytes(
        (unsigned char *)gClient, gClientSz,
        (char *)"8B0D????????5053E8????????8AD88B44240CF6DB1ADB8D4C2410");
    g_pClientConnectionMgr =
        (ClientConnectionMgr *)*(uintptr_t *)((unsigned char *)tmp + 2);
  }
  {
    unsigned char *tmp = (unsigned char *)(uintptr_t *)(scanBytes(
        (unsigned char *)gClient, gClientSz,
        (char *)"892D????????E8????????A1????????508D8D????????E8????????"
                "33DBC685????????01"));
    if (tmp) g_pScreenMultiAdr = tmp + 2;
  }
  spliceUp(scanBytes((unsigned char *)gClient, gClientSz,
                     (char *)"8B??68????????E8????????8B????8B??????????8B??33?"
                             "?????68??????????FF"),
           (void *)hookGetScreenMulti);
  if (g_pScreenMultiAdr) {
    g_pScreenMulti_RequestServerDetails =
        scanBytes((unsigned char *)gClient, gClientSz,
                  (char *)"8B44240483EC1885C0744E8038007449");
  }

  /*spliceUp(scanBytes((unsigned char *)gFearExe, gFearExeSz,
                    (char *)"6A0AFF520883F8010F84"),fearDataUpdate);*/
  /*spliceUp(scanBytes((unsigned char *)gClient, gClientSz,
                    (char
     *)"8B??????81??03020000??8B??????0F"),fearDataUpdate);*/
  spliceUp(scanBytes((unsigned char *)gFearExe, gFearExeSz,
                     (char *)"A1????????81EC????????85C0558BAC"),
           (void *)fearDataUpdate);
  if (aData->doConnectIpAdrTramp) {
    {
      void *tmp = scanBytes((unsigned char *)gFearExe, gFearExeSz,
                            (char *)"6A00??6A00E8????????8B??85??7417");
      tmp = (unsigned char *)tmp + ((sizeof("6A00??6A00E8") - 1) / 2);
      tmp = (unsigned char *)tmp + (*(uintptr_t *)tmp) + 4;
      unknownFunc2 = tmp;
    }
    getStructFromString1 = scanBytes((unsigned char *)gFearExe, gFearExeSz,
                                     (char *)"5355568BE98B45145733FF85C07638");

    {
      void *tmp =
          scanBytes((unsigned char *)gFearExe, gFearExeSz,
                    (char *)"8B0D????????50E8????????85C08B4C24108901740C");
      unknownStruct1 = (void *)*(uintptr_t *)((unsigned char *)tmp + 2);
    }

    spliceUp(scanBytes((unsigned char *)gFearExe, gFearExeSz,
                       (char *)"83E8028BCE74488B166A01FF12"),
             (void *)hookOnConnectServer);
  }
  g_pGameDatabaseMgr = (CGameDatabaseMgr *)*(uintptr_t *)*(
      uintptr_t *)(scanBytes((unsigned char *)gClient, gClientSz,
                             (char *)"8B0D????????8B41248B1650A1????????"
                                     "508BCEFF526885C08BCF751350E8") +
                   2);
  m_hModelsCat = g_pLTDatabase->GetCategory(
      *(HDATABASE *)((unsigned char *)g_pGameDatabaseMgr + 0x20),
      "Character/Models");
}

void fearData::Update() {
  if (*(uintptr_t *)aData->aGameClientStruct &&
      *(unsigned *)((
          unsigned char *)((*(uintptr_t *)(aData->aGameClientStruct)) +
                           0x224))) {
    g_pGameSpyBrowser = (IGameSpyBrowser *)*(
        uintptr_t *)((unsigned char *)(*(uintptr_t *)g_pClientConnectionMgr) +
                     0x22FC);
    if (g_pGameSpyBrowser && (g_pScreenMulti || g_pScreenMultiAdr)) {
      unsigned char *g_pScreenMulti;
      if (this->g_pScreenMulti)
        g_pScreenMulti = this->g_pScreenMulti;
      else
        g_pScreenMulti =
            (unsigned char *)*(uintptr_t *)*(uintptr_t *)g_pScreenMultiAdr;
      if (!g_pGameSpyBrowser_RequestServerListAdr) {
        int tmp;
        g_pGameSpyBrowser_RequestServerListAdr =
            ((unsigned char *)(*(uintptr_t *)(g_pGameSpyBrowser)) + 0x8);
        unprotectMem((unsigned char *)g_pGameSpyBrowser_RequestServerListAdr);
        g_pGameSpyBrowser_RequestServerList =
            (void *)*(uintptr_t *)g_pGameSpyBrowser_RequestServerListAdr;
      }

      if (g_pScreenMulti && !*((unsigned char *)g_pScreenMulti +
                               0x2F4))  // FEARXP 0x238 INTERNET - 1; LAN - 0
        *(uintptr_t *)g_pGameSpyBrowser_RequestServerListAdr =
            (uintptr_t)requestMasterServer;
      else
        *(uintptr_t *)g_pGameSpyBrowser_RequestServerListAdr =
            (uintptr_t)g_pGameSpyBrowser_RequestServerList;
    }
    if (aData->bDoGameSpy) {
      unsigned tmp = GetTickCount();
      if (aData->timerGamespyServ == 0) {
        aData->timerGamespyServ = tmp;
      } else {
        tmp = tmp - aData->timerGamespyServ;
        if (tmp > 5000) {
          g_pGameSpyBrowser = (IGameSpyBrowser *)*(
              uintptr_t *)((unsigned char *)(*(
                               uintptr_t *)g_pClientConnectionMgr) +
                           0x22FC);
          if (g_pGameSpyBrowser)
            *((unsigned char *)g_pGameSpyBrowser + 0x84) = 1;
          aData->timerGamespyServ = 0;
          aData->bDoGameSpy = 0;
        }
      }
    }
  }
}

char IsMasterServerRequestWorking;

void requestMasterServerCallback(const char *pBuffer, const unsigned nBufferLen,
                                 void *pCallbackParam) {
  appData *aData = &handleData::instance()->aData;
  fearData *pSdk = aData->pSdk;
  if (!nBufferLen) {
    IsMasterServerRequestWorking = 0;
    return;
  }
  char *pStr = (char *)pBuffer, *pN = (char *)pBuffer,
       *pEnd = (char *)(pBuffer + nBufferLen);
  unsigned srvCnt = 0;
  while (true) {
    if (!pN) break;
    pN = strstr(pN, "\n");
    if (pN >= pEnd) break;
    if (pN) {
      *pN = 0;
      pN++;
    }
    if (!isValidIpAddressPort(pStr)) break;
    pSdk->addInternetServer((char *)pStr);
    pStr = pN;
    srvCnt++;
  }
  aData->bDoGameSpy = 1;
  *((unsigned char *)pSdk->g_pGameSpyBrowser + 0x84) = 1;
  IsMasterServerRequestWorking = 0;
};

void stdcall fearData::requestMasterServer(bool, unsigned short, char const *) {
  fearData *pSdk = &handleData::instance()->pSdk;
  if (IsMasterServerRequestWorking) return;
  IsMasterServerRequestWorking = 1;
  *((unsigned char *)pSdk->g_pGameSpyBrowser + 0x84) = 0;
  pSdk->g_pGameSpyBrowser->RequestURLData(
      "http://master.fear-combat.org/api/serverlist-ingame.php",
      (void *)requestMasterServerCallback, nullptr);
}

void fearData::addInternetServer(char *str) {
  unsigned char *g_pScreenMulti =
      (unsigned char *)*(uintptr_t *)*(uintptr_t *)g_pScreenMultiAdr;
  ((void(__thiscall *)(void *, char *, unsigned,
                       unsigned))g_pScreenMulti_RequestServerDetails)(
      g_pScreenMulti, str, 1, 1);
}

void regcall fearData::fearDataUpdate() {
  fearData *pSdk = &handleData::instance()->pSdk;
  pSdk->Update();
}

void regcall fearData::hookGetScreenMulti(reg *p) {
  fearData *pSdk = &handleData::instance()->pSdk;

  pSdk->g_pScreenMulti = (unsigned char *)p->tcx;
}

void regcall fearData::hookCheckpointEvent(reg *p) {
  fearData *pSdk = &handleData::instance()->pSdk;
  pSdk->g_pLTServer->GetObjectPos((HOBJECT)p->v0, &pSdk->checkPointPos);
  pSdk->checkPointState = 2;
}
void regcall fearData::hookOnMapLoaded(reg *p) {
  appData *aData = &handleData::instance()->aData;
  fearData *pSdk = aData->pSdk;
  char *lvlName = pSdk->getCurrentLevelName();
  unsigned skinState = 9;
  aData->setPatchHoleKillHives(0);
  if (lvlName) {
    switch (hash_rta(lvlName)) {
      case hash_ct("07_ATC_Roof"):
        skinState = 0;
        break;
      case hash_ct("02_Docks"):
        skinState = 0;
        break;
      case hash_ct("XP2_W06"):
        skinState = 0;
        break;
      case hash_ct("13_Hives"):
        aData->setPatchHoleKillHives(1);
        break;
      default:
        aData->setCoopDoSpawn(1);
        break;
    }
    pSdk->checkPointState = 0;
    aData->skinState = skinState;
    *aData->aSkinStr = (char *)"Player";
    aData->storyModeCnt = 0;
    if (aData->bPreventNoclip)
      *(unsigned short *)(aData->aFreeMovement) = 0x06EB;
  }
}

void regcall fearData::hookRespawnStartPoint(reg *p) {
  fearData *pSdk = &handleData::instance()->pSdk;
  HOBJECT startPtObj = (HOBJECT) * (uintptr_t *)((unsigned char *)p->tax + 8);
  LTVector startPt;
  pSdk->g_pLTServer->GetObjectPos(startPtObj, &startPt);
  if (pSdk->checkPointState != 2) {
    if (memcmp(&startPt, &pSdk->checkPointPos, sizeof(LTVector))) {
      pSdk->checkPointState = 0;
    }
  }
  LTVector newPos = pSdk->checkPointPos;
  char *lvlName = pSdk->getCurrentLevelName();
  if (lvlName) {
    switch (hash_rta(lvlName)) {
      case hash_ct("07_ATC_Roof"):
        newPos = LTVector{1566.47f, 2085.51f, 3938.72f};
        pSdk->checkPointState = 1;
        break;
      case hash_ct("11_Mapes_Elevator"):
        newPos = LTVector{3085.24f, -400.0f, -3066.04f};
        pSdk->checkPointState = 1;
        break;
      case hash_ct("02_Docks"):
        newPos = LTVector{-11716.2f, -2833.13f, -10565.7f};
        pSdk->checkPointState = 1;
        break;
      case hash_ct("18_Facility_Upper"):
        newPos = LTVector{-1340.0f, -1040.0f, -100.0f};
        pSdk->checkPointState = 1;
        break;
      case hash_ct("09_Bishop_Rescue"):
        newPos = LTVector{18721.3f, 2741.3f, 389.176f};
        pSdk->checkPointState = 1;
        break;
      case hash_ct("12_Badge"):
        newPos = LTVector{-8808.6f, -30.0f, -3414.9f};
        pSdk->checkPointState = 1;
        break;
      case hash_ct("17_Factory"):
        newPos = LTVector{-117.221f, -929.996f, -16681.2f};
        pSdk->checkPointState = 1;
        break;
      case hash_ct("08_Admin"):
        newPos = LTVector{-14026.0f, 2426.0f, 5882.0f};
        pSdk->checkPointState = 1;
        break;
      case hash_ct("XP_CHU01"):
        newPos = LTVector{-704.0f, -1500.0f, -3072.0f};
        pSdk->checkPointState = 1;
        break;
      case hash_ct("XP2_W01"):
        newPos = LTVector{15180.0f, 25179.0f, 47028.0f};
        pSdk->checkPointState = 1;
        break;
      case hash_ct("XP_HOS01"):
        newPos = LTVector{4036.75f, 80.9f, 7598.07f};
        pSdk->checkPointState = 1;
        break;
      case hash_ct("XP_HOS02"):
        newPos = LTVector{512.0f, 21424.1f, -3840.0f};
        pSdk->checkPointState = 1;
        break;
      case hash_ct("XP2_W06"):
        newPos = LTVector{-24186.0f, -6460.0f, -65997.0f};
        pSdk->checkPointState = 1;
        break;
      case hash_ct("XP2_W14"):
        newPos = LTVector{-8651.0f, 990.0f, -4160.0f};
        pSdk->checkPointState = 1;
        break;
      case hash_ct("XP2_W16"):
        newPos = LTVector{-8972.0f, 2070.0f, -3833.0f};
        pSdk->checkPointState = 1;
        break;
      case hash_ct("XP2_W17"):
        newPos = LTVector{-21728.0f, 1800.0f, 7588.0f};
        pSdk->checkPointState = 1;
        break;
    }
  }
  if (pSdk->checkPointState)
    pSdk->g_pLTServer->SetObjectPos(startPtObj, newPos);
  if (pSdk->checkPointState == 2) pSdk->checkPointState = 1;
}

void fearData::setExeType(bool state) {
  // int tmp;
  /*VirtualProtect(
      (void *)(uintptr_t *)((unsigned char *)(*(uintptr_t *)g_pLTClient) +
                            0x1E0),
      4, PAGE_READWRITE, (PDWORD)&tmp);*/
  unprotectMem((unsigned char *)(uintptr_t *)((unsigned char *)(*(
                                                  uintptr_t *)g_pLTClient) +
                                              0x1E0),
               sizeof(void *));
  if (state)
    *(uintptr_t *)((unsigned char *)(*(uintptr_t *)g_pLTClient) + 0x1E0) =
        (uintptr_t)aData->aIsMultiplayerGameClient;
  else
    *(uintptr_t *)((unsigned char *)(*(uintptr_t *)g_pLTClient) + 0x1E0) =
        (uintptr_t)aData->aIsMultiplayerGameClient;
  if (g_pLTServer) {
    /*VirtualProtect(
        (void *)(uintptr_t *)((unsigned char *)(*(uintptr_t *)g_pLTServer) +
                              0x1EC),
        4, PAGE_READWRITE, (PDWORD)&tmp);*/
    unprotectMem((unsigned char *)(uintptr_t *)((unsigned char *)(*(
                                                    uintptr_t *)g_pLTServer) +
                                                0x1EC),
                 sizeof(void *));
    if (state)
      *(uintptr_t *)((unsigned char *)(*(uintptr_t *)g_pLTServer) + 0x1EC) =
          (uintptr_t)aData->aIsMultiplayerGameClient;
    else
      *(uintptr_t *)((unsigned char *)(*(uintptr_t *)g_pLTServer) + 0x1EC) =
          (uintptr_t)aData->aIsMultiplayerGameClient;
  }
  if (state) {
    aData->setMpGame(1);
  } else
    aData->setMpGame(0);
  aData->bExeType = state;
};

void regcall fearData::hookOnConnectServer(reg *p) {  // only FEAR.exe 1.08
  fearData *pSdk = &handleData::instance()->pSdk;

  unsigned char *structEsi = (unsigned char *)p->tsi,
                *structEdi = (unsigned char *)p->tbp,
                *structEbp = (unsigned char *)p->tdi,
                state = (unsigned char)p->tax;
  if (state == 1) {
    unsigned char *struct2 = ((unsigned char *(__thiscall *)(void *, char *))(
                                  void *)pSdk->getStructFromString1)(
        ((unsigned char *)(*(uintptr_t *)pSdk->unknownStruct1) + 0x198),
        (char *)"internet");
    if (struct2) {
      bool flag = *(uintptr_t *)((unsigned char *)structEdi + 0x68) == -1;
      unsigned char *struct3 = (unsigned char *)*(uintptr_t *)struct2;
      if (!((bool(__thiscall *)(void *, bool, unsigned char *)) *
            (uintptr_t *)((unsigned char *)struct3 + 0x54))(struct2, flag,
                                                            structEdi + 0x6C)) {
        *(uintptr_t *)((unsigned char *)structEsi + 0x1C) = (uintptr_t)struct2;
        p->origFunc = (uintptr_t)hookOnConnectServerRet;
      }
    }
  } else if (!state) {
    unsigned char *struct4 =
        ((unsigned char *)(*(uintptr_t *)((unsigned char *)structEbp + 0x1D8)));
    if (struct4) {
      unsigned char *struct5 = (unsigned char *)*(uintptr_t *)struct4;
      if (!((bool(__thiscall *)(void *, unsigned char *))(void *)*(
              uintptr_t *)((unsigned char *)struct5 + 0x30))(
              struct4, (unsigned char *)((unsigned char *)structEdi + 0xD4))) {
        ((bool(__thiscall *)(void *, unsigned char *, unsigned char *,
                             unsigned))(void *)pSdk->unknownFunc2)(
            structEsi, struct4, structEdi, 1);
        if (!*(unsigned char *)structEdi + 4)
          p->origFunc = (uintptr_t)hookOnConnectServerRet;
      }
    }
  }
}

void regcall fearData::hookUseSkin1(reg *p) {
  appData *aData = &handleData::instance()->aData;
  fearData *pSdk = aData->pSdk;
  if (!aData->skinState) {
    p->tax = (uintptr_t)pSdk->getModelStruct((char *)"Player");
    *aData->aSkinStr = (char *)"Player";
    aData->skinState++;
  } else {
    p->tax = (uintptr_t)pSdk->getModelStruct((char *)"DeltaForce_multi");
    *aData->aSkinStr = (char *)"DeltaForce_multi";
  }
}

void regcall fearData::hookLoadMaps1(reg *p) {
  p->tax = (uintptr_t)hookLoadMaps(p->tax, &p->tbx);
  p->state = 2;
}
void regcall fearData::hookLoadMaps2(reg *p) {
  p->tax = (uintptr_t)hookLoadMaps(p->tdi, &p->v5);
  p->state = 2;
}

char *getCurrentLevelName() {
  fearData *pSdk = &handleData::instance()->pSdk;
  return pSdk->getCurrentLevelName();
}

fearData::~fearData() {}
