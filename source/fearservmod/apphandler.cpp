#include "pch.h"
// #include "sdk.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <float.h>
#endif
#include "shared/common_macro.h"
#include "shared/memory_utils.h"
#include "shared/string_utils.h"
#include "shared/debug.h"
#include "splice/splice.h"
#include "shared/patch_handler.h"
#include "apphandler.h"
#include "sdkhandler.h"
#include "executionhandler.h"

extern "C" volatile uint8_t *g_doConnectIpAdrTramp = 0;

AppHandler::AppHandler() {

  m_eServerModule =
        reinterpret_cast<uint8_t *>(GetModuleHandle(_T("engineserver.dll")));
  // {
  //   auto exec_section =
  //       GetModuleFirstExecSection(reinterpret_cast<HMODULE>(m_eServerModule));
  //   m_eServer = reinterpret_cast<uint8_t *>(m_eServerModule +
  //                                           exec_section->VirtualAddress);
  //   m_eServerSz = exec_section->SizeOfRawData;
  // }
  {
    m_ExecModule = reinterpret_cast<uint8_t *>(GetModuleHandle(0));
    auto exec_section =
        GetModuleFirstExecSection(reinterpret_cast<HMODULE>(m_ExecModule));
    m_Exec = reinterpret_cast<uint8_t *>(m_ExecModule); //+ exec_section->VirtualAddress);
    m_ExecSz =  GetModuleSize((HMODULE)m_ExecModule);//exec_section->SizeOfRawData;
  }
}

void AppHandler::hookPlayWave(SpliceHandler::reg *p)
{
    char * str = reinterpret_cast<char*>(p->v0);
    if(str) {
        if(strcmp(str, "Snd\\Amb\\rooftop_wind_amb.wav") == 0) {
            auto &inst = *ExecutionHandler::instance()->appHandler();
            if(inst.m_playSound == 5) {
                inst.m_playSound = 0;
                p->origFunc = 0x0402F4E;
            } else ++inst.m_playSound;
        }
    }
}

void AppHandler::patchEndpoints(uint8_t *mod, uint32_t modSz)
{

    {
      static auto pat = BSF("68????????50FF15????????83C40C8D");
      unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
          (unsigned char *)mod, modSz, reinterpret_cast<uint8_t *>(&pat)));
      if (tmp) {
        m_patchHandler->addCode(tmp + 1, 12);
        *(const char **)(tmp + 1) = m_strMasterAvail;
        *(uint16_t *)(tmp + 6) = 0xE890;
        *(uintptr_t *)(tmp + 8) =
            getRel4FromVal((tmp + 8), (unsigned char *)strcpy);
      }
    }
    // NoAvailableGameSpy
    if (static auto pat = BSF("75??33D2895424??895424??89");
        uint8_t *adr =
            scanBytes(mod, modSz, reinterpret_cast<uint8_t *>(&pat))) {
      m_patchHandler->addCode(adr, 1);
      *static_cast<uint8_t *>(adr) = 0xEB;
    }

    // {
    //   static auto pat = BSF("6A1068????????8B??83????6A0083");
    //   unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
    //       (unsigned char *)mod, modSz, reinterpret_cast<uint8_t *>(&pat)));
    //   if (tmp) {
    //     m_patchHandler->addCode(tmp, 2);
    //     *(uint16_t *)(tmp) = 0x27EB;  // disable magic value
    //   }
    // }
    if(mod == m_eServer) { //%s.ms%d.gamespy.com disable
      static auto pat = BSF("760DB8060000005D81C4????????C3");
      unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
          (unsigned char *)mod, modSz, reinterpret_cast<uint8_t *>(&pat)));
      if (tmp) {
        m_patchHandler->addCode(tmp, 2);
        *(uint16_t *)(tmp) = 0x9090;
      }
      {
        static auto pat = BSF("8B??????508D??????68??????????FF??????????83C4??8D??????68");
        unsigned char *tmp = scanBytes(
            (unsigned char *)mod, modSz, reinterpret_cast<uint8_t *>(&pat));
        m_patchHandler->addCode(tmp + 1, 22);
        //*(tmp) = 0x68;
        // if (*strGame)
        //   *(uintptr_t *)(tmp + 1) = (uintptr_t)strGame;
        *(const char **)(tmp + 10) = m_strMaster;
        *(uint16_t *)(tmp + 15) = 0xE890;
        *(uintptr_t *)(tmp + 17) =
            getRel4FromVal((tmp + 17), (unsigned char *)strcpy);
      }
    } else {
        //Fix +join ip:port crash
        {
            static auto pat = BSF("78??A1????????????E8????????83");
              unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
                  (unsigned char *)mod, modSz, reinterpret_cast<uint8_t *>(&pat)));
              if(tmp) {
                m_patchHandler->addCode(tmp, 1);
                *reinterpret_cast<uint8_t *>(tmp) = 0xeb;
              }
        }
        {
          static auto pat = BSF("68????????50FF15????????83C41068");
          unsigned char *tmp = (unsigned char*)(unsigned char *)(unsigned *)(scanBytes(
              (unsigned char *)mod, modSz, reinterpret_cast<uint8_t *>(&pat)));
          if (tmp) {
            m_patchHandler->addCode(tmp + 1, 12);
            *(const char **)(tmp + 1) = m_strMaster;
            *(uint16_t *)(tmp + 6) = 0xE890;
            *(uintptr_t *)(tmp + 8) =
                getRel4FromVal((tmp + 8), (unsigned char *)strcpy);
          }
        }
    }
}

void AppHandler::setMotd(uint8_t *mod, uintptr_t modSz)
{
    {
      static auto pat = BSF("68????????68????????FF15????????56");
      unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
          (unsigned char *)mod, modSz, reinterpret_cast<uint8_t *>(&pat)));
      if (tmp) {
        tmp += 1;
        m_patchHandler->addCode(tmp, 4);
        *(const char **)tmp = m_strMotd;
      }
    }
//    if (!*m_strMotd) { // modt disable
//      unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
//          (unsigned char *)mod, modSz,
//          BYTES_SEARCH_FORMAT(
//              "833D????????01740333C0C3538B5C241085DB750433C0")));
//      if (tmp) {
//        m_patchHandler->addCode(tmp, 2);
//        *(uint16_t *)(tmp + 7) = 0x9090;
//      }
//    }
}

void AppHandler::hookSwitchToSP(SpliceHandler::reg *p) {
    auto &inst = *ExecutionHandler::instance()->sdkHandler();
    p->state = 2;
    inst.setExeType(false);
    p->tax = 0;
}

void AppHandler::hookSwitchToMP(SpliceHandler::reg *p) {
    auto &inst = *ExecutionHandler::instance()->sdkHandler();
    p->state = 2;
    inst.setExeType(true);
    p->tax = 1;
}

void AppHandler::hookStoryModeOn(SpliceHandler::reg *p) {
    auto &inst = *ExecutionHandler::instance()->sdkHandler();
    inst.m_bfreeMovement = 0;
}

void AppHandler::hookDoorTimer(SpliceHandler::reg *p) {
    unsigned char *func = getVal4FromRel(p->origFunc + 1);
    p->tax =
        ((uintptr_t(__thiscall *)(uintptr_t, uintptr_t))func)(p->tcx, p->retadr);
    float val = *(float *)(p->tax + 0x24);
    if (val > 0.0f)
        val += 3.0f;
    else
        val += 1.0f;
    *(float *)(p->tax + 0x24) = val;
    p->argcnt = 1;
    p->state = 2;
}

void AppHandler::hookStoryModeOff(SpliceHandler::reg *p) {
    auto &sdk = *ExecutionHandler::instance()->sdkHandler();
    auto &inst = *ExecutionHandler::instance()->appHandler();
    char *lvlName = sdk.getCurrentLevelName();
    inst.m_storyModeCnt++;
    if (lvlName) {
        switch (StringUtil::hash_rt(lvlName)) {
        case StringUtil::hash_ct("01_Intro"):
        case StringUtil::hash_ct("07_ATC_Roof"):
        case StringUtil::hash_ct("XP2_W01"):
        case StringUtil::hash_ct("XP2_W06"):
            return;
        }
    }
    //*(unsigned short *)(aData->aFreeMovement) = 0x9090;
    sdk.m_bfreeMovement = 0;
}

void AppHandler::hookfRateFix(SpliceHandler::reg *p) {
    float fRateNew = 1.0f;
    float fRate = reinterpret_cast<float &>(p->tax);
    if (fRate <= 0.0f)
        p->tax = reinterpret_cast<unsigned &>(fRateNew);
    /*CPlayerObj * pPlayerObj = (CPlayerObj *)p->tdi;
  fearData *pSdk = &handleData::instance()->pSdk;
    HCLIENT hClient = (HCLIENT) * (HCLIENT *)((unsigned char *)pPlayerObj +
                                            pSdk->CPlayerObj_m_hClient);
    if(!hClient)
      return;
  //GameClientData *pGameClientData = getGameClientData(hClient);
  unsigned clientId = pSdk->g_pLTServer->GetClientID(hClient);
  playerData *pPlData = &pSdk->pPlayerData[clientId];
  switch(p->v3){
    case 0x3A4: //dual pistols
    case 0x2DF: //g2a2
    case 0x366: //submachinegun
    case 0x130: //pistol
    case 0x350: //shotgun
    case 0x339: //penetrator
    case 0x384: //asp
    pPlData->isAimed = 0;
    //DBGLOG("AIM OFF")
    break;
    case 0x39D:
    case 0x2E0:
    case 0x367:
    case 0x12f:
    case 0x351:
    case 0x33A:
    case 0x385:
    pPlData->isAimed = 1;
    //DBGLOG("AIM ON")
  }*/
}

void AppHandler::hookOnAddClient_CheckNickname(SpliceHandler::reg *p)
{
    auto &sdk = *ExecutionHandler::instance()->sdkHandler();
    NetClientData *ncd = (NetClientData *)p->tax;
    p->state = 2;
    p->tax = ((uintptr_t(__thiscall *)(
        uintptr_t, uintptr_t))sdk.g_pGetNetClientData)(p->tsi, p->tax);
    if (p->tax) {
      int isUni = IS_TEXT_UNICODE_ILLEGAL_CHARS;
      IsTextUnicode((void *)ncd->m_szName,
                    wcslen(ncd->m_szName) * sizeof(wchar_t), &isUni);
      if (isUni & IS_TEXT_UNICODE_ILLEGAL_CHARS)
        p->tax = (uintptr_t)sdk.m_hUnarmedRecord;
    }
}

void AppHandler::hookEnterSlowMo(SpliceHandler::reg *p)
{
    auto &sdk = *ExecutionHandler::instance()->sdkHandler();
    if (sdk.m_currentRandomSlowMoInd == 13) {
      sdk.m_currentRandomSlowMoInd = 0;
      unsigned char *in = sdk.m_randomSlowmoTable;
      for (int i = 0; i != 13; i++) {
          in[i] = i;
      }
      shuffle(in, 13);
    }
    else
      sdk.m_currentRandomSlowMoInd++;
    p->v0 = (uintptr_t)sdk.m_hSlowMoHandles[sdk.m_randomSlowmoTable[rand()%13/*sdk.m_currentRandomSlowMoInd*/]];
    //p->v0 = (uintptr_t)sdk.m_hFastForward;
}

void AppHandler::hookPickupRndWeapon(SpliceHandler::reg *p)
{
    auto &sdk = *ExecutionHandler::instance()->sdkHandler();
    if (sdk.m_currentRandomWeapon != (HWEAPON)p->tax)
      p->tax = 0;
}

const char *wepsmpstr[] = {
    "Remote Charge",   "Pistol",        "Submachinegun",
    "Shotgun",         "Assault Rifle", "Nail Gun",
    "Semi-auto Rifle", "Cannon",        "Missile Launcher",
    "Plasma Weapon",   "Vulcan Cannon", "Turret_Helicopter",
    "Turret_Remote",   "Turret_Street", "Frag Grenade",
    "Proximity",       "Unarmed",       0};

const char *wepsmpstr_xp2[] = {
    "Remote Charge",   "Pistol",        "Submachinegun",
    "Shotgun",         "Assault Rifle", "Nail Gun",
    "Semi-auto Rifle", "Cannon",        "Missile Launcher",
    "Plasma Weapon",   "Vulcan Cannon", "Turret_Helicopter",
    "Turret_Remote",   "Turret_Street", "Frag Grenade",
    "Proximity",       "Unarmed", "AdvancedRifle" , "Minigun",
    "GrenadeLauncher", "LaserCarbine", "ChainLightningGun",
    "DeployableTurretGrenade", 0};



void AppHandler::hookRandomWeapon(SpliceHandler::reg *p)
{
    p->state = 2;
    auto &sdk = *ExecutionHandler::instance()->sdkHandler();
    unsigned timeMS = sdk.getRealTimeMS();
    unsigned wepCnt = sdk.isXP2 ? 23 : 17;
    if (!sdk.m_randomWeaponTime || timeMS - sdk.m_randomWeaponTime > 90000) {
      sdk.m_randomWeaponTime = timeMS;
      if (sdk.m_currentRandomWeaponInd == wepCnt) {
        sdk.m_currentRandomWeaponInd = 0;
        unsigned char *in = sdk.m_randomWepTable;
        for (int i = 0; i != wepCnt; i++) {
            in[i] = i;
        }
        shuffle(in, wepCnt);
      }
      else
        sdk.m_currentRandomWeaponInd++;
      const char *szWep =
          sdk.isXP2 ? wepsmpstr_xp2[sdk.m_randomWepTable[sdk.m_currentRandomWeaponInd]] : wepsmpstr[sdk.m_randomWepTable[sdk.m_currentRandomWeaponInd]];
      sdk.m_currentRandomWeapon =
          sdk.g_pLTDatabase->GetRecordB(sdk.m_hCatWeapons, szWep);
      ((void(__thiscall *)(void *, const char *, uintptr_t,
                           uintptr_t))sdk.CCommandMgr_QueueCommand)(
          sdk.pCmdMgr, "msg Player RESETINVENTORY", 0, 0);
      char str[64];
      sprintf(str, "msg Player (ACQUIREWEAPON %s)", szWep);
      ((void(__thiscall *)(void *, char *, uintptr_t, uintptr_t))
           sdk.CCommandMgr_QueueCommand)(sdk.pCmdMgr, str, 0, 0);
      int i = 0;
      for (; i != 5; i++) {
        sprintf(str, "msg Player (ACQUIREAMMO %s)", szWep);
        ((void(__thiscall *)(void *, char *, uintptr_t, uintptr_t))
             sdk.CCommandMgr_QueueCommand)(sdk.pCmdMgr, str, 0, 0);
      }
//      ((void(__thiscall *)(void *, const char *, uintptr_t,
//                           uintptr_t))sdk.CCommandMgr_QueueCommand)(
//          sdk.pCmdMgr, "msg Player (ANIMATE DSlumpB)", 0, 0);
//      if (!sdk.m_randomWepTable[sdk.m_currentRandomWeaponInd]) {
//        ((void(__thiscall *)(void *, const char *, uintptr_t,
//                             uintptr_t))sdk.CCommandMgr_QueueCommand)(
//            sdk.pCmdMgr, "msg Player (CHANGEWEAPON Unarmed)", 0, 0);
//      } else {
//        sprintf(str, "msg Player (CHANGEWEAPON %s)", szWep);
//        ((void(__thiscall *)(void *, char *, uintptr_t, uintptr_t))
//             sdk.CCommandMgr_QueueCommand)(sdk.pCmdMgr, str, 0, 0);
//      }

      p->tax = 0;
    } else
      p->tax = (uintptr_t)sdk.m_currentRandomWeapon;
}

void AppHandler::hookComposeArchives(SpliceHandler::reg *p){
    char *str = (char *)p->tdi;
    if (str) {
      if (strstr(str, "AdditionalContent")) {
        p->tcx = p->tcx | 1;
      }
    }
}

void AppHandler::hookGameMode(SpliceHandler::reg *p) {
    memcpy((void *)p->tax, L"SinglePlayer", 13 * 2);
    wchar_t *val = (wchar_t *)((unsigned char *)p->tax - 0x208);
    memcpy((void *)val, L"SinglePlayer", 13 * 2);
    memcpy((void *)((unsigned char *)p->tsi + 4), "SinglePlayer", 13);
}

void AppHandler::hookGameMode2(SpliceHandler::reg *p) {
    memcpy((void *)p->tax, "SinglePlayer", 13);
}


void AppHandler::configParse(char *pathCfg)
{
    m_preventNoclip = getCfgInt(pathCfg, (char *)"PreventNoclip", 1);
    // bIgnoreUnusedMsgID = getCfgInt(pathCfg, (char *)"PreventSpecialMsg");
    m_bSyncObjects = getCfgInt(pathCfg, (char *)"SyncObjects");
    bCustomSkins = getCfgInt(pathCfg, (char *)"CustomSkins");
    m_bCoop = getCfgInt(pathCfg, (char *)"CoopMode");
    m_bAdditionalContent = getCfgInt(pathCfg, (char *)"AdditionalContent");
    m_bRandWep = getCfgInt(pathCfg, (char *)"RandomWeapon");
    m_bBotsMP = getCfgInt(pathCfg, (char *)"BotsMP");
    // pSdk->freeMovement=bPreventNoclip;
    //m_preventNoclip = 2;
    //bIgnoreUnusedMsgID = 1;
    if (m_bCoop) {
      {
        unsigned char *tmp = scanBytes(
            (unsigned char *)m_Exec, m_ExecSz,
            BYTES_SEARCH_FORMAT("8B??????????????FF????8B??????89??????????8B"));
        if (tmp) {
          m_spliceHandler->spliceUp(tmp, (void *)hookGameMode2);
        }
      }
    }
}

void AppHandler::hookConfigLoad(SpliceHandler::reg *p)
{
    auto &inst = *ExecutionHandler::instance()->appHandler();
    if (p->tax) {
      inst.configParse((char *)p->tax);
    }
}

void AppHandler::hookConfigLoad2(SpliceHandler::reg *p)
{
    auto &inst = *ExecutionHandler::instance()->appHandler();
    if (p->tax) {
        if (!inst.m_bConfigInitialized) {
            inst.m_bConfigInitialized = 1;
            inst.configHandle();
        }
    }
}

void AppHandler::hookAdditionalDirectoryCreate(SpliceHandler::reg *hook)
{
    auto &inst = *ExecutionHandler::instance()->appHandler();
    auto tempPath = reinterpret_cast<char*>(hook->tax);
    ((void(__cdecl *)(char *))inst.m_internalFuncPathCreateFolders)(tempPath);
    /*
    auto tempPath = reinterpret_cast<char*>(hook->tax);
    // char fullPath[MAX_PATH];
    if (GetFileAttributesA(tempPath) != INVALID_FILE_ATTRIBUTES)
        return;
    // char tempPath[MAX_PATH];
    // strcpy(tempPath, relPath);

    char* p = tempPath;

    while (*p) {
        // Move forward in the string until we find a directory separator or end
        while (*p && *p != '\\' && *p != '/') {
            ++p;
        }

        // Temporarily end the string here to create intermediate directory
        char oldChar = *p;
        *p = '\0';

        // if (!CreateDirectoryA(tempPath, NULL)) return;
        CreateDirectoryA(tempPath, NULL);
        // {
            // DWORD error = GetLastError();
            // if (error != ERROR_ALREADY_EXISTS) {
            //     printf("Failed to create directory '%s'. Error code: %lu\n", tempPath, error);
            //     return; // Exit on error
            // }
        // }

        // Restore the character (either '\' or '/') and continue
        *p = oldChar;

        // Move past the separator if we are not at the end
        if (*p) {
            ++p;
        }
    }
    // if(GetFullPathNameA(relPath, MAX_PATH, fullPath, nullptr)){
    //     MessageBoxA(nullptr, fullPath, relPath, 0);
    //     //SHCreateDirectoryExA(NULL, fullPath, NULL);
    // }
    */
}

void AppHandler::hookLoadGameServer(SpliceHandler::reg *p)
{
    auto &inst = *ExecutionHandler::instance()->appHandler();
    auto &hsplice = *inst.spliceHandler();
    auto &hpatch = *inst.patchHandler();
    // inst.m_Server = reinterpret_cast<uint8_t *>(p->tax);
    // inst.m_ServerSz = GetModuleSize((HMODULE)inst.m_Server);
    {
      inst.m_ServerModule = reinterpret_cast<uint8_t *>(p->tax);
      auto exec_section =
          GetModuleFirstExecSection(reinterpret_cast<HMODULE>(inst.m_ServerModule));
      inst.m_Server = reinterpret_cast<uint8_t *>(inst.m_ServerModule +
                                           exec_section->VirtualAddress);
      inst.m_ServerSz = exec_section->SizeOfRawData;
    }
    {
        static auto pat = BSF("E8????????8BC8E8????????8D4C????FF15????????8B");
        unsigned char *tmp = scanBytes(
            (unsigned char *)inst.m_Server, inst.m_ServerSz, reinterpret_cast<uint8_t *>(&pat)); // Gamemode
        // reset Config
        // fix
        if (tmp) {
            hpatch.addCode(tmp, 2);
            *(unsigned short *)(tmp) = 0x0AEB;
        }
    }
    {
      static auto pat = BSF("558BEC83E4??81EC????????????8B????85????89??????0F??????????8B????85??0F??????????33C0");
      hsplice.spliceUp(
          scanBytes((unsigned char *)inst.m_Server, inst.m_ServerSz, reinterpret_cast<uint8_t *>(&pat)),
          (void *)hookConfigLoad2);
    }
}

// StarCompare() helper function
bool regcall starCompareHelper(char *str, char *pat) {
  if (*pat == '\0')
    return 1;
  if (*pat == '*') {
    if (*str) {
      // advance str and use the * again
      if (starCompareHelper(str + 1, pat))
        return 1;
    }
    // let * match nothing and advacne to the next pattern
    return starCompareHelper(str, pat + 1);
  }
  if (*pat == *str) {
    return starCompareHelper(str + 1, pat + 1);
  }
  return 0;
}

bool regcall starCmp(char *str, char *pat) {
  if (!str || !pat)
    return 0;
  do {
    if (starCompareHelper(str, pat))
      return 1;
  } while (*str++);
  return 0;
}


void AppHandler::hookLoadObject(SpliceHandler::reg *p){
    auto &inst = *ExecutionHandler::instance()->appHandler();
    if(!inst.m_bDisableLights)
        return;
    char ** pstr = (char**)p->v3;
    if(pstr) {
        char * str = *pstr;
        bool matched = false;
        if (starCmp(str, (char *)"*Light*")) {
            if(strcmp(str, "LightSpot") == 0 || strcmp(str, "LightCube") == 0)
                matched = false;/*rand() % 2;*/
            else matched = true;
        }
        else if(starCmp(str, (char *)"*Sky*") ||/* starCmp(str, (char *)"*Render*") ||*/ strcmp(str, (char *)"WorldProperties") == 0) {
            matched = true;
        }
        if(matched) {
            p->origFunc=reinterpret_cast<uintptr_t>((p->pt->hookPoint)+132);
        }
    }
//    char *str = (char *)p->tdi;
//    if (str) {
//      if (strstr(str, "AdditionalContent")) {
//        p->tcx = p->tcx | 1;
//      }
//    }
}

void AppHandler::hookSetObjFlags(SpliceHandler::reg *p){
    auto &sdk = *ExecutionHandler::instance()->sdkHandler();
    uintptr_t *pObj = (uintptr_t *)p->tsi;
    if (pObj) {
      char *objName =
          (char *)((unsigned char *)pObj + sdk.ObjectCreateStruct_m_Name);
      // if((pObj[1] & FLAG2_RIGIDBODY) || (pObj[1] & FLAG2_CLIENTRIGIDBODY)){
      bool isMatch = 0;
      // if (starCmp(objName, (char *)"VendingMachine*.Base"))
      //   isMatch = 1;
      // if (starCmp(objName, (char *)"VendingMachine*.glow"))
      //   isMatch = 1;
      if (starCmp(objName, (char *)"Table_Utililty_*.Base"))
        isMatch = 1;
      if (starCmp(objName, (char *)"Table_Utililty_*.glow"))
        isMatch = 1;
      if (starCmp(objName, (char *)"industrial_fridge*.Base"))
        isMatch = 1;
      if (starCmp(objName, (char *)"industrial_fridge*.glow"))
        isMatch = 1;
      if (starCmp(objName, (char *)"Sofa-2Seat*_Flip*.Sofa2"))
        isMatch = 1;

      if (isMatch) {
        pObj[2] = 0x40;
      }

    }
}

void AppHandler::hookStoryModeView(SpliceHandler::reg *p) {
    auto &inst = *ExecutionHandler::instance()->appHandler();
    uint8_t *adr = (uint8_t *)*(uintptr_t *)(inst.m_storyModeStruct);
    bool state = 0;
    if (!*(uint8_t *)(adr + 0x678))
        state = 0;
    else {
        if (!*(uint8_t *)(adr + 0x679))
            state = 0;
        else
            state = 1;
    }
    p->tax = state;
}

void AppHandler::hookClientGameMode(SpliceHandler::reg *p) {
    auto &inst = *ExecutionHandler::instance()->appHandler();
    char *str = (char *)*(uintptr_t *)p->tax;
    if (StringUtil::hash_rt(str) == StringUtil::hash_ct("SinglePlayer")) {
       inst.setFlashlight(1);
    } else {
        inst.setFlashlight(0);
    }
}

void AppHandler::hookModelMsg(SpliceHandler::reg *p) {
    if (p->v1) switch (StringUtil::hash_rt((char *)p->v1)) {
        case StringUtil::hash_ct("THROW"):
            if (p->v0 == 0x19)
                p->origFunc =
                    (uintptr_t)p->pt->origFunc + 2;  // skip call ebx
            break;
        }
}

void AppHandler::hookClientSettingsLoad(SpliceHandler::reg *p) {
    auto &inst = *ExecutionHandler::instance()->appHandler();

    if (!inst.m_bSettingsLoaded) {
        inst.m_bSettingsLoaded = true;
        {
            static auto pat1 = BSF("8B0D????????85C9A3????????A1????????0F95C3");
            uint8_t *tmp =
                scanBytes(inst.m_Exec, inst.m_ExecSz, reinterpret_cast<uint8_t *>(&pat1));
            if (tmp) {
                if (**reinterpret_cast<uintptr_t **>(tmp + 2)) {
                    {
                        static auto pat = BSF("0F95C385C0885C24??74??B9");
                        uint8_t *tmp =
                            scanBytes(inst.m_Exec, inst.m_ExecSz, reinterpret_cast<uint8_t *>(&pat));
                        if (tmp) {
                            unprotectCode(tmp, 3);
                            *reinterpret_cast<uint16_t *>(tmp) = 0xDB30;
                            *reinterpret_cast<uint8_t *>(tmp + 2) = 0x90;
                        }
                    }
                    {
                        static auto pat = BSF("0F????????89??????????8B??????????89");
                        uint8_t *tmp = scanBytes(
                            inst.m_Exec, inst.m_ExecSz, reinterpret_cast<uint8_t *>(&pat));
                        if (tmp) {
                            unprotectCode(tmp, 5);
                            uint8_t d[] = {0xB9, 0x01, 0x00, 0x00, 0x00};
                            memcpy(tmp, d, 5);
                        }
                    }
                    {
                        static auto pat = BSF("7402B0018B????8B????E8????????E8????????5F");
                        uint8_t *tmp = scanBytes(
                            inst.m_Exec, inst.m_ExecSz, reinterpret_cast<uint8_t *>(&pat));
                        if (tmp) {
                            unprotectCode(tmp, 2);
                            *reinterpret_cast<uint16_t *>(tmp) = 0x9090;
                        }
                    }
                    {
                        static auto pat = BSF("6A00C705????????01000000C705????????000000008B116A06FF520C");
                        uint8_t *tmp =
                            scanBytes(inst.m_Exec, inst.m_ExecSz, reinterpret_cast<uint8_t *>(&pat));
                        if (tmp) {
                            unprotectCode(tmp, 15);
                            *reinterpret_cast<uint16_t *>(tmp) = 0x9090;
                            *reinterpret_cast<uint16_t *>(tmp + 12) = 0x43EB;
                        }
                    }
                    {
                      static auto pat = BSF("753468????????FF15????????E8????????85C074206A006A30");
                      uint8_t *tmp =
                          scanBytes(inst.m_Exec, inst.m_ExecSz, reinterpret_cast<uint8_t *>(&pat));
                      if (tmp) {
                        unprotectCode(tmp, 1);
                        *tmp = 0xEB;
                        }
                    }
                }
            }
        }
    }
}


struct readMsg {
  int8_t pad4[4];
  int32_t f4;
  int32_t f8;
  uint32_t f12;
  void *f16;
  uint32_t f20;
  uint32_t f24;
};
//extern float __cdecl sinf(float _X);
//extern float __cdecl cosf(float _X);
#include <math.h>
void AppHandler::hookMID(SpliceHandler::reg *p){
    p->argcnt = 2;
    auto &sdk = *ExecutionHandler::instance()->sdkHandler();
    auto &inst = *ExecutionHandler::instance()->appHandler();
    readMsg *pMsg = (readMsg *)((unsigned char *)p->v1 + 8);
    uintptr_t v1 = pMsg->f12, v2 = (uintptr_t)pMsg->f16, v3 = pMsg->f20,
              v4 = pMsg->f24;
    CAutoMessageBase_Read *pMsgRead = (CAutoMessageBase_Read *)p->v1;
    HOBJECT hClientObj = sdk.GetClientObject((HCLIENT)p->v0);
    // if (!hClientObj) return;
    GameClientData *pGameClientData = sdk.getGameClientData((HCLIENT)p->v0);
    if (!pGameClientData) {
      p->state = 1;
      return;
    }
    unsigned clientId = sdk.g_pLTServer->GetClientID((HCLIENT)p->v0);
    CPlayerObj *pPlayerObj = sdk.GetPlayerFromHClient((HCLIENT)p->v0);
    /*if (!pPlayerObj) {
      p->state = 1;
      return;
    }*/
    playerData *pPlData = &sdk.pPlayerData[clientId];
    unsigned playerState = 0;
    if (pPlayerObj) {
      playerState = *(unsigned *)((unsigned char *)pPlayerObj +
                                  sdk.CPlayerObj_m_ePlayerState);
      if (inst.m_bCoop && !pPlData->bIsDead &&
          playerState == ePlayerState_Dying_Stage2 /*&& pSdk->checkPointState*/) {
        EnterCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
        pPlData->bIsDead = 1;
        LeaveCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
        if (*(unsigned char *)((unsigned char *)pPlayerObj +
                               sdk.CCharacter_m_bOnGround)) {
          // HCLIENT playerClient = *(HCLIENT*)((unsigned char
          // *)pPlayerObj+0x2880);
          HOBJECT hPlayerObj = sdk.GetClientObject((HCLIENT)p->v0);
          sdk.g_pLTServer->GetObjectPos(hPlayerObj, &sdk.checkPointPos);
          sdk.checkPointPos.y += 50.0f;
          // newPos = pSdk->checkPointPos;
          sdk.checkPointState = 2;
        }
      }
    }
    uint8_t nMessageId = pMsgRead->ReadBits(8);
    if (!*(unsigned char *)(pGameClientData + 0x7C) /*m_bPassedSecurity*/ &&
        nMessageId != MID_MULTIPLAYER_UPDATE &&
        nMessageId != MID_CLIENTCONNECTION) {
      p->state = 1;
      return;
    }
    switch (nMessageId) {
    case MID_FRAG_SELF:
    case MID_DECISION:
    case MID_WEAPON_FINISH:
    case MID_WEAPON_FINISH_RAGDOLL:
    case MID_OBJECT_ALPHA:
    case MID_RENDER_STIMULUS:
    case MID_STIMULUS:
    case MID_PLAYER_TELEPORT:
    case MID_DO_DAMAGE:
    case MID_GAME_PAUSE:
    case MID_GAME_UNPAUSE:
    case MID_SOUND_BROADCAST_DB:
    case MID_START_LEVEL:
    case MID_MULTIPLAYER_OPTIONS:
    case MID_PUNKBUSTER_MSG:
    case MID_SONIC:
    case MID_AIDBUG:
    case MID_ADD_GOAL:
    case MID_REMOVE_GOAL:
    case MID_DYNANIMPROP:
      p->state = 1;
      break;
    case MID_PLAYER_ANIMTRACKERS: {
      if (!pPlayerObj) {
        p->state = 1;
        break;
      }
      unsigned nAnimTrackerMsgId = pMsgRead->ReadBits(8);
      if (nAnimTrackerMsgId == MID_ANIMTRACKERS_ADD &&
          nAnimTrackerMsgId == MID_ANIMTRACKERS_REMOVE) {
        p->state = 1;
        break;
      }
      unsigned nNumTrackers = pMsgRead->ReadBits(8);
      uint32_t nValueIndex = 0;
      const char *const pszNullWeightSetName = sdk.g_pLTDatabase->GetString(
          sdk.g_pLTDatabase->GetAttribute(sdk.m_hGlobalRecord,
                                          "NullWeightSetName"),
          nValueIndex, "");
      HMODELWEIGHTSET hWS = INVALID_MODEL_WEIGHTSET;
      sdk.g_pModelLT->FindWeightSet(pPlayerObj->m_hObject, pszNullWeightSetName,
                                    hWS);
      for (uint8_t nTracker = 0; nTracker < nNumTrackers; ++nTracker) {
        unsigned trkID = pMsgRead->ReadBits(8);
        if (trkID != 0xFF) {  // MAIN_TRACKER
          unsigned hWeightSet = pMsgRead->ReadBits(0x20);
          if (hWeightSet != hWS) {
            p->state = 1;
            break;
          }
        }
      }
      // if(nAnimTrackerMsgId == MID_ANIMTRACKERS_ADD) {
      //   unsigned m_nFootstepTrackerId = pMsgRead->ReadBits(8);
      //   DBGLOG("footsteps %p", m_nFootstepTrackerId)
      // }

      break;
    }
    case MID_PLAYER_ACTIVATE: {
      if (!pPlayerObj) {
        p->state = 1;
        break;
      }
      CArsenal *pArsenal =
          (CArsenal *)((unsigned char *)pPlayerObj + sdk.CPlayerObj_m_Arsenal);
      if (!pArsenal) {
        p->state = 1;
        break;
      }
      LTRotation rTrueCameraRot;
      LTVector curPos, objPos;
      pMsgRead->ReadData((void *)&objPos, 0x60);
      pMsgRead->ReadCompLTRotation(&rTrueCameraRot);
      unsigned type = pMsgRead->ReadBits(8);
      unsigned curTime = sdk.getRealTimeMS();
      HOBJECT hObject = pMsgRead->ReadObject();
      bool bPosInvalid = 0;
      sdk.g_pLTServer->GetObjectPos(hClientObj, &curPos);
      sdk.g_pLTServer->GetObjectPos(hObject, &objPos);
      if (!curPos.NearlyEquals(objPos, 256.0f)) {
        bPosInvalid = 1;
      }
      switch (type) {
      case MID_ACTIVATE_TURRET:
        bPosInvalid = 0;
        break;
      case MID_ACTIVATE_LADDER:
        if (!pPlayerObj) {
          p->state = 1;
          break;
        }
        bPosInvalid = 0;
        hObject ? pPlData->bLadderInUse = 1 : pPlData->bLadderInUse = 0;
        if (pPlData->onChangeWeaponHWEAPON !=
            *(HWEAPON *)((unsigned char *)pArsenal +
                         sdk.CArsenal_m_hCurWeapon)) {
          unsigned delta = (unsigned)(curTime - pPlData->onChangeWeaponTime);
          if (delta <= 1536) {
            // if (*(HOBJECT *)((unsigned char *)pPlayerObj +
            //                 pSdk->CCharacter_m_hLadderObject))
            /*((void(__thiscall *)(CPlayerObj *)) *
             (uintptr_t *)((unsigned char *)*(uintptr_t *)pPlayerObj +
                           pSdk->CPlayerObj_DropCurrentWeapon))(pPlayerObj);*/

            // pSdk->resetToUnarmed(pPlayerObj);
            // pPlData->bResetToUnarmed=1;
            ((void(__thiscall *)(CPlayerObj *, HWEAPON, bool, HAMMO, bool,
                                 bool))sdk.CPlayerObj_ChangeWeapon)(
                pPlayerObj, sdk.m_hUnarmedRecord, 1, sdk.m_hUnarmedRecordAmmo,
                0, 0);
          }
        }
        break;
      }
      if (bPosInvalid)
        p->state = 1;

    } break;
    case MID_WEAPON_CHANGE: {
      if (!pPlayerObj) {
        p->state = 1;
        break;
      }
      CArsenal *pArsenal =
          (CArsenal *)((unsigned char *)pPlayerObj + sdk.CPlayerObj_m_Arsenal);
      if (!pArsenal) {
        p->state = 1;
        break;
      }
      EnterCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
      pPlData->speedPrev = -1.0f;
      pPlData->invalidSpeedCnt = 0;
      pPlData->onChangeWeaponHWEAPON =
          *(HWEAPON *)((unsigned char *)pArsenal + sdk.CArsenal_m_hCurWeapon);
      pPlData->onChangeWeaponTime = sdk.getRealTimeMS();
      LeaveCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
      HWEAPON hWeapon =
          pMsgRead->ReadDatabaseRecord(sdk.g_pLTDatabase, sdk.m_hCatWeapons);
      HAMMO hAmmo =
          pMsgRead->ReadDatabaseRecord(sdk.g_pLTDatabase, sdk.m_hCatAmmo);
      HWEAPONDATA hWpnData = 0;
      if (hWeapon) {
        hWpnData = sdk.GetWeaponData(hWeapon);
        HAMMO hAmmoServ = sdk.g_pLTDatabase->GetRecordLink(
            sdk.g_pLTDatabase->GetAttribute(hWpnData, _C("AmmoName")), 0, 0);
        if (hAmmo && hAmmoServ != hAmmo)
          p->state = 1;
      } else
        p->state = 1;
      // ((void(__thiscall *)(uintptr_t, uintptr_t, uintptr_t))p->hook)(
      //     p->tcx, p->v0, p->v1);
      // p->state = 1;

    } break;
    case MID_OBJECT_MESSAGE: {
      HOBJECT hTarget = pMsgRead->ReadObject();
      unsigned val = pMsgRead->ReadBits(0x20);
      switch (val) {
      case MID_SFX_MESSAGE:
      case 0xFF:
        break;
      default:
        p->state = 1;
      }
    } break;
    case MID_SLOWMO: {
      if (!pPlayerObj) {
        p->state = 1;
        break;
      }
      void *m_Inventory =
          (((unsigned char *)pPlayerObj + sdk.CPlayerObj_m_Inventory));
      HGEAR m_hSlowMoGearRecord = *(HGEAR *)((unsigned char *)m_Inventory + 0x58);
      if (!inst.m_bCoop) { // everyone can use & no wait for charge
        if (m_Inventory && !m_hSlowMoGearRecord) {
          p->state = 1;
          break;
        }
        float m_fSlowMoCharge = *(float *)((unsigned char *)m_Inventory + 0x5C);
        float m_fSlowMoMaxCharge =
            *(float *)((unsigned char *)m_Inventory + 0x64);
        float fPercent = m_fSlowMoMaxCharge * 0.20f;
        if ((m_fSlowMoMaxCharge - fPercent) >= m_fSlowMoCharge)
          p->state = 1;
      }
    } break;
    case MID_PLAYER_GHOSTMESSAGE:
    case MID_PLAYER_MESSAGE: {
      wchar_t text[64];
      unsigned len = pMsgRead->ReadWString(text, sizeof(text) / sizeof(wchar_t)) *
                     sizeof(wchar_t);
      if (len > (63 * sizeof(wchar_t)))
        p->state = 1;
    } break;
    case MID_PLAYER_SPECTATORMODE: {
      p->state = sdk.checkPlayerStatus(pGameClientData);
      if (p->state) {
        sdk.g_pLTServer->KickClient((HCLIENT)p->v0);
        p->state = 1;
      }
      if ((playerState == ePlayerState_Alive) ||
          (playerState == ePlayerState_Dying_Stage2) ||
          (playerState == ePlayerState_Dying_Stage1)) {
        p->state = 1;
        break;
      }
    }
    case MID_PLAYER_RESPAWN: {
      // TODO: nullify on malloc
      // auto prevSpawn = sdk.pPlayerData[clientId].spawnTime;
      EnterCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
      memset(&sdk.pPlayerData[clientId], 0, sizeof(playerData));
      LeaveCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
      // auto currentSpawnTime = sdk.getRealTimeMS();
      // sdk.pPlayerData[clientId].spawnTime = currentSpawnTime;
     //  if((currentSpawnTime - prevSpawn) < 2000) {
     //    p->state = 1;
     //    break;
     // }
      p->state = sdk.checkPlayerStatus(pGameClientData);
      if (p->state) {
        sdk.g_pLTServer->KickClient((HCLIENT)p->v0);
        p->state = 1;
        // pSdk->BootWithReason(pGameClientData,
        // eClientConnectionError_PunkBuster,
        //                     (char *)"Respawn fail");
      }
      if ((playerState == ePlayerState_Alive) ||
          (playerState == ePlayerState_Dying_Stage2) ||
          (playerState == ePlayerState_Dying_Stage1)) {
        p->state = 1;
        break;
      }
    } break;
    case MID_PICKUPITEM_ACTIVATE:
    case MID_PICKUPITEM_ACTIVATE_EX: {
      HOBJECT hTarget = pMsgRead->ReadObject();
      LTVector targetPos, curPos;
      sdk.g_pLTServer->GetObjectPos(hTarget, &targetPos);
      sdk.g_pLTServer->GetObjectPos(hClientObj, &curPos);
      HOBJECT hFiredFrom = sdk.HandleToObject(hTarget);
      if (hFiredFrom) {
        // HWEAPONDATA hWpnData = pSdk->GetWeaponData(hTarget);
        // HAMMO hAmmo = pSdk->g_pLTDatabase->GetRecordLink(
        //     pSdk->g_pLTDatabase->GetAttribute(hWpnData, "AmmoName"), 0, 0);
        //
        HAMMO hWeapon =
            *(void **)((uint8_t *)hFiredFrom + 0xDC); // 0xDC - weapon,0xE0 - ammo
        if (hWeapon) {
          HWEAPONDATA hWeaponData = sdk.GetWeaponData(hWeapon);
          if (hWeaponData) {
            bool IsGrenade = ((bool(__thiscall *)(
                CWeaponDB *, HAMMODATA, const char *, uintptr_t,
                uintptr_t))sdk.g_pWeaponDB_GetBool)(
                sdk.g_pWeaponDB, hWeaponData, "IsGrenade", 0, 0);
            if (IsGrenade &&
                ((bool(__thiscall *)(void *, HOBJECT))
                     sdk.CGrenadeProximity_IsEnemy)(hFiredFrom, hClientObj)) {
              p->state = 1;
            }
            hFiredFrom = *(void **)((uint8_t *)hFiredFrom + 0xF4);

            if (IsGrenade && hFiredFrom != hClientObj)
              p->state = 1;
          }
        }
      }

      if (!curPos.NearlyEquals(targetPos, 256.0f)) {
        p->state = 1;
      }
    } break;
    case MID_WEAPON_RELOAD: {
      // pPlData->lastFireWeaponReload=1;
      if (!pPlayerObj || playerState != ePlayerState_Alive) {
        p->state = 1;
        break;
      }
      HWEAPON hWeapon = 0;
      HWEAPONDATA hWpnData = 0;
      CArsenal *pArsenal =
          (CArsenal *)((unsigned char *)pPlayerObj + sdk.CPlayerObj_m_Arsenal);
      if (!pArsenal) {
        p->state = 1;
        break;
      }
      hWeapon =
          pMsgRead->ReadDatabaseRecord(sdk.g_pLTDatabase, sdk.m_hCatWeapons);
      // bIsNul++;
      HAMMO hAmmo = 0;
      hAmmo = pMsgRead->ReadDatabaseRecord(sdk.g_pLTDatabase, sdk.m_hCatAmmo);
      // bIsNul++;
      if (!hWeapon && hAmmo) p->state = 1;
      if (hWeapon) {
        hWpnData = sdk.GetWeaponData(hWeapon);
        HAMMO hAmmoServ = sdk.g_pLTDatabase->GetRecordLink(
            sdk.g_pLTDatabase->GetAttribute(hWpnData, "AmmoName"), 0, 0);
        if (hAmmo && hAmmoServ != hAmmo) p->state = 1;
      }
    } break;
    case MID_WEAPON_FIRE: {
      if (!pPlayerObj || playerState != ePlayerState_Alive) {
        p->state = 1;
        break;
      }

      // unsigned char bIsNul=0;
      LTVector firePos, curPos, vPath;
      HWEAPON hWeapon = 0;
      CArsenal *pArsenal =
          (CArsenal *)((unsigned char *)pPlayerObj + sdk.CPlayerObj_m_Arsenal);
      if (!pArsenal) {
        p->state = 1;
        break;
      }
      if (pMsgRead->ReadBits(0x1)) {
        hWeapon = pMsgRead->ReadDatabaseRecord(sdk.g_pLTDatabase,
                                               sdk.m_hCatWeapons);
        // bIsNul++;
      }
      HAMMO hAmmo = 0;
      if (pMsgRead->ReadBits(0x1)) {
        hAmmo =
            pMsgRead->ReadDatabaseRecord(sdk.g_pLTDatabase, sdk.m_hCatAmmo);
        // bIsNul++;
      }
      if (!hWeapon && hAmmo)
        p->state = 1;

      unsigned ammoType = 0;
      HAMMODATA hAmmoData = 0;
      HWEAPONDATA hWpnData = 0;
      float dist = 256.0f;
      bool bUnarmed = 0;
      unsigned unarmFireDelay = 0;
      bool spawnLog = false;
      sdk.g_pLTServer->GetObjectPos(hClientObj, &curPos);
      if (hWeapon &&
          StringUtil::hash_rt((char *)*(uintptr_t *)(hWeapon)) == StringUtil::hash_ct("Unarmed")) {
        if (hAmmo) {
          unsigned hAnim = pPlData->hAnim;
          unsigned hAnimPenult = pPlData->hAnimPenult;
          if (sdk.isXP2) {
            hAnim++;
            hAnimPenult++;
          }
          if (!inst.bCustomSkins) {
            switch (StringUtil::hash_rt((char *)*(uintptr_t *)(hAmmo))) {
            case StringUtil::hash_ct("Melee_JabRight"):
                spawnLog = true;
            case StringUtil::hash_ct("Melee_JabLeft"):
              // unarmFireDelay = 0x100;
              if (hAnimPenult >= 0x10C && hAnimPenult <= 0x116)
                break;
              p->state = 1;
              break;
            case StringUtil::hash_ct("Melee_RifleButt"):
              // unarmFireDelay = 0x100;

            //          auto rot = LTRotation();

//                  auto q = pPlData->camRot;
//                  LTVector newPos;
//                  constexpr double PI = 3.141592653589793238463;
//                  float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
//                  float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
//                  float roll = std::atan2(sinr_cosp, cosr_cosp);

//                  // pitch (y-axis rotation)
//                  float sinp = 2 * (q.w * q.y - q.z * q.x);
//                  float pitch;
//                  if (std::abs(sinp) >= 1)
//                      pitch = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
//                  else
//                      pitch = std::asin(sinp);

//                  // yaw (z-axis rotation)
//                  float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
//                  float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
//                  float yaw = atan2(siny_cosp, cosy_cosp);

//                  float pitch = atan2(rot.m_Quat[1], -rot.m_Quat[2]);
//                  float pitchAngle = pitch * (180 / PI);
//                  float yaw = atan2(rot.m_Quat[0], -rot.m_Quat[2]);
//                  float yawAngle = yaw * (180 / PI);
//                  float roll = atan2(rot.m_Quat[0], -rot.m_Quat[1]);
//                  float rollAngle = roll * (180 / PI);
//                  constexpr float forwardMod = 100.0f;
//                  printf("%lf %lf %lf\n", pitchAngle, yawAngle, rollAngle);
//                  newPos[0] = curPos[0] + (forwardMod * cosf(pitchAngle));
//                  newPos[1] = curPos[1] + (forwardMod * sinf(yawAngle));
//                  newPos[2] = curPos[2]; //+ (forwardMod * cosf(rollAngle));
//                  newPos[0] = curPos[0] + (forwardMod * roll);
//                  newPos[1] = curPos[1] + (forwardMod * pitch);
//                  newPos[2] = curPos[2] + (forwardMod * yaw);
//                  newPos[2] = curPos[2] + (forwardMod * (cosf(yawRadian) * cosf(pitchRadian)));
//                  newPos[0] = curPos[0] + (forwardMod * (sinf(yaw) * cosf(pitch)));
//                  newPos[1] = curPos[1] + (forwardMod * -sinf(pitch));
//                  newPos[2] = curPos[2] + (forwardMod * (cosf(yaw) * cosf(pitch)));

              // printf()
              // if (hAnimPenult == 0x343 || hAnimPenult == 0x34C)
              //     break;
              //   p->state = 1;
              break;
            case StringUtil::hash_ct("Melee_SlideKick"):
              // unarmFireDelay = 0x16;
              if (sdk.isXP2) {
                if (hAnim == 0x12C || hAnim == 0x47D)
                  break;
                p->state = 1;
              } else if (hAnim != 0x12B)
                p->state = 1;
              break;
            case StringUtil::hash_ct("Melee_RunKickLeft"):
            case StringUtil::hash_ct("Melee_RunKickRight"):
              // unarmFireDelay = 0x50;
              if (hAnim != 0x12E)
                p->state = 1;
              break;
            case StringUtil::hash_ct("Melee_JumpKick"):
              // unarmFireDelay = 0x30;
              if (hAnim != 0x12D)
                p->state = 1;
              break;
            default:
              p->state = 1;
              break;
            }
          }else{
              switch (StringUtil::hash_rt((char *)*(uintptr_t *)(hAmmo))) {
              case StringUtil::hash_ct("Melee_JabRight"):
              case StringUtil::hash_ct("Melee_JabLeft"):
                // unarmFireDelay = 0x100;
                break;
              case StringUtil::hash_ct("Melee_RifleButt"):
                // unarmFireDelay = 0x100;
                break;
              case StringUtil::hash_ct("Melee_SlideKick"):
                // unarmFireDelay = 0x16;
                break;
              case StringUtil::hash_ct("Melee_RunKickLeft"):
              case StringUtil::hash_ct("Melee_RunKickRight"):
                // unarmFireDelay = 0x50;
                break;
              case StringUtil::hash_ct("Melee_JumpKick"):
                // unarmFireDelay = 0x30;
                break;
              default:
                p->state = 1;
                break;
              }
          }
//           if(p->state)
//            printf("fired: %s %p %p %p\n",(char *)*(uintptr_t
//           *)(hAmmo),hAnim,hAnimPenult,p->state);
        }
        dist = 150.0f;
        bUnarmed = 1;
      }
      if (!bUnarmed && hWeapon) {
        hWpnData = sdk.GetWeaponData(hWeapon);
        HAMMO hAmmoServ = sdk.g_pLTDatabase->GetRecordLink(
            sdk.g_pLTDatabase->GetAttribute(hWpnData, "AmmoName"), 0, 0);
        unsigned ammoCnt = ((unsigned(__thiscall *)(
            CArsenal *, HAMMO))sdk.CArsenal_GetAmmoCount)(pArsenal, hAmmoServ);
        if (!ammoCnt)
          p->state = 1;
        if (hAmmo && hAmmoServ != hAmmo)
          p->state = 1;
      }
      hWeapon =
          *(HWEAPON *)((unsigned char *)pArsenal + sdk.CArsenal_m_hCurWeapon);
      if (!hWeapon) {
        p->state = 1;
        break;
      }
      if (!hAmmo) {
        hWpnData = sdk.GetWeaponData(hWeapon);
        hAmmo = sdk.g_pLTDatabase->GetRecordLink(
            sdk.g_pLTDatabase->GetAttribute(hWpnData, "AmmoName"), 0, 0);
        HAMMODATA hAmmoData = sdk.GetAmmoData(hAmmo);
        ammoType = ((unsigned(__thiscall *)(
            CWeaponDB *, HAMMODATA, const char *, uintptr_t,
            uintptr_t))sdk.g_pWeaponDB_GetInt32)(sdk.g_pWeaponDB, hAmmoData,
                                                   "Type", 0, 0);
      }
      if ((hWeapon && StringUtil::hash_rt((char *)*(uintptr_t *)(hWeapon)) ==
                          StringUtil::hash_ct("Turret_Remote")) ||
          ammoType == TRIGGER)
        break;

      pMsgRead->ReadData((void *)&firePos, 0x60);
      if (firePos.x == 0.0 && firePos.y == 0.0 && firePos.z == 0.0)
        p->state = 1;
      if (_isnan(firePos.x) && _isnan(firePos.y) || _isnan(firePos.z))
        p->state = 1;
      pMsgRead->ReadData((void *)&vPath, 0x60);
      if (vPath.x == 0.0 && vPath.y == 0.0 && vPath.z == 0.0)
        p->state = 1;
      if (_isnan(vPath.x) && _isnan(vPath.y) || _isnan(vPath.z))
        p->state = 1;
      pMsgRead->ReadBits(8); // random number seed
      pMsgRead->ReadBits(8); // perturb count
      pMsgRead->ReadBits(8); // perturb
      unsigned fireTimestamp = pMsgRead->ReadBits(0x20);
      unsigned fireServTimestamp = sdk.getRealTimeMS();
      // printf("fire %p %p\n",fireTimestamp,fireServTimestamp);

      bool bFire = 1;
      // pSdk->g_pLTServer->GetObjectPos(hTarget,&targetPos);
//      sdk.g_pLTServer->GetObjectPos(hClientObj, &curPos);

      if (!curPos.NearlyEquals(firePos, dist)) {
        p->state = 1;
      }

      if (p->state) {
        if (hAmmo)
          ((unsigned(__thiscall *)(
              CArsenal *, HAMMO))sdk.CArsenal_DecrementAmmo)(pArsenal, hAmmo);
        break;
      }

      // if ((hWeapon && hash_rta((char *)*(uintptr_t *)(hWeapon)) ==
      //                    hash_ct("Minigun")))
      //  break;
      CWeapon *pWeapon = ((CWeapon * (__thiscall *)(CArsenal *))
                              sdk.CArsenal_GetCurWeapon)(pArsenal);
      unsigned ammoInClip;
      // bool isLastBullet=0;
      if ((bUnarmed || hWpnData) && pWeapon) {
        ammoInClip = pWeapon->m_nAmmoInClip;
        // if(ammoInClip == 1){
        //  *(bool volatile *)&isLastBullet=1;// bad, bad compiler!
        //}

        unsigned delta = fireServTimestamp - pPlData->lastFireWeaponIgnored;
        if (delta > 2048) { // rapid fire timer
          ((void(__thiscall *)(uintptr_t, uintptr_t, uintptr_t))p->hook)(
              p->tcx, p->v0, p->v1);
          EnterCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
          pPlData->lastFireWeaponIgnored = 0;
          LeaveCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));

        }
        p->state = 1;
        if (bUnarmed || (ammoInClip != pWeapon->m_nAmmoInClip)) {
          ammoInClip = pWeapon->m_nAmmoInClip;
          if (pPlData->lastFireWeapon != pWeapon) {
            EnterCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
            pPlData->lastFireWeaponClipAmmo = 0;
            pPlData->lastFireWeaponTimestamp = 0;
            pPlData->lastFireWeaponReload = 0;
            pPlData->lastFireWeaponAccuracyFailCnt = 0;
            pPlData->lastFireWeaponAccuracyFail = 0;
            pPlData->lastFireWeapon = pWeapon;
            LeaveCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
            unsigned dwAni = 8; // RELOAD
            // pSdk->g_pModelLT->GetAnimIndex(pWeapon->m_hModelObject,"Reload",dwAni);
            // pSdk->g_pModelLT->GetAnimLength(pWeapon->m_hModelObject, dwAni,
            //                                 pPlData->lastFireWeaponReloadLength);
            // pPlData->lastFireWeaponReloadLength -= 96;
            // if ((int)(pPlData->lastFireWeaponReloadLength) < 0)
            //   pPlData->lastFireWeaponReloadLength = 0;

            /*pPlData->lastFireWeaponClipMax = ((unsigned(__thiscall *)(
                CWeaponDB *, HWEAPONDATA, char *, uintptr_t,
                uintptr_t))pSdk->g_pWeaponDB_GetInt32)(
                pSdk->g_pWeaponDB, hWpnData, _C(WDB_WEAPON_nShotsPerClip), 0,
                0);
            pPlData->lastFireWeaponDelay = ((unsigned(__thiscall *)(
                CWeaponDB *, HWEAPONDATA, char *, uintptr_t,
                uintptr_t))pSdk->g_pWeaponDB_GetInt32)(
                pSdk->g_pWeaponDB, hWpnData, _C(WDB_WEAPON_nFireDelay), 0, 0);*/
          } else {
            /*if(pPlData->lastFireWeaponAccuracyFailCnt){
              unsigned fTimeDelta = fireTimestamp -
      pPlData->lastFireWeaponAccuracyFail; if(fTimeDelta >= 3000)
                pPlData->lastFireWeaponAccuracyFailCnt = 0;
            }
      if (hWeapon &&
          hash_rta((char *)*(uintptr_t *)(hWeapon)) == hash_ct("Assault Rifle"))
        if(pPlData->lastvPathFire[0])
        if(!pPlData->isAimed){
          if(pPlData->lastvPathFire.NearlyEquals(vPath,0.003f)){
            pPlData->lastFireWeaponAccuracyFail=fireTimestamp;
            pPlData->lastFireWeaponAccuracyFailCnt++;
            if(pPlData->lastFireWeaponAccuracyFailCnt==5){
                ((void(__thiscall *)(CPlayerObj *)) *
                 (uintptr_t *)((unsigned char *)*(uintptr_t *)pPlayerObj +
                               pSdk->CPlayerObj_DropCurrentWeapon))(pPlayerObj);
            p->state=1;
            break;
          }
          }
      }*/
            // pPlData->lastvPathFire = vPath;
            /*if(pPlData->lastFireWeaponReload){
              pPlData->lastFireWeaponReload = 0;
              unsigned delta = fireTimestamp - pPlData->lastFireWeaponTimestamp;
              if ( (delta < pPlData->lastFireWeaponReloadLength)) {
                //((unsigned(__thiscall *)(
                //    CArsenal *, HAMMO))pSdk->CArsenal_DecrementAmmo)(pArsenal,
                //    hAmmo);
                ((void(__thiscall *)(CPlayerObj *)) *
                 (uintptr_t *)((unsigned char *)*(uintptr_t *)pPlayerObj +
                               pSdk->CPlayerObj_DropCurrentWeapon))(pPlayerObj);
                //p->state = 1;
                pPlData->lastFireWeaponClipAmmo = 0;
                pPlData->lastFireWeaponTimestamp = 0;
                break;
              }
            }*/
            // if(pPlData->lastFireWeaponReload){
            //  pPlData->lastFireWeaponReload=0;
            // pPlData->lastFireWeaponDidReload=1;
            //}
            // if ((ammoInClip > pPlData->lastFireWeaponClipAmmo)) {
            // if(isLastBullet){
            /*if (ammoInClip == pPlData->lastFireWeaponClipMax) {
              pPlData->lastFireWeaponReload = 1;
            }*/
            unsigned fireServTimestampCheck = 0;
            // unsigned delay = pPlData->lastFireWeaponDelay + 0x800;
            // if (fireServTimestamp > (delay + 1))
            //  fireServTimestampCheck = fireServTimestamp - delay;

            if(fireTimestamp > (fireServTimestamp+0xFFFF)){
                EnterCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
                ((unsigned(__thiscall *)(CArsenal *, HAMMO))
                     sdk.CArsenal_DecrementAmmo)(pArsenal, hAmmo);
                pPlData->lastFireWeaponClipAmmo = 0;
                pPlData->lastFireWeaponTimestamp = 0;
                pPlData->lastFireWeaponIgnored = fireServTimestamp;
                // pPlData->invalidSpeed=true;
                LeaveCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
                p->state = 0;
                break;
              }


  //          if (fireTimestamp > fireServTimestamp ||
  //              fireTimestamp < (fireServTimestamp - 5000)) {
  //            ((unsigned(__thiscall *)(CArsenal *, HAMMO))
  //                 pSdk->CArsenal_DecrementAmmo)(pArsenal, hAmmo);
  //            pPlData->lastFireWeaponClipAmmo = 0;
  //            pPlData->lastFireWeaponTimestamp = 0;
  //            pPlData->lastFireWeaponIgnored = fireServTimestamp;
  //            break;
  //          }

            /*else if (fireTimestamp < fireServTimestampCheck) {
               //p->state = 1;
               ((void(__thiscall *)(CPlayerObj *)) *
                (uintptr_t *)((unsigned char *)*(uintptr_t *)pPlayerObj +
                              pSdk->CPlayerObj_DropCurrentWeapon))(pPlayerObj);
               pPlData->lastFireWeaponClipAmmo = 0;
               pPlData->lastFireWeaponTimestamp = 0;
               break;
             }*/
              DBGLOG("UNARM delta %d delay %d anim %s", (fireTimestamp - pPlData->lastFireWeaponTimestamp), unarmFireDelay, (char *)*(uintptr_t *)(hAmmo));
            if (bUnarmed && (fireTimestamp - pPlData->lastFireWeaponTimestamp) <
                                unarmFireDelay) { // original unarm FireDelay is 0
                                                  // so we add extra check
                DBGLOG("detected UNARM RAPID?")
              p->state = 0;
              EnterCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
              pPlData->lastFireWeaponClipAmmo = 0;
              pPlData->lastFireWeaponTimestamp = 0;
              pPlData->lastFireWeaponIgnored = fireServTimestamp;
              LeaveCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
              // pPlData->invalidSpeed=true;
              break;
            }
          }
          EnterCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
          pPlData->lastFireWeaponClipAmmo = ammoInClip;
          pPlData->lastFireWeaponTimestamp = fireTimestamp;
          LeaveCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
        }
      }

      if(inst.m_bRandWep && spawnLog){

          void * SpawnObject = sdk.m_spawnLogOffset;//inst.m_ServerModule+0x16B650;
          auto rot = LTRotation();
          rot.m_Quat[0] = 0.0;
          rot.m_Quat[1] = 0.0;
          rot.m_Quat[2] = 1.0;
          rot.m_Quat[3] = 1.0;

          auto np = curPos;
          np[1] -= 5.6;
          np[2] -= 64.0;
          ILTBaseClass * pObj = ((ILTBaseClass*(__cdecl *)(char *, const LTVector&, const LTRotation&))SpawnObject)((char *)"WeaponItem Gravity 0;MoveToFloor 0;AmmoAmount 10;WeaponType (a2x4);MPRespawn 0; DMTouchPickup 1;LifeTime 3000.00;Health 0;Placed 0", np, rot);
          if(pObj && pObj->m_hObject) {
              auto hObj = pObj->m_hObject;
              ILTCommon * common = sdk.g_pLTServer->Common();
              uint32_t nFlags = 0;
              bool bGravityOn = true;
//              common->GetObjectFlags(hObj, OFT_Flags, nFlags);
//              bGravityOn = bGravityOn && (nFlags & FLAG_SOLID);
              common->SetObjectFlags(hObj, OFT_Flags,
                                          0x10c30,
                                          0x10c30);
          }
      }

    }

    break;
    case MID_PLAYER_GEAR: {
      if (!pPlayerObj) {
        p->state = 1;
        break;
      }
      HGEAR hGear =
          pMsgRead->ReadDatabaseRecord(sdk.g_pLTDatabase, sdk.m_hCatGear);
      if (hGear && StringUtil::hash_rt((char *)*(uintptr_t *)(hGear)) != StringUtil::hash_ct("MedKit")) {
        p->state = 1;
        }
      break;
    } break;
    case MID_DROP_GRENADE: {
      if (pPlayerObj && (playerState == ePlayerState_Dying_Stage1 ||
                         playerState == ePlayerState_Dying_Stage2)) {
        pMsgRead->ReadDatabaseRecord(sdk.g_pLTDatabase, sdk.m_hCatWeapons);
        HAMMO hAmmo =
            pMsgRead->ReadDatabaseRecord(sdk.g_pLTDatabase, sdk.m_hCatAmmo);
        if (hAmmo && StringUtil::hash_rt((char *)*(uintptr_t *)(hAmmo)) == StringUtil::hash_ct("Frag")) {
          CArsenal *pArsenal = (CArsenal *)((unsigned char *)pPlayerObj +
                                            sdk.CPlayerObj_m_Arsenal);
          if (!pArsenal) {
            p->state = 1;
            break;
          }
          unsigned nAmmo = ((unsigned(__thiscall *)(
              CArsenal *, HAMMO))sdk.CArsenal_GetAmmoCount)(pArsenal, hAmmo);
          ((unsigned(__thiscall *)(
              CArsenal *, HAMMO, int))sdk.CArsenal_SetAmmo)(pArsenal, hAmmo, 0);
          if (!nAmmo) {
            p->state = 1;
            break;
          }
          {
            LTVector firePos, curPos;
            pMsgRead->ReadData((void *)&firePos, 0x60); // flash pos
            pMsgRead->ReadData((void *)&firePos, 0x60); // fire pos
            if (firePos.x == 0.0 && firePos.y == 0.0 && firePos.z == 0.0)
              p->state = 1;
            if (_isnan(firePos.x) && _isnan(firePos.y) || _isnan(firePos.z))
              p->state = 1;
            sdk.g_pLTServer->GetObjectPos(hClientObj, &curPos);
            if (!curPos.NearlyEquals(firePos, 128.0f)) {
              p->state = 1;
            }
            pMsgRead->ReadData((void *)&firePos, 0x60);
            if (firePos.x == 0.0 && firePos.y == 0.0 && firePos.z == 0.0)
              p->state = 1;
            if (_isnan(firePos.x) && _isnan(firePos.y) || _isnan(firePos.z))
              p->state = 1;
          }
        } else {
          p->state = 1;
        }
        /*unsigned nAmmoIndex = pSdk->g_pLTDatabase->GetRecordIndex(hAmmo);
        int * m_pAmmo = (int *)*(unsigned
        *)(pSdk->g_pArsenal+pSdk->CArsenal_m_pAmmo); if(!m_pAmmo[nAmmoIndex]){
          p->state=1;
        }else{
          m_pAmmo[nAmmoIndex]=0;
        }
        //CWeapon * cWep = pSdk->g_pArsenal->GetWeapon(hWep);*/
      } else {
        p->state = 1;
        break;
      }
    } break;
    /*case MID_WEAPON_SOUND_LOOP:
        if(pPlData->bResetToUnarmed){
      if(playerState==ePlayerState_Alive){
            CArsenal *pArsenal = (CArsenal *)((unsigned char *)pPlayerObj +
                                            pSdk->CPlayerObj_m_Arsenal);
          if(pArsenal){
            HWEAPON hWeapon =
            *(HWEAPON *)((unsigned char *)pArsenal +
    pSdk->CArsenal_m_hCurWeapon); if(!hWeapon){
                    ((void(__thiscall *)(CPlayerObj
    *,HWEAPON,bool,HAMMO,bool,bool))pSdk->CPlayerObj_ChangeWeapon)( pPlayerObj,
    pSdk->m_hUnarmedRecord, 1, pSdk->m_hUnarmedRecordAmmo, 0, 0);
                    pPlData->bResetToUnarmed=0;
            }

          }
      }

    }
    break;*/
    case MID_PLAYER_UPDATE: {
      if (!pPlayerObj) {
        p->state = 1;
        break;
      }
  //    unsigned timeMS = pSdk->getRealTimeMS();
      EnterCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
      pPlData->thisMoveTimeMS = pMsgRead->ReadBits(0x20);
      LeaveCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
  //    if (pPlData->thisMoveTimeMS >
  //        timeMS) // ignore messages that claim they from future
  //      p->state = 1;
      uint16_t nClientChangeFlags = pMsgRead->ReadBits(8);
      if (nClientChangeFlags & CLIENTUPDATE_CAMERAINFO) {
//        LTRotation rTrueCameraRot;
//        pMsgRead->ReadCompLTRotation(&rTrueCameraRot);
        pMsgRead->ReadCompLTRotation(&pPlData->camRot);
        bool bSeparateCameraFromBody = pMsgRead->ReadBits(1);
        if (bSeparateCameraFromBody) {
          pMsgRead->ReadBits(0x20);
        }
        LTVector vCameraPos;
        pMsgRead->ReadCompLTVector(&vCameraPos);
      }
      if (nClientChangeFlags & CLIENTUPDATE_ALLOWINPUT) {
        pMsgRead->ReadBits(1);
      }
      if (nClientChangeFlags & CLIENTUPDATE_ANIMATION) {
        bool bPlayerBody = pMsgRead->ReadBits(1);
        /*if(!bPlayerBody){
          if(playerState == ePlayerState_Alive)
            pSdk->g_pLTServer->KickClient((HCLIENT)p->v0);
          p->state=1;
        }*/
        unsigned nNumTrackers = pMsgRead->ReadBits(8);

        // if(numTrackers){
        for (uint8_t nTracker = 0; nTracker < nNumTrackers; ++nTracker) {
          unsigned trkId = pMsgRead->ReadBits(8);
          if (trkId != 0xFF) {                           // check is main tracker
            unsigned hWeight = pMsgRead->ReadBits(0x20); // hWeightSet
            if (hWeight > 4) {
              // DBGLOG("weight set %d", hWeight)
              sdk.BootWithReason(pGameClientData,
                                   eClientConnectionError_PunkBuster,
                                   (char *)"error 1");
              p->state = 1;
              break;
            }
          }
          bool bEnabled = pMsgRead->ReadBits(1); // tracker info
          if (bPlayerBody && bEnabled) {
            unsigned hAnim = pMsgRead->ReadBits(0x20); // hAnim
            if (!inst.bCustomSkins &&
                (hAnim >= (65 - sdk.isXP2) && hAnim <= (83 - sdk.isXP2)) || (hAnim >= (99 - sdk.isXP2) && hAnim <= (107 - sdk.isXP2)))
              p->state = 1;
            //DBGLOG("anim: %p\n",hAnim);
            EnterCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
            pPlData->hAnimPenult = pPlData->hAnim;
            pPlData->hAnim = hAnim;
            LeaveCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
            /*if(!hAnim){
              DBGLOG("hAnim is ZERO")
                pSdk->g_pLTServer->KickClient((HCLIENT)p->v0);
                p->state=1;
            }*/
            bool bSetTime = pMsgRead->ReadBits(1);
            if (bSetTime) {
              unsigned n = pMsgRead->ReadBits(0x20);
              if (n > 0xFFFF) {
                DBGLOG("%d > 0xFFFF", n)
                sdk.BootWithReason(pGameClientData,
                                     eClientConnectionError_PunkBuster,
                                     (char *)"error 2");
                p->state = 1;
                break;
              }
            }
            bool bLooping = pMsgRead->ReadBits(1);
            bool bSetRate = pMsgRead->ReadBits(1);
            if (bSetRate) {
              unsigned var = pMsgRead->ReadBits(0x20);
              float fRate = reinterpret_cast<float &>(var);
              // DBGLOG("fRate %llf", fRate);
              if (fRate <= 0.0f || fRate > 100.0f) {
                // pPlData->invalidSpeed = true;
                p->state = 1;
                break;
              } else{
                CArsenal *pArsenal = (CArsenal *)((unsigned char *)pPlayerObj +
                                                  sdk.CPlayerObj_m_Arsenal);
                if (pArsenal) {
                  HWEAPON hWeapon = *(HWEAPON *)((unsigned char *)pArsenal +
                                                 sdk.CArsenal_m_hCurWeapon);
                  HWEAPONDATA hWpnData = sdk.GetWeaponData(hWeapon);
                  uint32_t nValueIndex = 0;
                  float fDef = 0.0f;
                  float fMovementMultiplier = sdk.g_pLTDatabase->GetFloat(
                      sdk.g_pLTDatabase->GetAttribute(
                          hWpnData, WDB_WEAPON_fMovementMultiplier),
                      nValueIndex, fDef);
                  float fMoveMultiplier =
                      *(float *)((unsigned char *)pPlayerObj +
                                 sdk.CPlayerObj_m_fMoveMultiplier);
                  auto f = fMovementMultiplier * fMoveMultiplier;
                  // DBGLOG("whatever speed? %llf", f);
                  EnterCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
                  if (pPlData->speedPrev <= 0) {
                    pPlData->speedPrev = f;
                  }
                  pPlData->speedPrev = fRate > pPlData->speedPrev ? pPlData->speedPrev : fRate;
                  if ((pPlData->speedPrev > f && (pPlData->invalidSpeedCnt++)==16) || (fRate > fMoveMultiplier+0.25f)) {
                        DBGLOG("invalid speed detected! f=%llf frate=%llf", f, fRate);
                        pPlData->invalidSpeed = true;
                  } else {pPlData->invalidSpeedCnt = 0;}
                  LeaveCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
                }
              }
            }
          }
        }
      }
      if(pPlData->invalidSpeed)
        p->state = 1;
      
    } break;
    case MID_CLIENTCONNECTION:
      switch (pMsgRead->ReadBits(8)) {
      /*case eClientConnectionState_LoggedIn:
      {
        GameClientData* pGameClientData =
      pSdk->getGameClientData((HCLIENT)p->v0);
        *(unsigned char*)(pGameClientData+0x74)=0xFF;
      }
      break;*/
      case eClientConnectionState_InWorld: {
        wchar_t playerName[16];
        unsigned len = pMsgRead->ReadWString(playerName, sizeof(playerName) /
                                                             sizeof(wchar_t)) *
                       sizeof(wchar_t);
        int isUni = IS_TEXT_UNICODE_ILLEGAL_CHARS;
        IsTextUnicode((void *)playerName, len, &isUni);
        if (isUni & IS_TEXT_UNICODE_ILLEGAL_CHARS) {
          // GameClientData* pGameClientData =
          // pSdk->getGameClientData((HCLIENT)p->v0);
          // DBGLOG("MID_CLIENTCONNECTION fail")
          sdk.BootWithReason(pGameClientData, eClientConnectionError_PunkBuster,
                               (char *)"Invalid player name");
          p->state = 1;
        }
      } break;
      case eClientConnectionState_KeyChallenge: {
        // GameClientData* pGameClientData =
        // pSdk->getGameClientData((HCLIENT)p->v0);
        char szChallengeResponse[256] = "";
        pMsgRead->ReadBits(0x20);
        pMsgRead->ReadBits(0x20);
        pMsgRead->ReadBits(0x20);
        unsigned char ret = 1;
        if (pMsgRead->ReadString(szChallengeResponse,
                                 sizeof(szChallengeResponse)) == 8) {
          char *pStr = szChallengeResponse;
          {
            unsigned i = 0;
            unsigned char c = pStr[i];
            while (c) {
              if (!(c >= 'a' && c <= 'z')) {
                ret = 0;
                break;
              }
              i++;
              c = pStr[i];
            }
          }
        } else
          ret = 0;
        if (ret && pMsgRead->ReadString(szChallengeResponse,
                                        sizeof(szChallengeResponse)) == 72) {
          char *pStr = szChallengeResponse;
          {
            unsigned i = 0;
            unsigned char c = pStr[i];
            while (c) {
              if (!((c >= 'a' && c <= 'f') || (c >= '0' && c <= '9'))) {
                ret = 0;
                break;
              }
              i++;
              c = pStr[i];
            }
          }
        } else
          ret = 0;

        {
          uint8_t aTcpIp[4];
          uint16_t nPort;
          sdk.getClientAddr((HCLIENT)p->v0, aTcpIp, &nPort);
          EnterCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_ipchunkSection.get()));
          sdk.m_ipData.erase(*(uint32_t *)aTcpIp);
          LeaveCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_ipchunkSection.get()));
//          IPData block = {*(uint32_t *)aTcpIp, nPort};
//          EnterCriticalSection(&pSdk->g_ipchunkSection);
//          IPChunk::foreach (
//              pSdk->g_ipchunk, &block,
//              [](uintptr_t index, IPChunk *Chunk, IPData *Block) -> uintptr_t {
//                if (Chunk->buf[index].ip == Block->ip) {
//                  Chunk->buf[index].ip = 0;
//                  return 1;
//                }
//                return 0;
//              });
//          LeaveCriticalSection(&pSdk->g_ipchunkSection);
        }
        if (!ret) {
          p->state = 1;
          sdk.BootWithReason(pGameClientData, eClientConnectionError_BadCDKey,
                               (char *)0);
        }
      } break;
      }
      break;
    case MID_PLAYER_INFOCHANGE: {
      wchar_t playerName[16];
      unsigned len = pMsgRead->ReadWString(playerName,
                                           sizeof(playerName) / sizeof(wchar_t)) *
                     sizeof(wchar_t);
      int isUni = IS_TEXT_UNICODE_ILLEGAL_CHARS;
      IsTextUnicode((void *)playerName, len, &isUni);
      if (isUni & IS_TEXT_UNICODE_ILLEGAL_CHARS) {
        // GameClientData* pGameClientData =
        // pSdk->getGameClientData((HCLIENT)p->v0);
        sdk.BootWithReason(pGameClientData, eClientConnectionError_PunkBuster,
                             (char *)"Invalid player name");
        p->state = 1;
      }
      unsigned skinIndex = pMsgRead->ReadBits(8);
      unsigned skinCount = 0;
      if (*(char *)(((GameModeMgr * (*)()) sdk.g_pGameModeMgr_instance)() +
                    0x18C) /*m_grbUseTeams*/) {
        skinCount =
            sdk.g_pLTDatabase->GetNumValues(sdk.g_pLTDatabase->GetAttribute(
                sdk.m_hGlobalRecord, "TeamModel"));
      } else {
        skinCount =
            sdk.g_pLTDatabase->GetNumValues(sdk.g_pLTDatabase->GetAttribute(
                sdk.m_hGlobalRecord, "DeathmatchModels"));
      }
      if (skinIndex >= skinCount) {
        p->state = 1;
        break;
      }
      unsigned nTeam = pMsgRead->ReadBits(8);
      if (nTeam >= 2 && nTeam != 0xFF)
        p->state = 1;
    } break;
    case MID_VOTE: {
      unsigned voteTime = sdk.getRealTimeMS();
      if (pMsgRead->ReadBits(3) == eVote_Start) {
        EnterCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
        p->state = sdk.checkPlayerStatus(pGameClientData);
        if (pPlData->lastVoteTime) {

          // printf("value_test %d", value_test);
          unsigned voteDelay = 3000;
          // if(voteDelay)
          // voteDelay-=250; //server timer may lag with client
          unsigned delta = voteTime - pPlData->lastVoteTime;
          if (delta < voteDelay) {
            p->state = 1;
            LeaveCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
            break;
          }
        }
        pPlData->lastVoteTime = voteTime;
        LeaveCriticalSection(static_cast<CRITICAL_SECTION *>(sdk.g_pldataSection.get()));
        unsigned char type = pMsgRead->ReadBits(3);
        if (type >= kNumVoteTypes) { 
            p->state = 1;
            break;
        } else {
            ServerSettings *pServSettings = (ServerSettings *)((
                unsigned char *)(((GameModeMgr * (*)())
                                      sdk.g_pGameModeMgr_instance)() +
                                 sdk.GameModeMgr_ServerSettings));
            if (pServSettings->m_bAllowVote[type] != 1) {
                p->state = 1;
                break;
            }
        }
        if (type < kNumVoteTypes && type == eVote_SelectMap) {
          unsigned nMapIndex = pMsgRead->ReadBits(0x20);
          CServerMissionMgr *g_pServerMissionMgr = *sdk.g_pServerMissionMgrAdr;
          unsigned nMaps = ((uintptr_t)g_pServerMissionMgr->m_CampaignEnd -
                            (uintptr_t)g_pServerMissionMgr->m_CampaignBegin) /
                           sizeof(uintptr_t);
          if (nMapIndex >= nMaps)
            p->state = 1;
        } else if (type == eVote_TeamKick) {
          unsigned nTargetId = pMsgRead->ReadBits(0x20);
          HCLIENT hTargetClient = sdk.g_pLTServer->GetClientHandle(nTargetId);
          GameClientData *pGameTargetData =
              sdk.getGameClientData(hTargetClient);
          if (!pGameTargetData) {
            p->state = 1;
            break;
          }
          if (*(unsigned char *)(pGameClientData + 0x74) !=
              *(unsigned char *)(pGameTargetData + 0x74)) // team
            p->state = 1;
        }
      }
      
    } break;
    case MID_PLAYER_EVENT: {
      unsigned char type = pMsgRead->ReadBits(8);
      if (type == sdk.ukPENextSpawnPoint || type == sdk.ukPEPrevSpawnPoint)
        p->state = 1;
    } break;
    case MID_PLAYER_CLIENTMSG: {
      unsigned char CP = pMsgRead->ReadBits(8);
      switch (CP) {
      case CP_DAMAGE:
      case CP_DAMAGE_VEHICLE_IMPACT:
        if (CP == CP_DAMAGE &&
            /*dmgId*/ pMsgRead->ReadBits(8) != sdk.m_uDT_CRUSH)
          p->state = 1;
        if (/*fDmg*/ pMsgRead->ReadBits(0x20) == -1)
          p->state = 1;
        if (pMsgRead->ReadBits(8)) { // is use timer
          pMsgRead->ReadBits(0x20);  // fDuration
        }
        if (CP == CP_DAMAGE && hClientObj != pMsgRead->ReadObject() /*hObject*/)
          p->state = 1;
        break;
      case CP_FLASHLIGHT:
        if (!inst.m_bCoop)
          p->state = 1;
        break;
      case CP_WEAPON_STATUS:
        unsigned char WS = pMsgRead->ReadBits(8);
        switch (WS) {
        case WS_RELOADING:
          // pPlData->lastFireWeaponReload++;
          break;
        }
        break;
      }
    }
    }
    pMsg->f12 = v1;
    pMsg->f16 = (void *)v2;
    pMsg->f20 = v3;
    pMsg->f24 = v4;
}

void AppHandler::configHandle()
{
    SetPriorityClass((HANDLE)-1, HIGH_PRIORITY_CLASS);
    uint8_t moveax0[] = {0xB8, 0x00, 0x00, 0x00, 0x00};
    SdkHandler &hsdk = *sdkHandler();
    SpliceHandler &hsplice = *spliceHandler();
    PatchHandler &hpatch = *patchHandler();
    hsdk.initServer();
    patchClientServer(m_eServer, m_eServerSz);
    {
        static auto pat = BSF("B8????????E8????????A1????????85C0");
        hsdk.m_spawnLogOffset = scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
    }
    {
        static auto pat = BSF("8D4C24??51505756FF15????????83C410C6????????5F");
        unsigned char *tmp =
            scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
            unsigned char *pfunc = (unsigned char *)*(uintptr_t *)(tmp + 10);
            hpatch.addCode(tmp + 8, 6);
            *(unsigned short *)(tmp + 8) = 0xE890;
            *(uintptr_t *)(tmp + 10) = getRel4FromVal(
                (tmp + 10), (unsigned char *)(void *)*(uintptr_t *)pfunc);
            hpatch.addCode(pfunc, 4);
            void *func = (void *)GetProcAddress(GetModuleHandle(_T("ntdll.dll")),
                                                _C("_vsnprintf"));
            if (func)
                *(uintptr_t *)(pfunc) = (uintptr_t)func;
        }
    }
    {
        static auto pat = BSF("75??8B4E086A006A0351E8");
        unsigned char *tmp =
            scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
            hpatch.addCode(tmp, 1); // allow connect with invalid assets
            *(tmp) = 0xEB;
        }
    }
    // {
    //     unsigned char *tmp =
    //         scanBytes((unsigned char *)m_Server, m_ServerSz,
    //                   BYTES_SEARCH_FORMAT(
    //                       "FF5204E8????????8A??????????05")); // disable punkbuster
    //     if (tmp) {
    //         hpatch.addCode(tmp, 4);
    //         *(unsigned *)(tmp) = 0xE8909090;
    //     }
    // }
    {
        static auto pat = BSF("745D8B????????????8B????8B??C1????884C2414");
        unsigned char *tmp = scanBytes(
            (unsigned char *)m_eServer, m_eServerSz, reinterpret_cast<uint8_t *>(&pat)); // show conn client
            // ip
        if (tmp) {
            hsplice.spliceUp(tmp - 7, (void *)SdkHandler::hookUDPConnReq);
            hpatch.addCode(tmp, 2);
            *(unsigned short *)(tmp) = 0x9090;
        }
    }
    {
        static auto pat = BSF("74608B??????????8B??C1????88??????33");
        unsigned char *tmp = scanBytes(
            (unsigned char *)m_eServer, m_eServerSz, reinterpret_cast<uint8_t *>(&pat)); // show disco client ip
        if (tmp) {
            hpatch.addCode(tmp, 2);
            *(unsigned short *)(tmp) = 0x9090;
        }
    }
    {
        static auto pat = BSF("8B5E0C81FB80000000732E8B06578B7C2410");
        unsigned char *tmp = scanBytes(
            (unsigned char *)m_eServer, m_eServerSz, reinterpret_cast<uint8_t *>(&pat)); // FindObjects spam
        if (tmp) {
            hpatch.addCode(tmp + 9, 2);
            *(unsigned short *)(tmp + 9) = 0x9090;
        }
    }
    {
        static auto pat = BSF("750980E1??888E????????B001");
        unsigned char *tmp =
            scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat)); // combo
            // death
            // fix
        if (tmp) {
            hpatch.addCode(tmp, 1);
            *(tmp) = 0xEB;
            //hpatch.codeswap((unsigned char *)tmp, d, 1);
        }
    }
    /*{
    unsigned char *tmp = scanBytes(
        (unsigned char *)gServer, gServerSz,
        (char *)"85C00F84????????8B??????????8D??????????FF5208"); //animation
  body 0 disable if (tmp) { patchData::codeswap((unsigned char *)tmp+2,
                          (unsigned char *)(const unsigned
  char[]){0x90,0x90,0x90,0x90,0x90,0x90}, 6);
    }
  }*/

    {
        static auto pat = BSF("8A42??84C076??33C90FB6D0");
        unsigned char *tmp =
            scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
            hpatch.addCode((unsigned char *)tmp + 5, 1);
            *(tmp + 5) =
                0xEB; // disable cleaning of ammo quantity (for MID_DROP_GRENADE)
        }
    }
    {
        static auto pat = BSF("83C404F6????74??8B??E8????????6A00E8????????8B??E8");
        unsigned char *tmp =
            scanBytes((unsigned char *)m_eServer, m_eServerSz, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
            hsplice.spliceUp(tmp, (void *)SdkHandler::hookOnMapLoaded);
        }
    }
    {
        static auto pat = BSF("7402893783FEFF7472");
        unsigned char *tmp = scanBytes((unsigned char *)m_eServer, m_eServerSz, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
            hsplice.spliceUp(tmp, (void *)SdkHandler::hookUDPRecvfrom);
        }
    }
    {
        static auto pat = BSF("8B168BCEFF1284C0740E6A016A03568BCF");
        unsigned char *tmp =
            scanBytes((unsigned char *)m_eServer, m_eServerSz, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
            hsplice.spliceUp(tmp, (void *)SdkHandler::hookCheckUDPDisconnect);
        }
    }
    {
        static auto pat = BSF("74??8B0D????????8D5424??C74424??00000000");
        unsigned char *tmp =
            scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
            hpatch.addCode((unsigned char *)tmp,
                               1); // ignore invalid world crc
            *(tmp) = 0xEB;
        }
    }
    // {
    //     static auto pat = BSF("3B7C2410741F8B4E086A006A0351");
    //     unsigned char *tmp =
    //         scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
    //     if (tmp) {
    //         hpatch.addCode((unsigned char *)tmp + 4,
    //                            1); // ignore invalid world crc
    //         *(tmp + 4) = 0xEB;
    //     }
    // }
    if (m_bRandWep) {
//        hsplice.spliceUp(
//            scanBytes((unsigned char *)m_eServer, m_eServerSz,
//                      BYTES_SEARCH_FORMAT("0F84840000008B442420"))+6,
//            (void *)hookLoadObject);
        {
            static auto pat = BSF("6A005256FF90??0000008BF085F60F84????????8B");
            unsigned char *tmp = scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                hsplice.spliceUp(tmp, (void *)hookRandomWeapon);
            }
        }
        {
            static auto pat = BSF("8B0D????????8B168BF88B41??50A1????????508BCEFF52??8B168BD86A20");
            unsigned char *tmp =
                scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                hsplice.spliceUp(tmp, (void *)hookPickupRndWeapon);
            }
        }
        // {
        //   unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
        //       (unsigned char *)gServer, gServerSz,
        //       BYTES_SEARCH_FORMAT("740768????????EB0568????????E8????????89")));
        //   if (tmp) {
        //     tmp += 3;
        //     patchData::addCode(tmp, 4);
        //     *(const char **)tmp = "FastForward";
        //   }
        // }

        {
            static auto pat = BSF("83EC10568B74241885F6578BF9");
            unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat)));
            if (tmp) {
                hsplice.spliceUp(tmp, (void *)hookEnterSlowMo);
            }
        }
        /*{
  unsigned char *tmp =
      scanBytes((unsigned char *)gServer, gServerSz,
                (char *)"FF24??????????56558BCBE8????????EB");
  if (tmp) {
    patchData::addCode((unsigned char *)tmp+7, 2);
    *(unsigned short *)(tmp+7) = 0x07EB;
  }
}*/
    }

    if (m_bCoop) {

        m_preventNoclip = 2;
        // bIgnoreUnusedMsgID=0;
        m_bSyncObjects = 1;
        m_bBotsMP = 1;
        {
            static auto pat = BSF("753F8D??????E8????????8B??????????8B");
            unsigned char *tmp = scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                hpatch.addCode((unsigned char *)tmp, 1);
                *(tmp) = 0xEB; // dont crush players on player
                // memcpy(tmp,moveax0,5);
            }
        }
        {
            static auto pat = BSF("8B??????85????8B??74??8B??????????85??74??3B??74??68");
            unsigned char *tmp = scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat)); // story
                // mode
                // on
            if (tmp) {
                hsplice.spliceUp(tmp, (void *)hookStoryModeOn);
            }
        }
        {
            static auto pat = BSF("8B??8B??????????85??75??68????????6A00E8????????");
            unsigned char *tmp =
                scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                hsplice.spliceUp(tmp, (void *)hookStoryModeOff);
            }
        }
        {
            static auto pat = BSF("E8????????84C074??8D??????68??????????E8");
            unsigned char *tmp = scanBytes((unsigned char *)m_Server,
                                           m_ServerSz, reinterpret_cast<uint8_t *>(&pat));  // use SP MAPS
            if (tmp) {
                unprotectCode(tmp);
                memcpy(tmp, moveax0, 5);
            }
        }
        if (m_bCoop == 1 && m_bAdditionalContent == 0) {
            {
                static auto pat = BSF("6A00????FF??????????8B??85??74??E8????????84??74");
                hsplice.spliceUp(
                    scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat)),
                    (void *)SdkHandler::hookLoadMaps1);
        }
        {
            static auto pat = BSF("5357??FF??????????3B??8B??0F??????????8B??????????8B??68??????????FF????8B????????????FF");
            hsplice.spliceUp(scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat)),
                     (void *)SdkHandler::hookLoadMaps2);
        }

            /*{
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"75??8B????8B????8B??FF??????????8B????8B");
      if (tmp) {
        spliceUp(tmp, (void *)fearData::hookUseSkin1);
      }
    }*/
        } else if (m_bAdditionalContent == 1) {
            static auto pat = BSF("88861C0200008B44243C888E1D020000");
            hsplice.spliceUp(
                scanBytes((unsigned char *)m_eServer, m_eServerSz, reinterpret_cast<uint8_t *>(&pat)),
                (void *)hookComposeArchives);
        }
        {
            static auto pat = BSF("8B??????????F6????74????8D??????????8B");
            unsigned char *tmp = scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                unprotectCode(tmp);
                *(tmp + 9) = 0xEB;
            }
        }
        {
            static auto pat = BSF("83E0FB68????????8B??89");
            unsigned char *tmp =
                scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                unprotectCode(tmp);
                *(unsigned short *)(tmp + 1) = 0x0BC8;
            }
        }
        {
            static auto pat = BSF("8B??????83????3B????75??8B??E8??????????????83????C3");
            unsigned char *tmp =
                scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat)); // cutscene
                // player
                // numbers
            if (tmp) {
                unprotectCode(tmp);
                *(unsigned short *)(tmp + 10) = 0x9090;
            }
        }
        {
            static auto pat = BSF("8A5C2408F6DB565768000100008BF9");
            unsigned char *tmp = scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat)); // CANACTIVATE always 1
            if (tmp) {
                hpatch.addCode((unsigned char *)tmp, 4);
                *(unsigned *)(tmp) = 0x909001B3;
            }
        }

        {
            static auto pat = BSF("D9????EB??D9??????8B??????????D9??????????89??????68????????8B??E8????????85??74??83??0375??D9??24EB??D9????0C");
            unsigned char *tmp = scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                // unprotectCode(tmp);

                //*(unsigned short *)(tmp+1)=0x90E8;
                hsplice.spliceUp(tmp - 14, (void *)hookDoorTimer);
            }
        }

        {
            static auto pat = BSF("EB??D9????0C8B??????????D9??????????68????????8B??89??????E8????????85??74??83????75??D9????EB??D9??????68????????D9??????????8B");
            unsigned char *tmp =
                scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                hsplice.spliceUp(tmp + 29, (void *)hookDoorTimer);
            }
        }
        {
            static auto pat = BSF("D9??24EB??D9????0C8B??????????D9??????????68");
            unsigned char *tmp = m_Server;

            unsigned i = 0;
            while (true) {
                tmp = scanBytes((unsigned char *)tmp, (m_ServerSz + m_Server) - tmp, reinterpret_cast<uint8_t *>(&pat));
                if (tmp) {
                    hsplice.spliceUp(tmp - 14, (void *)hookDoorTimer);
                }
                tmp++;
                i++;
                if (i == 2)
                    break;
            }
        }
        {
            static auto pat = BSF("508B??FF??????????8B??E8????????88??????????8A");
            unsigned char *tmp = m_Server;

            unsigned i = 0;
            while (true) {
                tmp = scanBytes((unsigned char *)tmp, (m_ServerSz + m_Server) - tmp, reinterpret_cast<uint8_t *>(&pat));
                if (tmp) {
                    hsplice.spliceUp(tmp, (void *)SdkHandler::hookUseSkin1);
                }
                tmp++;
                i++;
                if (i == 2)
                    break;
            }
        }
        {
            static auto pat = BSF("75??8B??????????68????????E8??????????8B??FF");
            unsigned char *tmp = scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                unprotectCode(tmp);
                *(unsigned short *)(tmp) = 0x9090;
                m_skinStr = (char **)(tmp + 9);
                *(uintptr_t *)(tmp + 9) = (uintptr_t) "Player";
            }
        }
        {
            static auto pat = BSF("75??E8????????84??74??8B??????????8B??6A006A0068??????????FF");
            unsigned char *tmp = scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                tmp += 2;
                unprotectCode(tmp);
                memcpy(tmp, moveax0, 5);
            }
        }
        {
            static auto pat = BSF("75??E8????????84??74??8B??????????8B??6A00");
            unsigned char *tmp = scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                tmp += 2;
                unprotectCode(tmp);
                memcpy(tmp, moveax0, 5);
            }
        }

        {
            static auto pat = BSF("E8????????84C074??8A??????????84C075??83??????75??8B");
            unsigned char *tmp = scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                unprotectCode(tmp);
                memcpy(tmp, moveax0, 5);
            }
        }
        {
            static auto pat = BSF("74??0F????8B??????????89??????????EB??A1????????C7??????????010000008B");
            unsigned char *tmp =
                scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                unprotectCode(tmp);
                uint8_t d[] = {0xB8, 0x03, 0x00, 0x00,0x00};
                memcpy(tmp,d,
                       5);
            }
        }

        {
            static auto pat = BSF("8B??85??????????????????C7??????????????C7??????????????74");
            unsigned char *tmp = scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                hsplice.spliceUp(tmp, (void *)SdkHandler::hookRespawnStartPoint);
            }
        }
        {
            static auto pat = BSF("A1????????8B??????????8B????????????8B????????????E8????????C3");
            unsigned char *tmp =
                scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                hsplice.spliceUp(tmp, (void *)SdkHandler::hookCheckpointEvent);
            }
        }
        {
            static auto pat = BSF("8A8424A000000084C074");
            unsigned char *tmp = m_Server;
            tmp = scanBytes((unsigned char *)tmp, (m_ServerSz + m_Server) - tmp,
                            reinterpret_cast<uint8_t *>(&pat));
            tmp += 9;
            unprotectCode(tmp);
            *(unsigned short *)(tmp) = 0x9090;
            tmp = scanBytes((unsigned char *)tmp, (m_ServerSz + m_Server) - tmp,
                            reinterpret_cast<uint8_t *>(&pat));
            tmp += 9;
            unprotectCode(tmp);
            *(unsigned short *)(tmp) = 0x9090;
        }
        {
            static auto pat = BSF("56E8????????8B??????????E8????????8B??????????8B");
            unsigned char *tmp =
                scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                unprotectCode(tmp);
                *(uintptr_t *)(tmp) = 0x90c301b0;
            }
        }
        {
            static auto pat = BSF("8B????????????8D????????????83C4183B??74");
            unsigned char *tmp = scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                hsplice.spliceUp(tmp + 7, (void *)hookGameMode);
            }
        }
        {
            static auto pat = BSF("E8????????84C074??8B0D????????68????????E8????????D9");
            unsigned char *tmp = scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                unprotectCode(tmp);
                memcpy(tmp, moveax0, 5);
            }
        }
        {
            static auto pat = BSF("E8????????84C08B??????????74??68????????EB??68");
            unsigned char *tmp =
                scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                unprotectCode(tmp);
                memcpy(tmp, moveax0, 5);
            }
        }
        {
            static auto pat = BSF("E8????????84C075??8B??????????8B0D??????????6A01E8");
            unsigned char *tmp =
                scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                unprotectCode(tmp);
                memcpy(tmp, moveax0, 5);
            }
        }
        {
            static auto pat = BSF("E8????????84C08B??????????74??68????????EB??68????????E8????????83EC08");
            unsigned char *tmp =
                scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                unprotectCode(tmp);
                memcpy(tmp, moveax0, 5);
            }
        }
        {
            static auto pat = BSF("E8????????84C074??8B??????????6A????6A00E8");
            unsigned char *tmp = scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                unprotectCode(tmp);
                m_SPModeSpawn = tmp;
                setCoopDoSpawn(1);
                // memcpy(tmp,moveax0,5);
            }
        }
        {
            static auto pat = BSF("75??8D??????E8????????8B????8B????8B????89");
            unsigned char *tmp = scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                m_patchHoleKillHives = tmp - 5;
                hsplice.spliceUp(m_patchHoleKillHives, (void *)SdkHandler::hookPatchHoleKillHives);
            }
        }
    }
    if (m_bBotsMP) {
        {
            static auto pat = BSF("E8????????84C074??8B??8B??FF??????????84??74??E8????????8A");
            unsigned char *tmp = scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                unprotectCode(tmp);
                memcpy(tmp, moveax0, 5);
            }
        }
        {
            static auto pat = BSF("E8????????84??0F??????????E8????????8A??????????05????????84??74??8B");
            unsigned char *tmp =
                scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                unprotectCode(tmp);
                memcpy(tmp, moveax0, 5);
            }
        }
    }
    {
        static auto pat = BSF("84C07408??8B??E8????????????C20800");
        unsigned char *tmp =
            scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
            // unprotectCode(tmp);
            hpatch.addCode(tmp + 8, 4);
            *(unsigned *)(tmp + 8) = getRel4FromVal(
                (tmp + 8),
                (unsigned char *)(void *)&CPlayerObj::handlePlayerPositionMessage);
        }
    }
    if (m_preventNoclip) {
        {
            if (!m_bCoop && hsdk.CPlayerObj_UpdateMovement) {
                unsigned char *tmp = (unsigned char *)hsdk.CPlayerObj_UpdateMovement;
                // unprotectCode(tmp);
                hpatch.addCode(tmp, 5);
                *(unsigned *)(tmp + 1) = getRel4FromVal(
                    (tmp + 1), (unsigned char *)(void *)&CPlayerObj::updateMovement);
                *(unsigned char *)(tmp) = 0xE9;
            }
        }

        if (m_preventNoclip == 2) {
            static auto pat = BSF("8BCFFF90????????3BC3A3????????7410C705????????????????893D????????391D????????75688B0D????????891D????????891D????????891D????????8B1168????????8BF9FF92????????3BC3A3????????75??8B0753");
            unsigned char *tmp = scanBytes(
                (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                tmp += 82;
                uint8_t d[] = {0xB9, 0x00, 0x00, 0x80, 0x3F, 0x90, 0x90, 0x8B, 0x07, 0x51};
                hpatch.addCode(tmp, 10);
                memcpy(tmp, reinterpret_cast<uint8_t*>(&d), 10);
            }
        } else {
            hsdk.m_bfreeMovement = 1;
        }
    }
    //*(uintptr_t *)tmp = 0x900008C2;
    unsigned char *StringToDamageType =
        scanBytes((unsigned char *)m_Server, m_ServerSz,
                  BYTES_SEARCH_FORMAT("538B????????????8B????????33FF??8B"));
    if (StringToDamageType) {
        hsdk.m_uDT_CRUSH =
            ((unsigned(__cdecl *)(char *))StringToDamageType)((char *)"CRUSH");
        if (hsdk.m_uDT_CRUSH != 4)
            hsdk.isXP2 = 1;
    }
    // spliceUp(scanBytes((unsigned char *)gServer, gServerSz,(char
    // *)"83????3D????????0F??????????0F"),(void *)hookMID_);
    {
        static auto pat = BSF("53568B74????85F68BD90F??????????578B??????56");
        hsplice.spliceUp(scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat)),
                         (void *)hookMID);
    }
    {
        static auto pat = BSF("508B??E8????????84C074??8D??????50");
        unsigned char *tmp =
            scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
        hsdk.g_pGetNetClientData = getVal4FromRel(tmp + 4);
        hsplice.spliceUp(tmp, (void *)hookOnAddClient_CheckNickname);
    }

    if (m_bSyncObjects) {
        /*{
  unsigned char *tmp = scanBytes(
      (unsigned char *)gServer, gServerSz,
      (char *)"8338??75??8A48??84C974??8B5608");  // MPClientOnlyRigidBody
  if (tmp) {
    patchData::addCode(tmp + 10, 1);
    *(unsigned char *)(tmp + 10) = 0xEB;
  }
}*/

        // spliceUp(scanBytes((unsigned char *)gServer, gServerSz,(char
        // *)"51535556578B3933DB4F894C2410"),(void *)hookReadProps);
        /*{
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          (char *)"E8????????84C074??8B??????????85C074??33??8A??????????6A00");
    //check in coop if (tmp) { patchData::addCode(tmp, 5); memcpy(tmp, moveax0,
    5);
      }
    }*/
        /*{
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    (char
    *)"E8????????84C074??8B????50E8????????83C40484C074");//FriendlyFire,
    CPlayerObj::ProcessDamageMsg if (tmp) { patchData::addCode(tmp, 5);
        memcpy(tmp, moveax0, 5);
      }
    }*/
        // custom sync
        // if (bCoop) {
        {
            static auto pat = BSF("E8????????84C074??F6??????74??68????????8B??E8????????85C074");
            unsigned char *tmp =
                scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                unprotectCode(tmp);
                memcpy(tmp, moveax0, 5);
            }
        }
        // } else
        {
          static auto pat = BSF("834E08048D????85??8D??????????74");
          unsigned char *tmp =
              scanBytes((unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat));
          hsdk.ObjectCreateStruct_m_Name = *(unsigned *)(tmp + 11);
          hsplice.spliceUp(tmp + 9, (void *)hookSetObjFlags);
        }
        // custom sync end
        /*{
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          (char
               *)"8B560883E2BF81CA80000000895608");  // WorldModel::PostReadProp
      if (tmp) {
        patchData::addCode(tmp,12);
        memset(tmp,0x90,12);
        spliceUp(tmp , (void *)hookSetObjFlags);
        //patchData::codeswap((unsigned char *)tmp,
        //                    (unsigned char *)(const unsigned char[]){
        //                        0xC7, 0x46, 0x04, 0x71, 0x00, 0x00, 0x00,
    0xC7,
        //                        0x46, 0x08, 0x40, 0x00, 0x00, 0x00, 0x90},
        //                    15);
      }
    }*/

        if (m_bCoop) {
            /*{
            unsigned char * tmp=scanBytes((unsigned char *)gServer, gServerSz,
                              (char
         *)"E8????????84C074??F6??????74??68????????8B??E8????????85C074");
         //WorldModel::PostReadProp if (tmp) { patchData::addCode(tmp, 5);
              memcpy(tmp, moveax0, 5);
            }
          }
          {
            unsigned char *tmp = scanBytes(
                (unsigned char *)gServer, gServerSz,
                (char *)"E8????????84??0F??????????8B??????????8D????????8B");
         //WorldModel::OnObjectCreated if (tmp) { patchData::addCode(tmp, 5);
              memcpy(tmp, moveax0, 5);
            }
          }*/

            {
                static auto pat = BSF("E8????????88??????8D??????????8B??8B??8B????8B????89");
                unsigned char *tmp = scanBytes(
                    (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat)); // ragdoll time Multi/SP
                if (tmp) {
                    hpatch.addCode(tmp, 5);
                    memcpy(tmp, moveax0, 5);
                }
            }
            {
                static auto pat = BSF("E8????????8B????8A??88??????8B????????????E8");
                unsigned char *tmp = scanBytes(
                    (unsigned char *)m_Server, m_ServerSz, reinterpret_cast<uint8_t *>(&pat)); // ragdoll
                    // type
                    // Multi/SP
                if (tmp) {
                    hpatch.addCode(tmp, 5);
                    memcpy(tmp, moveax0, 5);
                }
            }
        } else {
        }
    }


}

void AppHandler::setCoopDoSpawn(bool state) {
    if (m_bCoopDoSpawn != state) {
        memswap(m_SPModeSpawn,reinterpret_cast<uint8_t*>(&m_coopDoSpawn),5);
        m_bCoopDoSpawn = !m_bCoopDoSpawn;
    }
}

void AppHandler::patchClientServer(uint8_t *mod, uint32_t modSz) {
    // limit loop (synch) to 2^16 coz it may cause infinte loop
    PatchHandler &hpatch = *patchHandler();
    {
        static auto pat = BSF("DFE0F6C4057A2BC784248400000000000000C744241C00000000C744241800000000C744241000000000C74424140000803F");
        uint8_t *tmp = scanBytes(mod, modSz, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
            uint8_t d[] = {
                0x66, 0xC7, 0x44, 0x24, 0x58, 0x00, 0x00, 0x90, 0x90, 0x90,
                0x90, 0x90, 0xDF, 0xE0, 0xF6, 0xC4, 0x05, 0x7A, 0x1F, 0xC7,
                0x44, 0x24, 0x14, 0x00, 0x00, 0x80, 0x3F, 0x31, 0xC0, 0x89,
                0x44, 0x24, 0x1C, 0x89, 0x44, 0x24, 0x18, 0x89, 0x44, 0x24,
                0x10, 0x31, 0xC0, 0x89, 0x84, 0x24, 0x84, 0x00, 0x00, 0x00};
            hpatch.addCode(tmp, 50);
            memcpy(tmp, reinterpret_cast<uint8_t*>(&d), 50);
        }
    }
    {
        static auto pat = BSF("C784248400000000000000E9F5FEFFFF");
        unsigned char *tmp =
            scanBytes((unsigned char *)mod, modSz, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
            uint8_t d[] = {0x66, 0xFF, 0x44, 0x24, 0x58, 0x74,
                                 0x09, 0x90, 0x90, 0x90, 0x90, 0xE9,
                                 0xE9, 0xFE, 0xFF, 0xFF};
            hpatch.addCode(tmp, 16);
            memcpy(tmp, reinterpret_cast<uint8_t*>(&d), 16);
        }
    }
}


void AppHandler::loadConfig()
{
    {
        TCHAR cfg[2048];
        int sz;
        sz = GetModuleFileNameW(0, cfg, sizeof(cfg));
        if (sz) {
            for (int i = sz; i != 0; i--) {
                if (cfg[i] == '\\') {
                    sz = i + 1;
                    break;
                }
            }
            TCHAR *cfgName = _T("gamecfg.txt");
            memcpy(cfg + sz, cfgName, sizeof(TCHAR) * sizeof("gamecfg.txt"));
            __int64 cfgSize = (FileSize(cfg) + 1024) * sizeof(TCHAR);
            m_iniBuffer = (TCHAR *)malloc(cfgSize);
            TCHAR *pIniBuf = m_iniBuffer;
            //int sz = getGlobalCfgString(cfg, _T("NS1"), _T(""/*"natneg1.gamespy.com"*/), pIniBuf, cfgSize);
            int sz = getGlobalCfgString(cfg, _T("NS1"), L"natneg1.openspy.net", pIniBuf, cfgSize);

            m_strNs1 = reinterpret_cast<char *>(pIniBuf);
            pIniBuf = reinterpret_cast<TCHAR*>(StringUtil::copyFromNative(pIniBuf,
                                                                      reinterpret_cast<char *>(pIniBuf), sz));
//            sz = getGlobalCfgString(cfg, _T("NS2"), _T(""/*"natneg2.gamespy.com"*/), pIniBuf, cfgSize);
            sz = getGlobalCfgString(cfg, _T("NS2"), L"natneg2.openspy.net", pIniBuf, cfgSize);
            m_strNs2 = reinterpret_cast<char *>(pIniBuf);
            pIniBuf = reinterpret_cast<TCHAR*>(StringUtil::copyFromNative(pIniBuf,
                                                                       reinterpret_cast<char *>(pIniBuf), sz));
//            sz = getGlobalCfgString(cfg, _T("Available"),
//                                    _T(""/*"%s.available.gamespy.com"*/), pIniBuf,
//                                    cfgSize);
            sz = getGlobalCfgString(cfg, _T("Available"),
                                    L"available.openspy.net", pIniBuf,
                                    cfgSize);
            m_strMasterAvail = reinterpret_cast<char *>(pIniBuf);
            pIniBuf = reinterpret_cast<TCHAR*>(StringUtil::copyFromNative(pIniBuf,
                                                                       reinterpret_cast<char *>(pIniBuf), sz));
//            sz = getGlobalCfgString(cfg, _T("Master"),
//                                    _T(""/*"%s.master.gamespy.com"*/), pIniBuf, cfgSize);
            sz = getGlobalCfgString(cfg, _T("Master"),
                                    L"master.openspy.net", pIniBuf, cfgSize);
            m_strMaster = reinterpret_cast<char *>(pIniBuf);
            pIniBuf = reinterpret_cast<TCHAR*>(StringUtil::copyFromNative(pIniBuf,
                                                                       reinterpret_cast<char *>(pIniBuf), sz));
            sz = getGlobalCfgString(
                cfg, _T("MOTD"),
                (TCHAR
                     *)L"", // http://motd.gamespy.com/motd/vercheck.asp?userid=%d&productid=%d&versionuniqueid=%s&distid=%d&uniqueid=%s&gamename=%s
                pIniBuf, cfgSize);
            m_strMotd = reinterpret_cast<char *>(pIniBuf);
            StringUtil::copyFromNative(pIniBuf,
                                                                       reinterpret_cast<char *>(pIniBuf), sz);
        }
    }
}

void AppHandler::hookLoadArchives(SpliceHandler::reg *p) {
    // if(*reinterpret_cast<uintptr_t*>(p->tbx+0x184) == 0)
    //     return;
    auto s =  reinterpret_cast<char**>(*(reinterpret_cast<uintptr_t*>(p->tdi) + 1)+0xAC);
    if(s && *(s) && **(s)!=0 && strstr(*s, "\\AdditionalContent\\") == 0) {
        p->state = 0;
        p->origFunc = reinterpret_cast<uintptr_t>(p->pt->hookPoint + 43);
    }
}

void AppHandler::hookMaxPlayersHUD(SpliceHandler::reg *p) {
    if(p->tax==15)
        p->tbx=0;
}

void AppHandler::hookZeroConfigFix(SpliceHandler::reg *p) {
    auto s = reinterpret_cast<char *>(p->tsi);
    bool ascii = true;
    while(auto c = *s) {
        if((c & 0x80) != 0) {ascii = false; break;};
        ++s;
    }
    if(!ascii || (s == reinterpret_cast<char *>(p->tsi)))
        p->tsi = reinterpret_cast<uintptr_t>("ServerOptions0000");
}

void AppHandler::hookZeroConfigFix2(SpliceHandler::reg *p) {
    if(auto newpoint = reinterpret_cast<uintptr_t>(strstr(reinterpret_cast<char *>(p->tcx),"ServerOptions"))) {
        p->tcx = newpoint++;
    }
    // if(strstr(reinterpret_cast<char *>(p->tcx), ":") == 0) {
    //     if(auto newpoint = reinterpret_cast<uintptr_t>(strstr(reinterpret_cast<char *>(p->tcx),"\\"))) {
    //         p->tcx = newpoint++;
    //     }
    // }
}

void AppHandler::hookMaxPlayers(SpliceHandler::reg *p) {
  if (p->tax) {
    auto dbStr = reinterpret_cast<char **>(p->tax);
    if (strcmp(*dbStr, "MaxPlayers.0.Range") == 0) {
        auto ptr = **reinterpret_cast<uintptr_t **>((reinterpret_cast<uint8_t *>(p->tax)+0xC));
        *reinterpret_cast<float *>(ptr+4) = MAX_PLAYERSF;
    }
  }
}

void AppHandler::init()
{
    srand(time(0));
    loadConfig();
    SpliceHandler &hsplice = *spliceHandler();
    {
        static auto pat = BSF("E8????????83C4108D4424");
        unsigned char *tmp = scanBytes(
            (unsigned char *)m_eServer, m_eServerSz, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
            m_internalFuncPathCreateFolders = getVal4FromRel(tmp + 1);
        }
    }
    {
      static auto pat = BSF("68????????8D8C24????????680401000051E8????????8D9424????????52");
      hsplice.spliceUp(
          scanBytes(
              (unsigned char *)m_eServer, m_eServerSz, reinterpret_cast<uint8_t *>(&pat)),
          (void *)hookAdditionalDirectoryCreate);
    }
    {
        static auto pat = BSF("84??75????68??????????E8????????8B??83C40885??74");
        hsplice.spliceUp(
            scanBytes((unsigned char *)m_eServer, m_eServerSz, reinterpret_cast<uint8_t *>(&pat)),
            (void *)hookLoadGameServer);
        // hsplice.spliceUp(
        //     scanBytes((unsigned char *)m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat)),
        //     (void *)hookLoadGameServer);
    }
    {
      static auto pat = BSF("8B0D????????83C40C5056FF97");
      hsplice.spliceUp(
          scanBytes(
              (unsigned char *)m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat)),
          (void *)hookMaxPlayers);
    }
    {
      static auto pat = BSF("8D????????????8B??E8????????8B????85C075??E8????????8B");
      hsplice.spliceUp(
          scanBytes(
              (unsigned char *)m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat)),
          (void *)hookConfigLoad);
    }
    {
      static auto pat = BSF("8B7704558BCEE8????????8BCE");
      hsplice.spliceUp(
          scanBytes(
              (unsigned char *)m_eServer, m_eServerSz, reinterpret_cast<uint8_t *>(&pat)),
          (void *)hookLoadArchives);
    }
    {
      static auto pat = BSF("A1????????508B??E8????????83");
      unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
          (unsigned char *)m_eServer, m_eServerSz, reinterpret_cast<uint8_t *>(&pat)));
      if (tmp) {
        const char **arr = (const char **)*(uintptr_t *)(tmp + 1);
        unprotectMem((uint8_t *)arr);
        arr[0] = m_strNs1;
        arr[1] = m_strNs2;
      }
    }
    patchEndpoints(m_eServer, m_eServerSz);
    setMotd(m_eServer, m_eServerSz);
}

void AppHandler::serverPreinitPatches() {
    PatchHandler &hpatch = *patchHandler();
    SpliceHandler &hsplice = *spliceHandler();
  // FixLagSetWindowsHookExA
  if (static auto pat = BSF("8D8424????????50568BD9E8????????84C0");
      uint8_t *adr =
          scanBytes(m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat))) {
      hsplice.spliceUp(
          scanBytes(
              (unsigned char *)m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat)),
          (void *)hookZeroConfigFix);
    {
      uint8_t d[] = {0x6a,0x01};
      hpatch.addCode(adr+40, sizeof(d));
      memcpy(adr+40, reinterpret_cast<uint8_t *>(&d), sizeof(d));
    }
    hsplice.spliceUp(adr+81,(void *)hookZeroConfigFix2);
    // {
    //   uint8_t d[] = {0x90, 0x90, 0x90, 0x90, 0x90};
    //   hpatch.addCode(adr+77, sizeof(d));
    //   memcpy(adr+77, reinterpret_cast<uint8_t *>(&d), sizeof(d));
    // }
    // {
    //   uint8_t d[] = {0x8D, 0x8C, 0x24, 0x18, 0x02,
    //                  0x00, 0x00, 0x51, 0xEB, 0x0E};
    //   hpatch.addCode(adr+40, sizeof(d));
    //   memcpy(adr+40, reinterpret_cast<uint8_t *>(&d), sizeof(d));
    // }
    // {
    //   uint8_t d[] = {0x90, 0x90, 0x90, 0x90, 0x90};
    //   hpatch.addCode(adr+77, sizeof(d));
    //   memcpy(adr+77, reinterpret_cast<uint8_t *>(&d), sizeof(d));
    // }
  }
  if(m_eServerModule =
        reinterpret_cast<uint8_t *>(GetModuleHandle(_T("engineserver.dll")))){
    auto exec_section =
        GetModuleFirstExecSection(reinterpret_cast<HMODULE>(m_eServerModule));
    m_eServer = reinterpret_cast<uint8_t *>(m_eServerModule +
                                            exec_section->VirtualAddress);
    m_eServerSz = exec_section->SizeOfRawData;
  }
    // m_eServer =
    //         reinterpret_cast<uint8_t *>(LoadLibrary(_T("engineserver.dll")));
    // m_eServerSz = m_eServerSz = GetModuleSize((HMODULE)m_eServer);
    { // KickPBCLDLL server
        static auto pat = BSF("A1????????85C00F85????????C705");
        uint8_t *tmp =
            scanBytes(m_eServer, m_eServerSz, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
            unprotectCode(tmp, 1);
            *static_cast<uint8_t *>(tmp) = 0xc3;
        }
    }
}

void AppHandler::clientPreinitPatches() {
  // FixLagSetWindowsHookExA
  if (static auto pat = BSF("74??A1????????85C075??6A006A00FF15");
      uint8_t *adr =
          scanBytes(m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat))) {
    unprotectCode(adr, 1);
    *static_cast<uint8_t *>(adr) = 0xEB;
  }
  // FixDinputLag
  if (static auto pat = BSF("74??6A02578BCEE8????????8B");
      uint8_t *adr =
          scanBytes(m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat))) {
    unprotectCode(adr, 1);
    *static_cast<uint8_t *>(adr) = 0xEB;
  }
  // NoICMPDLL
  if (static auto pat = BSF("74??E8????????68????????FF");
      uint8_t *adr =
          scanBytes(m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat))) {
    unprotectCode(adr, 1);
    *static_cast<uint8_t *>(adr) = 0xEB;
  }
  // `
  if (static auto pat = BSF("A1????????85C00F85????????C7");
      uint8_t *adr =
          scanBytes(m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat))) {
    unprotectCode(adr, 1);
    *static_cast<uint8_t *>(adr) = 0xC3;
  }
  // NoMutex
  if (static auto pat = BSF("6A006A00FF15????????8BF885FF74");
      uint8_t *adr =
          scanBytes(m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat))) {
    unprotectCode(adr, 5);
    static uint8_t d[] = {0x58, 0x31, 0xC0, 0xEB, 0x05};
    memcpy(adr, reinterpret_cast<uint8_t *>(&d), 5);
  }
  // NoLosingFocus
  if (static auto pat = BSF("C705????????000000008B116A");
      uint8_t *adr =
          scanBytes(m_Exec, m_ExecSz,
                    reinterpret_cast<uint8_t *>(&pat))) {
    adr += 6;
    unprotectCode(adr, 7);
    static uint8_t d[] = {0x01, 0x00, 0x00, 0x00, 0x58, 0xEB, 0x38};
    memcpy(adr, reinterpret_cast<uint8_t *>(&d), 7);
  }
}

void AppHandler::hookSwitchToModeXP(SpliceHandler::reg *p) {
  auto &inst = *ExecutionHandler::instance()->appHandler();
  if (p->tax == 2) {
    p->tax = 0;
    *(inst.m_RunGameModeXP + 13) = 0x3;
    inst.setMpGame(1);
  } else if (!p->tax) {
    *(inst.m_RunGameModeXP + 13) = 0x2;
    inst.setMpGame(0);
  }
}

void AppHandler::initClient() {

    {
        TCHAR cfg[1024];
        int sz;
        sz = static_cast<int>(GetModuleFileNameW(
            nullptr, reinterpret_cast<wchar_t *>(cfg), sizeof(cfg)));
        if (sz) {
            for (int i = sz; i != 0; i--) {
                if (cfg[i] == '\\') {
                    sz = i + 1;
                    break;
                }
            }
            TCHAR *cfgName = _T("gamecfg.txt");
            memcpy(cfg + sz, cfgName, sizeof(TCHAR) * sizeof("gamecfg.txt"));
            size_t cfgSize =
                static_cast<size_t>((FileSize(cfg) + 1024) * sizeof(TCHAR));
            m_iniBuffer = reinterpret_cast<TCHAR *>(malloc(cfgSize));
            TCHAR *pIniBuf = m_iniBuffer;
//            size_t sz = getGlobalCfgString(
//                cfg, _T("Master"),
//                _T(""),
//                pIniBuf, cfgSize);
            size_t sz = getGlobalCfgString(
                cfg, _T("Master"),
                L"master.openspy.net",
                pIniBuf, cfgSize);
            m_strMaster = reinterpret_cast<char *>(pIniBuf);
            m_isHttpMaster = memcmp(m_strMaster, L"http", sizeof(wchar_t)*4) == 0;
            pIniBuf = reinterpret_cast<TCHAR*>(StringUtil::copyFromNative(pIniBuf,
                                   reinterpret_cast<char *>(pIniBuf), sz));
            sz = getGlobalCfgString(
                cfg, _T("MOTD"),
                _T("?userid=%d&productid=%d&versionuniqueid=%s&distid=%d&uniqueid=%s&gamename=%s"),
                pIniBuf, cfgSize);
            m_strMotd = reinterpret_cast<char *>(pIniBuf);
            pIniBuf = reinterpret_cast<TCHAR*>(StringUtil::copyFromNative(pIniBuf,
                                   reinterpret_cast<char *>(pIniBuf), sz));
            m_bShowIntro = getGlobalCfgInt(cfg, _T("ShowIntro"));
            setMotd(m_Exec, m_ExecSz);
        }
    }
    if(!m_isHttpMaster) {
        loadConfig();
        patchEndpoints(m_Exec, m_ExecSz);
    }

    SpliceHandler &hsplice = *spliceHandler();
    PatchHandler &hpatch = *patchHandler();
    SdkHandler &hsdk = *sdkHandler();
//    {
//        uint8_t *tmp = reinterpret_cast<uint8_t*>(0x00402F40);
//        if (tmp) {
//            hsplice.spliceUp(tmp,
//                             reinterpret_cast<void *>(hookPlayWave));
//        }
//    }
    // {
    //     static auto pat = BSF("8B??????????85F60F??????????A1????????F6C40174");
    //     uint8_t *tmp =
    //         scanBytes(m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat));
    //     if (tmp) {
    //         hsplice.spliceUp(tmp,
    //                          reinterpret_cast<void *>(hookClientSettingsLoad));
    //     }
    // }

    {
        static auto pat = BSF("A1????????8B??????????8B????FF????33");
        uint8_t *tmp = scanBytes(m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat));
        uint8_t *adr;
        m_gameClientStruct = reinterpret_cast<uint8_t *>(
            *reinterpret_cast<uintptr_t *>(tmp + 1));
        while (true) {
            adr = reinterpret_cast<uint8_t *>(
                *reinterpret_cast<uintptr_t *>(tmp + 1));
            if (*reinterpret_cast<uintptr_t *>(adr)) {
                adr = reinterpret_cast<uint8_t *>(
                    *reinterpret_cast<uintptr_t *>(adr));
                adr = adr + *reinterpret_cast<uintptr_t *>(tmp + 7);
                if (*reinterpret_cast<uintptr_t *>(adr)) {
                    adr = reinterpret_cast<uint8_t *>(
                        *reinterpret_cast<uintptr_t *>(adr));
                    if (*reinterpret_cast<uintptr_t *>(adr)) {
                        adr = reinterpret_cast<uint8_t *>(
                            *reinterpret_cast<uintptr_t *>(adr));
                        break;
                    }
                }
            }
            Sleep(50);
        }
        m_ClientModule = reinterpret_cast<uint8_t *>(adr);
    }
    {
      auto exec_section =
          GetModuleFirstExecSection(reinterpret_cast<HMODULE>(m_ClientModule));
      m_Client = reinterpret_cast<uint8_t *>(m_ClientModule +
                                           exec_section->VirtualAddress);
      m_ClientSz = exec_section->SizeOfRawData;
    }
    {
      static auto pat = BSF("8B0D????????83C40C5056FF97");
      hsplice.spliceUp(
          scanBytes(
              (unsigned char *)m_Client, m_ClientSz, reinterpret_cast<uint8_t *>(&pat)),
          (void *)hookMaxPlayers);
    }
    {
      static auto pat = BSF("8B9BF40000004085DB8901");
      hsplice.spliceUp(
          scanBytes(
              (unsigned char *)m_Client, m_ClientSz, reinterpret_cast<uint8_t *>(&pat))+6,
          (void *)hookMaxPlayersHUD);
    }
    
    // {
    //     static auto pat = BSF("E8????????84C00F84????????8B750C");
    //     uint8_t *tmp = scanBytes(
    //         m_Client, m_ClientSz, reinterpret_cast<uint8_t *>(&pat));
    //     if (tmp) {
    //         uint8_t d[] = {0xb8, 0, 0, 0, 0};
    //         hpatch.addCode(tmp, sizeof(d));
    //         memcpy(tmp, reinterpret_cast<uint8_t*>(&d), sizeof(d)); // no logos
    //     }
    // }
    
    // m_ClientSz = GetModuleSize(reinterpret_cast<HMODULE>(m_Client));
//    {
//        uint8_t *tmp =
//            scanBytes(m_Client, m_ClientSz,
//                      BSF("740E8B??????????85C00F??????????A1"));  // MOTD info
//        if (tmp) {
//            hpatch.addCode(tmp, 2);
//            *reinterpret_cast<uint16_t *>(tmp) = 0x9090;
//        }
//    }
    patchClientServer(m_Exec, m_ExecSz);
    {
        static auto pat = BSF("74278D4C2410518D54240852");
        uint8_t *tmp = scanBytes(m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
            hpatch.addCode(tmp,2);
            *(uint16_t *)(tmp) = 0x53EB;  // kick GameSpy key
        }
    }
    // {
    //     static auto pat = BSF("837C24??0175??33D2");
    //     uint8_t *tmp = scanBytes(m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat));
    //     if (tmp) {
    //         hpatch.addCode(tmp+5,1);
    //         *(tmp+5) = 0xEB;  //%s.available.gamespy.com
    //     }
    // }
    // {
    //     static auto pat = BSF("8B??74??E8????????68????????FF??????????85??89");
    //     uint8_t *tmp =
    //         scanBytes(m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat));
    //     if (tmp) {
    //         hpatch.addCode(tmp+2,1);
    //         *(tmp+2) = 0xEB;  // ICMP disable
    //     }
    // }
    {
        static auto pat = BSF("B9????????E8????????B9????????E8????????B9????????C7");
        uint8_t *tmp =
            scanBytes(m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat));  // Punkbuster disable
        if (tmp) {
            tmp = (uint8_t *)*(unsigned *)(tmp + 11);
            *(uintptr_t *)(tmp + 0x10) = 0;
        }
    }
    {
        static auto pat = BSF("E8????????8A??????????B8????????33FF84C875");
        uint8_t *tmp = scanBytes(
            m_Client, m_ClientSz, reinterpret_cast<uint8_t *>(&pat));  // fix
        // keyboard
        if (tmp) {
            uint8_t d[] = {0x90, 0x90, 0x90, 0x90, 0x90};
            hpatch.addCode(tmp,5);
            memcpy(tmp, reinterpret_cast<uint8_t*>(&d), 5);
        }
    }
    {
        static auto pat = BSF("74??8B????FF????????????6A");
        uint8_t *tmp =
            scanBytes(m_Client, m_ClientSz, reinterpret_cast<uint8_t *>(&pat));  // no weapon bug
        if (tmp) {
            hpatch.addCode(tmp + 1, 1);
            *(uint8_t *)(tmp + 1) -= 7;
        }
    }
    {
        static auto pat = BSF("FF52??6A1BFF15");
        uint8_t *tmp = scanBytes(
            m_Client, m_ClientSz, reinterpret_cast<uint8_t *>(&pat));  // No escape key
        // handling on
        // downloading
        // content (causes crash)
        // only for 1.08 exe
        if (tmp) {
            uint8_t d[] = {0xB8,0x00,0x00,0x00,0x00,0x90,0x90,0x90};
            hpatch.addCode(tmp+3, sizeof(d));
            memcpy(tmp+3, reinterpret_cast<uint8_t*>(&d), sizeof(d));
            // *(unsigned short *)(tmp) = 0x3BEB; // or perhaps fix crash when download connect processed
        }
    }
    if (!m_bShowIntro) {
        {
            static auto pat = BSF("5155568BF18B0D????????8B01FF90????????8BE8");
            uint8_t *tmp = scanBytes(m_Client, m_ClientSz, reinterpret_cast<uint8_t *>(&pat));  // no
                                                                  // splash
            // videos
            if (tmp) {
                hpatch.addCode(tmp, 4);
                *(unsigned *)(tmp) = 0x000004C2;
            }
        }

        {
            static auto pat = BSF("8B0D??????????FF57??85ED89??????76");
            uint8_t *tmp = scanBytes(
                m_Client, m_ClientSz, reinterpret_cast<uint8_t *>(&pat));
            if (tmp) {
                uint8_t d[] = {0xE9, 0xF4, 0x00, 0x00, 0x00};
                hpatch.addCode(tmp + 16, 5);
                memcpy(tmp+16, reinterpret_cast<uint8_t*>(&d), 5); // no logos
            }
        }
    }
    {
        static auto pat = BSF("FF????84??75??A1????????6A01??8D????????8B");
        uint8_t *tmp = scanBytes(
            m_Client, m_ClientSz, reinterpret_cast<uint8_t *>(&pat));  // MOTD
        if (tmp) {
            hpatch.addCode(tmp + 5, 1);
            *(tmp + 5) = 0xEB;
        }
    }
    {
        static auto pat = BSF("7D??B22D33??381088??????75");
        uint8_t *tmp =
            scanBytes(m_Client, m_ClientSz, reinterpret_cast<uint8_t *>(&pat));  // cdkey menu fix
        if (tmp) {
            hpatch.addCode(tmp, 1);
            *(tmp) = 0xEB;
        }
    }
    {
        static auto pat = BSF("FF50??8BF085F60F84????????8B0655");
        uint8_t *tmp = scanBytes(
            m_Client, m_ClientSz, reinterpret_cast<uint8_t *>(&pat));  // NoUpdatePunkBuster
        if (tmp) {
            uint8_t d[] = {0x58, 0x31, 0xC0};
            hpatch.addCode(tmp,3);
            memcpy(tmp, reinterpret_cast<uint8_t*>(&d), 3);
        }
    }
    {
        static auto pat = BSF("0F94C16A01518B8E????????52");
        uint8_t *tmp =
            scanBytes(m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
            hpatch.addCode((uint8_t *)tmp, 4); // ping in server list not working somehow
            *(unsigned *)tmp = 0x6a90c931;
        }
    }
    {
      static auto pat = BSF("8BCF89B71C060000E8????????8B871C060000"); // only fear 1.08 exe
      g_doConnectIpAdrTramp = scanBytes(
          m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat));
    }
    {
      static auto pat = BSF("FFD38B54242083C40C80660CDF");
      hsplice.spliceUp(
          scanBytes(m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat)),
          (void *)hookModelMsg);
    }
    hsdk.initClient();
    {
      static auto pat = BSF("0F8467010000A1????????85C08BC87522A1????????5068????????E8");
      uint8_t *tmp = scanBytes(
          m_Exec, m_ExecSz, reinterpret_cast<uint8_t *>(&pat));
      if (tmp) {
        hpatch.addCode(tmp + 2, 2);
        *(unsigned short *)(tmp + 2) = 0x007D;
        }
    }
    {
        static auto pat = BSF("74078BCEE8????????8B166A008BCEFF5268");
        uint8_t *tmp =
            scanBytes(m_Client, m_ClientSz, reinterpret_cast<uint8_t *>(&pat));  // MOTD
        if (tmp) {
            hpatch.addCode(tmp, 1);
            *(tmp) = 0xEB;
        } /*else {
            // Extraction Point
            m_RunGameModeXP =
                scanBytes((uint8_t *)m_Client, m_ClientSz,
                          BYTES_SEARCH_FORMAT("FF????????????8B????????6A02FF5044"));
            hsplice.spliceUp(m_RunGameModeXP, (void *)hookSwitchToModeXP);
            m_isMultiplayerGameClient = scanBytes(
                (uint8_t *)m_Client, m_ClientSz,
                BYTES_SEARCH_FORMAT("A1????????85??74??8B????83??0474??83??0574"));
            unprotectCode(m_isMultiplayerGameClient);
        }*/
    }
    if (g_doConnectIpAdrTramp) {
      {
        static auto pat = BSF("E8????????84C074218D4C2404E8????????6A016A008D44240C50");
        hsplice.spliceUp(
            scanBytes(
                m_Client, m_ClientSz, reinterpret_cast<uint8_t *>(&pat)),
            (void *)hookSwitchToSP /*, (void *)6*/);
      }
      {
        static auto pat =
            BSF("E8????????84C0754B8D4C2404E8????????6A016A008D44240C50");
        void *tmp = scanBytes(m_Client, m_ClientSz, reinterpret_cast<uint8_t *>(&pat));
        m_isMultiplayerGameClient = (uint8_t *)((uint8_t *)tmp + 1) +
                                    (*(uintptr_t *)((uint8_t *)tmp + 1)) + 4;
        unprotectCode(m_isMultiplayerGameClient, 100);
        hsplice.spliceUp(tmp, (void *)hookSwitchToMP);
    }

      // toggleIsMPGame();
    }
    {
        static auto pat = BSF("A1????????8A887806000084C9740E8A887906000084C974048AC3");
        uint8_t *tmp = scanBytes(
            m_Client, m_ClientSz, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
            m_storyModeStruct = (uint8_t *)*(uintptr_t *)(tmp + 1);
        }
    }
    {
        static auto pat = BSF("6A00FF??????????8B??????????8B????FF??????????8B");
        uint8_t *tmp =
            scanBytes(m_Client, m_ClientSz, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
            unprotectCode(tmp, 100);
            *(unsigned short *)(tmp) = 0x9050;

            hsplice.spliceUp(tmp, (void *)hookStoryModeView);
        }
    }
    {
        static auto pat = BSF("50E8????????8B??E8????????84C00F????????????E8");
        uint8_t *tmp =
            scanBytes(m_Client, m_ClientSz, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
            hsplice.spliceUp(tmp, (void *)hookClientGameMode);
        }
    }
    {
      static auto pat = BSF("E8????????84C075??8B??????????8B??8D????????33??FF");
      uint8_t *tmp = m_Client;
      unsigned i = 0;
      while (true) {
        tmp = scanBytes(
            (uint8_t *)tmp, (m_ClientSz + m_Client) - tmp, reinterpret_cast<uint8_t *>(&pat));
        if (tmp) {
          unprotectCode(tmp, 100);
          m_flashlightAdr[i] = tmp;
          // memcpy(tmp,moveax0,5);
        }
        tmp++;
        i++;
        if (i == 2) break;
        }
    }
}

uintptr_t AppHandler::getCfgInt(char *pathCfg, char *valStr, int def) {
    return GetPrivateProfileIntA("Extra", valStr, def, pathCfg);
}

size_t AppHandler::getGlobalCfgString(wchar_t *pathCfg, wchar_t *valStr,
                                      wchar_t *strDefault, wchar_t *buf,
                                      size_t nSize) {
    const wchar_t *app = isServer() ? _T("Server") : _T("Client");
    GetPrivateProfileString(app, valStr, strDefault, buf, nSize, pathCfg);
    return wcslen(buf) + 1;
}

size_t AppHandler::getGlobalCfgInt(wchar_t *pathCfg, wchar_t *valStr) {
    const wchar_t *app = isServer() ? _T("Server") : _T("Client");
    return GetPrivateProfileInt(app, valStr, 0, pathCfg);
}

void AppHandler::setMpGame(bool state)
{
    uint8_t d[] = {0xB0, 0x01, 0xC3};
    memcpy((unsigned char *)m_isMultiplayerGameClient, d, 3);
    if (state)
        *(m_isMultiplayerGameClient + 1) = 1;
    else
        *(m_isMultiplayerGameClient + 1) = 0;
}

void AppHandler::setFlashlight(bool state) {
    if (m_bFlashlight != state) {
        memswap((uint8_t *)m_flashlightAdr[0], reinterpret_cast<uint8_t*>(&m_flashlight1), 5);
        memswap((uint8_t *)m_flashlightAdr[1], reinterpret_cast<uint8_t*>(&m_flashlight2), 5);
        m_bFlashlight = !m_bFlashlight;
    }
}

