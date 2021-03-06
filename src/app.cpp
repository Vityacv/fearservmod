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
#include "conv.h"
#include "patch.h"
// 0.3125 3EA00000

/*
74 12 8B 50 28 89 53 20 8B 48 2C 89 4B 24 8B 50 30
8B 54 24 14 8B 44 24 18 42 3B D0 89 54 24 14 0F 8C EE FE FF FF
0F 84 E1 00 00 00 8B 4E 08 8B 01 57 FF 50 0C 8B D8 F6 C3 01
loop


56 55 52 57 E8 59 FA FF FF EB 16
D9 55 F8 D9 55 08 D8 CA D9 5D B8 D8 4D 08 D9 5D BC DD D8 D9 45 B4 D8 4D 08 D9 5D
C0 loop2 engineserver
*/

#define NOINTRO

unsigned char *doConnectIpAdrTramp;
void regcall hookChangeStr(reg *p) {
  p->state = 2;
  memcpy((void *)p->tax, "fear", sizeof("fear"));
  strcpy((char *)(p->tdx + p->tax), (char *)p->tax);
}

int regcall getCfgInt(char *pathCfg, char *valStr) {
  return GetPrivateProfileIntA("Extra", valStr, 0, pathCfg);
}

int regcall getGlobalCfgString(bool server, TCHAR *pathCfg, TCHAR *valStr,
                               TCHAR *strDefault, TCHAR *buf, int nSize) {
  TCHAR *app = server ? _T("Server") : _T("Client");
  GetPrivateProfileString(app, valStr, strDefault, buf, nSize, pathCfg);
  return _tcslen(buf) + 1;
}

int regcall getGlobalCfgInt(bool server, TCHAR *pathCfg, TCHAR *valStr) {
  TCHAR *app = server ? _T("Server") : _T("Client");
  return GetPrivateProfileInt(app, valStr, 0, pathCfg);
}

void regcall hookGameMode(reg *p) {
  memcpy((void *)p->tax, L"SinglePlayer", 13 * 2);
  wchar_t *val = (wchar_t *)((unsigned char *)p->tax - 0x208);
  memcpy((void *)val, L"SinglePlayer", 13 * 2);
  memcpy((void *)((unsigned char *)p->tsi + 4), "SinglePlayer", 13);
}

void regcall hookGameMode2(reg *p) {
  memcpy((void *)p->tax, "SinglePlayer", 13);
}

void regcall appData::setCoopDoSpawn(bool state) {
  if (bCoopDoSpawn != state) {
    memswap(
        (unsigned char *)aSPModeSpawn,
        (unsigned char *)(const unsigned char[]){0x31, 0xC0, 0x90, 0x90, 0x90},
        5);
    bCoopDoSpawn = !bCoopDoSpawn;
  }
}

void regcall appData::setPatchHoleKillHives(bool state) {
  if (flagPatchHoleKillHives != state) {
    memswap((unsigned char *)aPatchHoleKillHives,
            (unsigned char *)(const unsigned char[]){0xEB}, 1);
    flagPatchHoleKillHives = !flagPatchHoleKillHives;
  }
}

void regcall appData::setMpGame(bool state) {
  memcpy((unsigned char *)aIsMultiplayerGameClient,
         (unsigned char *)(const unsigned char[]){0xB0, 0x01, 0xC3}, 3);
  if (state)
    *(aIsMultiplayerGameClient + 1) = 1;
  else
    *(aIsMultiplayerGameClient + 1) = 0;
}

void regcall appData::setFlashlight(bool state) {
  if (bFlashlight != state) {
    memswap(
        (unsigned char *)aFlashlight[0],
        (unsigned char *)(const unsigned char[]){0x31, 0xC0, 0x90, 0x90, 0x90},
        5);
    memswap(
        (unsigned char *)aFlashlight[1],
        (unsigned char *)(const unsigned char[]){0x31, 0xC0, 0x90, 0x90, 0x90},
        5);
    bFlashlight = !bFlashlight;
  }
}

extern "C" void
hookOnConnectServerRet() { //-fomit-frame-pointer required :troll:
  // appData *aData = &handleData::instance()->aData;
  asm volatile("push %ebx");
  asm volatile("mov %0,%%eax" : "=m"(doConnectIpAdrTramp));
  asm volatile("jmp *%eax");
  __builtin_unreachable();
}

void regcall hookSwitchToSP(reg *p) {
  fearData *pSdk = &handleData::instance()->pSdk;
  p->state = 2;
  pSdk->setExeType(0);
  p->tax = 0;
}

void regcall hookSwitchToMP(reg *p) {
  fearData *pSdk = &handleData::instance()->pSdk;
  p->state = 2;
  pSdk->setExeType(1);
  p->tax = 1;
}

void regcall hookStoryModeOn(reg *p) {
  appData *aData = &handleData::instance()->aData;
  //*(aData->aFreeMovement)=0xE9;
  //*(unsigned short *)(aData->aFreeMovement) = 0x09EB;
  aData->pSdk->freeMovement = 1;
  //*(uintptr_t*)(aData->aMPMovement1+1)=getRel4FromVal((aData->aMPMovement1+1),aData->aMPMovement2);
}

void regcall hookDoorTimer(reg *p) {
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

void regcall hookStoryModeOff(reg *p) {
  appData *aData = &handleData::instance()->aData;

  char *lvlName = aData->pSdk->getCurrentLevelName();
  aData->storyModeCnt++;
  if (lvlName) {
    switch (hash_rta(lvlName)) {
    case hash_ct("01_Intro"):
    case hash_ct("07_ATC_Roof"):
    case hash_ct("XP2_W01"):
    case hash_ct("XP2_W06"):
      return;
    }
  }
  //*(unsigned short *)(aData->aFreeMovement) = 0x9090;
  aData->pSdk->freeMovement = 0;
}

void regcall hookfRateFix(reg *p) {
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
const char *wepsmpstr[] = {
    "Remote Charge",   "Pistol",        "Submachinegun",
    "Shotgun",         "Assault Rifle", "Nail Gun",
    "Semi-auto Rifle", "Cannon",        "Missile Launcher",
    "Plasma Weapon",   "Vulcan Cannon", "Turret_Helicopter",
    "Turret_Remote",   "Turret_Street", "Frag Grenade",
    "Proximity",       "Unarmed",       0};

void regcall hookOnAddClient_CheckNickname(reg *p) {
  fearData *pSdk = &handleData::instance()->pSdk;
  NetClientData *ncd = (NetClientData *)p->tax;
  p->state = 2;
  p->tax = ((uintptr_t(__thiscall *)(
      uintptr_t, uintptr_t))pSdk->g_pGetNetClientData)(p->tsi, p->tax);
  if (p->tax) {
    int isUni = IS_TEXT_UNICODE_ILLEGAL_CHARS;
    IsTextUnicode((void *)ncd->m_szName,
                  wcslen(ncd->m_szName) * sizeof(wchar_t), &isUni);
    if (isUni & IS_TEXT_UNICODE_ILLEGAL_CHARS)
      p->tax = (uintptr_t)pSdk->m_hUnarmedRecord;
  }
}

void regcall hookEnterSlowMo(reg *p) {
  fearData *pSdk = &handleData::instance()->pSdk;
  p->v0 = (uintptr_t)pSdk->m_hFastForward;
}

void regcall hookRandomWeapon(reg *p) {
  p->state = 2;
  fearData *pSdk = &handleData::instance()->pSdk;
  unsigned timeMS = pSdk->getRealTimeMS();
  if (!pSdk->randomWeaponTime || timeMS - pSdk->randomWeaponTime > 90000) {
    pSdk->randomWeaponTime = timeMS;
    const char *szWep =
        wepsmpstr[pSdk->randomWepTable[pSdk->currentRandomWeaponInd]];
    if (pSdk->currentRandomWeaponInd == 17)
      pSdk->currentRandomWeaponInd = 0;
    else
      pSdk->currentRandomWeaponInd++;
    pSdk->currentRandomWeapon =
        pSdk->g_pLTDatabase->GetRecord(pSdk->m_hCatWeapons, szWep);
    ((void(__thiscall *)(void *, const char *, uintptr_t,
                         uintptr_t))pSdk->CCommandMgr_QueueCommand)(
        pSdk->pCmdMgr, "msg Player RESETINVENTORY", 0, 0);
    char str[64];
    sprintf(str, "msg Player (ACQUIREWEAPON %s)", szWep);
    ((void(__thiscall *)(void *, char *, uintptr_t, uintptr_t))
         pSdk->CCommandMgr_QueueCommand)(pSdk->pCmdMgr, str, 0, 0);
    int i = 0;
    for (i; i != 5; i++) {
      sprintf(str, "msg Player (ACQUIREAMMO %s)", szWep);
      ((void(__thiscall *)(void *, char *, uintptr_t, uintptr_t))
           pSdk->CCommandMgr_QueueCommand)(pSdk->pCmdMgr, str, 0, 0);
    }
    if (!pSdk->randomWepTable[pSdk->currentRandomWeaponInd]) {
      ((void(__thiscall *)(void *, const char *, uintptr_t,
                           uintptr_t))pSdk->CCommandMgr_QueueCommand)(
          pSdk->pCmdMgr, "msg Player (CHANGEWEAPON Unarmed)", 0, 0);
    } else {
      sprintf(str, "msg Player (CHANGEWEAPON %s)", szWep);
      ((void(__thiscall *)(void *, char *, uintptr_t, uintptr_t))
           pSdk->CCommandMgr_QueueCommand)(pSdk->pCmdMgr, str, 0, 0);
    }

    p->tax = 0;
  } else
    p->tax = (uintptr_t)pSdk->currentRandomWeapon;
}

void regcall hookPickupRndWeapon(reg *p) {
  fearData *pSdk = &handleData::instance()->pSdk;
  if (pSdk->currentRandomWeapon != (HWEAPON)p->tax)
    p->tax = 0;
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

void regcall hookMID(reg *p) {
  p->argcnt = 2;
  handleData *hData = handleData::instance();
  appData *aData = &hData->aData;
  fearData *pSdk = &hData->pSdk;
  readMsg *pMsg = (readMsg *)((unsigned char *)p->v1 + 8);
  uintptr_t v1 = pMsg->f12, v2 = (uintptr_t)pMsg->f16, v3 = pMsg->f20,
            v4 = pMsg->f24;
  CAutoMessageBase_Read *pMsgRead = (CAutoMessageBase_Read *)p->v1;
  HOBJECT hClientObj = pSdk->GetClientObject((HCLIENT)p->v0);
  // if (!hClientObj) return;
  GameClientData *pGameClientData = pSdk->getGameClientData((HCLIENT)p->v0);
  if (!pGameClientData) {
    p->state = 1;
    return;
  }
  unsigned clientId = pSdk->g_pLTServer->GetClientID((HCLIENT)p->v0);
  CPlayerObj *pPlayerObj = pSdk->GetPlayerFromHClient((HCLIENT)p->v0);
  /*if (!pPlayerObj) {
    p->state = 1;
    return;
  }*/
  playerData *pPlData = &pSdk->pPlayerData[clientId];
  unsigned playerState = 0;
  if (pPlayerObj) {
    playerState = *(unsigned *)((unsigned char *)pPlayerObj +
                                pSdk->CPlayerObj_m_ePlayerState);
    if (aData->bCoop && !pPlData->bIsDead &&
        playerState == ePlayerState_Dying_Stage2 /*&& pSdk->checkPointState*/) {
      pPlData->bIsDead = 1;
      if (*(unsigned char *)((unsigned char *)pPlayerObj +
                             pSdk->CCharacter_m_bOnGround)) {
        // HCLIENT playerClient = *(HCLIENT*)((unsigned char
        // *)pPlayerObj+0x2880);
        HOBJECT hPlayerObj = pSdk->GetClientObject((HCLIENT)p->v0);
        pSdk->g_pLTServer->GetObjectPos(hPlayerObj, &pSdk->checkPointPos);
        pSdk->checkPointPos.y += 50.0f;
        // newPos = pSdk->checkPointPos;
        pSdk->checkPointState = 2;
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
  case MID_PLAYER_ACTIVATE: {
    if (!pPlayerObj) {
      p->state = 1;
      break;
    }
    CArsenal *pArsenal =
        (CArsenal *)((unsigned char *)pPlayerObj + pSdk->CPlayerObj_m_Arsenal);
    if (!pArsenal) {
      p->state = 1;
      break;
    }
    LTRotation rTrueCameraRot;
    LTVector curPos, objPos;
    pMsgRead->ReadData((void *)&objPos, 0x60);
    pMsgRead->ReadCompLTRotation(&rTrueCameraRot);
    unsigned type = pMsgRead->ReadBits(8);
    unsigned curTime = pSdk->getRealTimeMS();
    HOBJECT hObject = pMsgRead->ReadObject();
    bool bPosInvalid = 0;
    pSdk->g_pLTServer->GetObjectPos(hClientObj, &curPos);
    pSdk->g_pLTServer->GetObjectPos(hObject, &objPos);
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
                       pSdk->CArsenal_m_hCurWeapon)) {
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
                               bool))pSdk->CPlayerObj_ChangeWeapon)(
              pPlayerObj, pSdk->m_hUnarmedRecord, 1, pSdk->m_hUnarmedRecordAmmo,
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
        (CArsenal *)((unsigned char *)pPlayerObj + pSdk->CPlayerObj_m_Arsenal);
    if (!pArsenal) {
      p->state = 1;
      break;
    }
    pPlData->onChangeWeaponHWEAPON =
        *(HWEAPON *)((unsigned char *)pArsenal + pSdk->CArsenal_m_hCurWeapon);
    pPlData->onChangeWeaponTime = pSdk->getRealTimeMS();
    HWEAPON hWeapon =
        pMsgRead->ReadDatabaseRecord(pSdk->g_pLTDatabase, pSdk->m_hCatWeapons);
    HAMMO hAmmo =
        pMsgRead->ReadDatabaseRecord(pSdk->g_pLTDatabase, pSdk->m_hCatAmmo);
    HWEAPONDATA hWpnData = 0;
    if (hWeapon) {
      hWpnData = pSdk->GetWeaponData(hWeapon);
      HAMMO hAmmoServ = pSdk->g_pLTDatabase->GetRecordLink(
          pSdk->g_pLTDatabase->GetAttribute(hWpnData, _C("AmmoName")), 0, 0);
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
        (((unsigned char *)pPlayerObj + pSdk->CPlayerObj_m_Inventory));
    HGEAR m_hSlowMoGearRecord = *(HGEAR *)((unsigned char *)m_Inventory + 0x58);
    if (!aData->bCoop) { // everyone can use & no wait for charge
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
  case MID_PLAYER_RESPAWN: {

    memset(&pSdk->pPlayerData[clientId], 0, sizeof(playerData));
    p->state = pSdk->checkPlayerStatus(pGameClientData);
    if (p->state) {
      pSdk->g_pLTServer->KickClient((HCLIENT)p->v0);
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
    pSdk->g_pLTServer->GetObjectPos(hTarget, &targetPos);
    pSdk->g_pLTServer->GetObjectPos(hClientObj, &curPos);
    HOBJECT hFiredFrom = pSdk->HandleToObject(hTarget);
    if (hFiredFrom) {
      // HWEAPONDATA hWpnData = pSdk->GetWeaponData(hTarget);
      // HAMMO hAmmo = pSdk->g_pLTDatabase->GetRecordLink(
      //     pSdk->g_pLTDatabase->GetAttribute(hWpnData, "AmmoName"), 0, 0);
      //
      HAMMO hWeapon =
          *(void **)((uint8_t *)hFiredFrom + 0xDC); // 0xDC - weapon,0xE0 - ammo
      if (hWeapon) {
        HWEAPONDATA hWeaponData = pSdk->GetWeaponData(hWeapon);
        if (hWeaponData) {
          bool IsGrenade = ((bool(__thiscall *)(
              CWeaponDB *, HAMMODATA, const char *, uintptr_t,
              uintptr_t))pSdk->g_pWeaponDB_GetBool)(
              pSdk->g_pWeaponDB, hWeaponData, "IsGrenade", 0, 0);
          if (IsGrenade &&
              ((bool(__thiscall *)(void *, HOBJECT))
                   pSdk->CGrenadeProximity_IsEnemy)(hFiredFrom, hClientObj)) {
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
  } break;
  case MID_WEAPON_FIRE: {
    if (playerState != ePlayerState_Alive) {
      p->state = 1;
      break;
    }

    // unsigned char bIsNul=0;
    LTVector firePos, curPos, vPath;
    HWEAPON hWeapon = 0;
    CArsenal *pArsenal =
        (CArsenal *)((unsigned char *)pPlayerObj + pSdk->CPlayerObj_m_Arsenal);
    if (!pArsenal) {
      p->state = 1;
      break;
    }
    if (pMsgRead->ReadBits(0x1)) {
      hWeapon = pMsgRead->ReadDatabaseRecord(pSdk->g_pLTDatabase,
                                             pSdk->m_hCatWeapons);
      // bIsNul++;
    }
    HAMMO hAmmo = 0;
    if (pMsgRead->ReadBits(0x1)) {
      hAmmo =
          pMsgRead->ReadDatabaseRecord(pSdk->g_pLTDatabase, pSdk->m_hCatAmmo);
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
    if (hWeapon &&
        hash_rta((char *)*(uintptr_t *)(hWeapon)) == hash_ct("Unarmed")) {
      if (hAmmo) {
        unsigned hAnim = pPlData->hAnim;
        unsigned hAnimPenult = pPlData->hAnimPenult;
        if (pSdk->isXP2) {
          hAnim++;
          hAnimPenult++;
        }
//        if (!aData->bCustomSkins) {
//          switch (hash_rta((char *)*(uintptr_t *)(hAmmo))) {
//          case hash_ct("Melee_JabRight"):
//          case hash_ct("Melee_JabLeft"):
//            unarmFireDelay = 0x100;
//            if (hAnimPenult >= 0x10C && hAnimPenult <= 0x116)
//              break;
//            p->state = 1;
//            break;
//          case hash_ct("Melee_RifleButt"):
//            unarmFireDelay = 0x100;
//            // printf()
//            // if (hAnimPenult == 0x343 || hAnimPenult == 0x34C)
//            //     break;
//            //   p->state = 1;
//            break;
//          case hash_ct("Melee_SlideKick"):
//            unarmFireDelay = 0x16;
//            if (pSdk->isXP2) {
//              if (hAnim == 0x12C || hAnim == 0x47D)
//                break;
//              p->state = 1;
//            } else if (hAnim != 0x12B)
//              p->state = 1;
//            break;
//          case hash_ct("Melee_RunKickLeft"):
//          case hash_ct("Melee_RunKickRight"):
//            unarmFireDelay = 0x50;
//            if (hAnim != 0x12E)
//              p->state = 1;
//            break;
//          case hash_ct("Melee_JumpKick"):
//            unarmFireDelay = 0x30;
//            if (hAnim != 0x12D)
//              p->state = 1;
//            break;
//          default:
//            p->state = 1;
//            break;
//          }
//        }
        // if(p->state)
        // printf("fired: %s %p %p %p\n",(char *)*(uintptr_t
        // *)(hAmmo),hAnim,hAnimPenult,p->state);
      }
      dist = 150.0f;
      bUnarmed = 1;
    }
    if (!bUnarmed && hWeapon) {
      hWpnData = pSdk->GetWeaponData(hWeapon);
      HAMMO hAmmoServ = pSdk->g_pLTDatabase->GetRecordLink(
          pSdk->g_pLTDatabase->GetAttribute(hWpnData, "AmmoName"), 0, 0);
      unsigned ammoCnt = ((unsigned(__thiscall *)(
          CArsenal *, HAMMO))pSdk->CArsenal_GetAmmoCount)(pArsenal, hAmmoServ);
      if (!ammoCnt)
        p->state = 1;
      if (hAmmo && hAmmoServ != hAmmo)
        p->state = 1;
    }
    hWeapon =
        *(HWEAPON *)((unsigned char *)pArsenal + pSdk->CArsenal_m_hCurWeapon);
    if (!hWeapon) {
      p->state = 1;
      break;
    }
    if (!hAmmo) {
      hWpnData = pSdk->GetWeaponData(hWeapon);
      hAmmo = pSdk->g_pLTDatabase->GetRecordLink(
          pSdk->g_pLTDatabase->GetAttribute(hWpnData, "AmmoName"), 0, 0);
      HAMMODATA hAmmoData = pSdk->GetAmmoData(hAmmo);
      ammoType = ((unsigned(__thiscall *)(
          CWeaponDB *, HAMMODATA, const char *, uintptr_t,
          uintptr_t))pSdk->g_pWeaponDB_GetInt32)(pSdk->g_pWeaponDB, hAmmoData,
                                                 "Type", 0, 0);
    }
    if ((hWeapon && hash_rta((char *)*(uintptr_t *)(hWeapon)) ==
                        hash_ct("Turret_Remote")) ||
        ammoType == TRIGGER)
      break;

    pMsgRead->ReadData((void *)&firePos, 0x60);
    if (firePos.x == 0.0 && firePos.y == 0.0 && firePos.z == 0.0)
      p->state = 1;
    if (isnan(firePos.x) && isnan(firePos.y) || isnan(firePos.z))
      p->state = 1;
    pMsgRead->ReadData((void *)&vPath, 0x60);
    if (vPath.x == 0.0 && vPath.y == 0.0 && vPath.z == 0.0)
      p->state = 1;
    if (isnan(vPath.x) && isnan(vPath.y) || isnan(vPath.z))
      p->state = 1;
    pMsgRead->ReadBits(8); // random number seed
    pMsgRead->ReadBits(8); // perturb count
    pMsgRead->ReadBits(8); // perturb
    unsigned fireTimestamp = pMsgRead->ReadBits(0x20);
    unsigned fireServTimestamp = pSdk->getRealTimeMS();
    // printf("fire %p %p\n",fireTimestamp,fireServTimestamp);

    bool bFire = 1;
    // pSdk->g_pLTServer->GetObjectPos(hTarget,&targetPos);
    pSdk->g_pLTServer->GetObjectPos(hClientObj, &curPos);

    if (!curPos.NearlyEquals(firePos, dist)) {
      p->state = 1;
    }

    if (p->state) {
      if (hAmmo)
        ((unsigned(__thiscall *)(
            CArsenal *, HAMMO))pSdk->CArsenal_DecrementAmmo)(pArsenal, hAmmo);
      break;
    }

    // if ((hWeapon && hash_rta((char *)*(uintptr_t *)(hWeapon)) ==
    //                    hash_ct("Minigun")))
    //  break;
    CWeapon *pWeapon = ((CWeapon * (__thiscall *)(CArsenal *))
                            pSdk->CArsenal_GetCurWeapon)(pArsenal);
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
        pPlData->lastFireWeaponIgnored = 0;
      }
      p->state = 1;
      if (bUnarmed || (ammoInClip != pWeapon->m_nAmmoInClip)) {
        ammoInClip = pWeapon->m_nAmmoInClip;
        if (pPlData->lastFireWeapon != pWeapon) {
          pPlData->lastFireWeaponClipAmmo = 0;
          pPlData->lastFireWeaponTimestamp = 0;
          pPlData->lastFireWeaponReload = 0;
          pPlData->lastFireWeaponAccuracyFailCnt = 0;
          pPlData->lastFireWeaponAccuracyFail = 0;
          pPlData->lastFireWeapon = pWeapon;
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
          if (bUnarmed && (fireTimestamp - pPlData->lastFireWeaponTimestamp) <
                              unarmFireDelay) { // original unarm FireDelay is 0
                                                // so we add extra check
            p->state = 0;
            pPlData->lastFireWeaponClipAmmo = 0;
            pPlData->lastFireWeaponTimestamp = 0;
            pPlData->lastFireWeaponIgnored = fireServTimestamp;
            break;
          }
        }
        pPlData->lastFireWeaponClipAmmo = ammoInClip;
        pPlData->lastFireWeaponTimestamp = fireTimestamp;
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
        pMsgRead->ReadDatabaseRecord(pSdk->g_pLTDatabase, pSdk->m_hCatGear);
    if (hGear && hash_rta((char *)*(uintptr_t *)(hGear)) != hash_ct("MedKit"))
      p->state = 1;
    break;
  } break;
  case MID_DROP_GRENADE: {
    if (pPlayerObj && (playerState == ePlayerState_Dying_Stage1 ||
                       playerState == ePlayerState_Dying_Stage2)) {
      pMsgRead->ReadDatabaseRecord(pSdk->g_pLTDatabase, pSdk->m_hCatWeapons);
      HAMMO hAmmo =
          pMsgRead->ReadDatabaseRecord(pSdk->g_pLTDatabase, pSdk->m_hCatAmmo);
      if (hAmmo && hash_rta((char *)*(uintptr_t *)(hAmmo)) == hash_ct("Frag")) {
        CArsenal *pArsenal = (CArsenal *)((unsigned char *)pPlayerObj +
                                          pSdk->CPlayerObj_m_Arsenal);
        if (!pArsenal) {
          p->state = 1;
          break;
        }
        unsigned nAmmo = ((unsigned(__thiscall *)(
            CArsenal *, HAMMO))pSdk->CArsenal_GetAmmoCount)(pArsenal, hAmmo);
        ((unsigned(__thiscall *)(
            CArsenal *, HAMMO, int))pSdk->CArsenal_SetAmmo)(pArsenal, hAmmo, 0);
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
          if (isnan(firePos.x) && isnan(firePos.y) || isnan(firePos.z))
            p->state = 1;
          pSdk->g_pLTServer->GetObjectPos(hClientObj, &curPos);
          if (!curPos.NearlyEquals(firePos, 128.0f)) {
            p->state = 1;
          }
          pMsgRead->ReadData((void *)&firePos, 0x60);
          if (firePos.x == 0.0 && firePos.y == 0.0 && firePos.z == 0.0)
            p->state = 1;
          if (isnan(firePos.x) && isnan(firePos.y) || isnan(firePos.z))
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
//    unsigned timeMS = pSdk->getRealTimeMS();
//    pPlData->thisMoveTimeMS = pMsgRead->ReadBits(0x20);
//    if (pPlData->thisMoveTimeMS >
//        timeMS) // ignore messages that claim they from future
//      p->state = 1;
    uint16_t nClientChangeFlags = pMsgRead->ReadBits(8);
    if (nClientChangeFlags & CLIENTUPDATE_CAMERAINFO) {
      LTRotation rTrueCameraRot;
      pMsgRead->ReadCompLTRotation(&rTrueCameraRot);
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
//          if (hWeight > 4) {
//            DBGLOG("weight set %d", hWeight)
//            pSdk->BootWithReason(pGameClientData,
//                                 eClientConnectionError_PunkBuster,
//                                 (char *)"error 1");
//            p->state = 1;
//            break;
//          }
        }
        bool bEnabled = pMsgRead->ReadBits(1); // tracker info
        if (bPlayerBody && bEnabled) {
          unsigned hAnim = pMsgRead->ReadBits(0x20); // hAnim
          if (!aData->bCustomSkins &&
              (hAnim >= (65 - pSdk->isXP2) && hAnim <= (107 - pSdk->isXP2)))
            p->state = 1;
          // printf("anim: %p\n",hAnim);
          pPlData->hAnimPenult = pPlData->hAnim;
          pPlData->hAnim = hAnim;
          /*if(!hAnim){
            DBGLOG("hAnim is ZERO")
              pSdk->g_pLTServer->KickClient((HCLIENT)p->v0);
              p->state=1;
          }*/
          bool bSetTime = pMsgRead->ReadBits(1);
          if (bSetTime) {
            unsigned n = pMsgRead->ReadBits(0x20);
//            if (n > 0xFFFF) {
//              DBGLOG("%d > 0xFFFF", n)
//              pSdk->BootWithReason(pGameClientData,
//                                   eClientConnectionError_PunkBuster,
//                                   (char *)"error 2");
//              p->state = 1;
//              break;
//            }
          }
          bool bLooping = pMsgRead->ReadBits(1);
          bool bSetRate = pMsgRead->ReadBits(1);
          if (bSetRate) {
            unsigned var = pMsgRead->ReadBits(0x20);
            float fRate = reinterpret_cast<float &>(var);
            if (fRate < 0.0f) {
              p->state = 1;
              break;
            }
          }
        }
      }
    }
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
        pSdk->BootWithReason(pGameClientData, eClientConnectionError_PunkBuster,
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
      if (!ret) {
        p->state = 1;
        pSdk->BootWithReason(pGameClientData, eClientConnectionError_BadCDKey,
                             (char *)0);
      }
      {
        uint8_t aTcpIp[4];
        uint16_t nPort;
        pSdk->getClientAddr((HCLIENT)p->v0, aTcpIp, &nPort);
        IPData block = {*(uint32_t *)aTcpIp, nPort};
        EnterCriticalSection(&pSdk->g_ipchunkSection);
        IPChunk::foreach (
            pSdk->g_ipchunk, &block,
            [](uintptr_t index, IPChunk *Chunk, IPData *Block) -> uintptr_t {
              if (Chunk->buf[index].ip == Block->ip) {
                Chunk->buf[index].ip = 0;
                return 1;
              }
              return 0;
            });
        LeaveCriticalSection(&pSdk->g_ipchunkSection);
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
      pSdk->BootWithReason(pGameClientData, eClientConnectionError_PunkBuster,
                           (char *)"Invalid player name");
      p->state = 1;
    }
    unsigned skinIndex = pMsgRead->ReadBits(8);
    unsigned skinCount = 0;
    if (*(char *)(((GameModeMgr * (*)()) pSdk->g_pGameModeMgr_instance)() +
                  0x18C) /*m_grbUseTeams*/) {
      skinCount =
          pSdk->g_pLTDatabase->GetNumValues(pSdk->g_pLTDatabase->GetAttribute(
              pSdk->m_hGlobalRecord, "TeamModel"));
    } else {
      skinCount =
          pSdk->g_pLTDatabase->GetNumValues(pSdk->g_pLTDatabase->GetAttribute(
              pSdk->m_hGlobalRecord, "DeathmatchModels"));
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
    unsigned voteTime = pSdk->getRealTimeMS();

    if (pMsgRead->ReadBits(3) == eVote_Start) {
      p->state = pSdk->checkPlayerStatus(pGameClientData);
      if (pPlData->lastVoteTime) {
        /*ServerSettings * pServSettings = (ServerSettings *)((unsigned char
         * *)(((GameModeMgr * (*)()) pSdk->g_pGameModeMgr_instance)() +
         * pSdk->GameModeMgr_ServerSettings));*/
        unsigned voteDelay = 3000; //(*(uint8_t*)(pServSettings+0x100)*1000);
        // if(voteDelay)
        // voteDelay-=250; //server timer may lag with client
        unsigned delta = voteTime - pPlData->lastVoteTime;
        if (delta < voteDelay) {
          p->state = 1;
          break;
        }
      }
      pPlData->lastVoteTime = voteTime;
      unsigned char type = pMsgRead->ReadBits(3);
      if (type == eVote_SelectMap) {
        unsigned nMapIndex = pMsgRead->ReadBits(0x20);
        CServerMissionMgr *g_pServerMissionMgr = *pSdk->g_pServerMissionMgrAdr;
        unsigned nMaps = ((uintptr_t)g_pServerMissionMgr->m_CampaignEnd -
                          (uintptr_t)g_pServerMissionMgr->m_CampaignBegin) /
                         sizeof(uintptr_t);
        if (nMapIndex >= nMaps)
          p->state = 1;
      } else if (type == eVote_TeamKick) {
        unsigned nTargetId = pMsgRead->ReadBits(0x20);
        HCLIENT hTargetClient = pSdk->g_pLTServer->GetClientHandle(nTargetId);
        GameClientData *pGameTargetData =
            pSdk->getGameClientData(hTargetClient);
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
    if (type == pSdk->ukPENextSpawnPoint || type == pSdk->ukPEPrevSpawnPoint)
      p->state = 1;
  } break;
  case MID_PLAYER_CLIENTMSG: {
    unsigned char CP = pMsgRead->ReadBits(8);
    switch (CP) {
    case CP_DAMAGE:
    case CP_DAMAGE_VEHICLE_IMPACT:
      if (CP == CP_DAMAGE &&
          /*dmgId*/ pMsgRead->ReadBits(8) != pSdk->uDT_CRUSH)
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
      if (!aData->bCoop)
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

void regcall patchClientServer(unsigned char *mod, unsigned modSz) {
  // limit loop (synch) to 2^16 coz it may cause infinte loop
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)mod, modSz,
                  BYTES_SEARCH_FORMAT(
                      "DFE0F6C4057A2BC784248400000000000000C744241C00000000C744"
                      "241800000000C744241000000000C74424140000803F"));
    if (tmp) {
      patchData::codeswap(
          (unsigned char *)tmp,
          (unsigned char *)(const unsigned char[]){
              0x66, 0xC7, 0x44, 0x24, 0x58, 0x00, 0x00, 0x90, 0x90, 0x90,
              0x90, 0x90, 0xDF, 0xE0, 0xF6, 0xC4, 0x05, 0x7A, 0x1F, 0xC7,
              0x44, 0x24, 0x14, 0x00, 0x00, 0x80, 0x3F, 0x31, 0xC0, 0x89,
              0x44, 0x24, 0x1C, 0x89, 0x44, 0x24, 0x18, 0x89, 0x44, 0x24,
              0x10, 0x31, 0xC0, 0x89, 0x84, 0x24, 0x84, 0x00, 0x00, 0x00},
          50);
    }
  }
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)mod, modSz,
                  BYTES_SEARCH_FORMAT("C784248400000000000000E9F5FEFFFF"));
    if (tmp) {
      patchData::codeswap((unsigned char *)tmp,
                          (unsigned char *)(const unsigned char[]){
                              0x66, 0xFF, 0x44, 0x24, 0x58, 0x74, 0x09, 0x90,
                              0x90, 0x90, 0x90, 0xE9, 0xE9, 0xFE, 0xFF, 0xFF},
                          16);
    }
  }
}

void regcall hookReadProps(reg *p) {
  fearData *pSdk = &handleData::instance()->pSdk;
  p->argcnt = 1;
  p->state = 1;
  unsigned char *tmp =
      ((unsigned char *(__thiscall *)(uintptr_t, uintptr_t))p->hook)(p->tcx,
                                                                     p->v0);
  p->tax = (uintptr_t)tmp;
  if (!tmp)
    return;
  switch (hash_rta((char *)p->v0)) {
  case hash_ct("MPClientOnly"):
    *(tmp + 0x28) = 0;
    break;
  case hash_ct("BoxPhysics"):
    //*(tmp+0x28)=1;
    break;
  case hash_ct("MPClientOnlyRigidBody"):
    *(tmp + 0x28) = 0;
    break;
  }
}

/*void regcall hookSetObjFlags (reg *p){
  p->state=2;
  uintptr_t * pObj = (uintptr_t*)p->tsi;
  if((pObj[1] & FLAG2_RIGIDBODY) || (pObj[1] & FLAG2_CLIENTRIGIDBODY))
    pObj[1]=0x71;
  p->tdx=0x40;
}*/

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

void regcall hookSetObjFlags(reg *p) {
  fearData *pSdk = &handleData::instance()->pSdk;
  uintptr_t *pObj = (uintptr_t *)p->tsi;
  if (pObj) {
    char *objName =
        (char *)((unsigned char *)pObj + pSdk->ObjectCreateStruct_m_Name);
    // if((pObj[1] & FLAG2_RIGIDBODY) || (pObj[1] & FLAG2_CLIENTRIGIDBODY)){
    bool isMatch = 0;

    // if (starCmp(objName, (char *)"library_chair*.Chair Physics"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"Crate-CardBoard*.Box"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"Table-Wall-Cover*.SlideTableAWM"))
    //   isMatch = 1;
    if (starCmp(objName, (char *)"VendingMachine*.Base"))
      isMatch = 1;
    if (starCmp(objName, (char *)"VendingMachine*.glow"))
      isMatch = 1;
    if (starCmp(objName, (char *)"Table_Utililty_*.Base"))
      isMatch = 1;
    if (starCmp(objName, (char *)"Table_Utililty_*.glow"))
      isMatch = 1;
    if (starCmp(objName, (char *)"industrial_fridge*.Base"))
      isMatch = 1;
    if (starCmp(objName, (char *)"industrial_fridge*.glow"))
      isMatch = 1;
    // if (starCmp(objName, (char *)"Barrel*.WorldModel00"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"Crate_*.WorldModel00"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"vase_*.Vase"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"vase_*.vase_top"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"vase_*.vase_bottom"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"box_single*.Box"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"box_single*.WorldModel00"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"TrafficBarrel*.TrafficBarrel"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"Bucket*.WorldModel00"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"Bucket*.Bucket"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"mopbucket_trans*.Bucket"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"junk_cone*.cone"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"decor_TrashCan*.trashcan"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"Cactus*.Cactus"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"Ind_Trashcan*.WorldModel00"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"decor_TrashCan*.trashcan"))
    //   isMatch = 1;
    if (starCmp(objName, (char *)"Sofa-2Seat*_Flip*.Sofa2"))
      isMatch = 1;
    // if (starCmp(objName, (char *)"GarbageCan*.WorldModel*"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"trash_can*.WorldModel*"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"chair_dress*.ChairPhysics"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"chair*.ChairPhysics"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"apt_TV*.WorldModel00"))
    //   isMatch = 1;
    // if (starCmp(objName, (char *)"apt_TV*.WorldModel00"))
    //   isMatch = 1;

    if (isMatch) {
      // pObj[1]=0x71;
      pObj[2] = 0x40;
    }
    // switch (hash_rta(objName)) {
    // // case hash_ct("GarbageCan0100.WorldModel01"):
    // case hash_ct("40oz00.WorldModel00"):
    //   // case hash_ct("Window-LobbyLrg00.WorldModel00"):
    //   // hhcase hash_ct("Window-LobbyLrg01.WorldModel00"):
    //   // pObj[1]=0x71;
    //   pObj[2] = 0x40;
    //   break;
    //   break;
    // }
    /*switch(hash_rta(objName)){
case hash_ct("Barrel101.WorldModel00"):
case hash_ct("Barrel207.WorldModel00"):
case hash_ct("Barrel214.WorldModel00"):
case hash_ct("box_single0200.WorldModel00"):
case hash_ct("TrafficBarrel01.TrafficBarrel"):
case hash_ct("Bucket200.WorldModel00"):
case hash_ct("Bucket00.Bucket"):
case hash_ct("mopbucket_trans0100.Bucket"):
case hash_ct("junk_cone100.cone"):
case hash_ct("junk_cone0100.cone"):
case hash_ct("decor_TrashCan0100.trashcan"):
case hash_ct("Cactus00.Cactus"):
case hash_ct("Cactus100.Cactus"):
case hash_ct("vase_100.Vase"):
case hash_ct("vase_101.Vase"):
case hash_ct("vase_102.Vase"):
case hash_ct("vase_0100.Vase"):
case hash_ct("vase_103.Vase"):
case hash_ct("Ind_Trashcan00.WorldModel00"):
case hash_ct("Ind_Trashcan01.WorldModel00"):
case hash_ct("Ind_Trashcan02.WorldModel00"):
case hash_ct("Ind_Trashcan03.WorldModel00"):
case hash_ct("Sofa-2Seat01_Flip.Sofa2"):
case hash_ct("Sofa-2Seat01_Flip00.Sofa2"):
case hash_ct("apt_TV01_static00.WorldModel00"):
case hash_ct("GarbageCan0100.WorldModel01"):
case hash_ct("Table_Utililty_101.Base"):
case hash_ct("Table_Utililty_102.Base"):
case hash_ct("VendingMachine0200.Base"):

      pObj[1]=0x71;
      pObj[2]=0x40;
      break;
case hash_ct("Powerbox_Explosive0100.Powerbox_EXP_door"):
case hash_ct("Powerbox_Explosive100.Powerbox_EXP_door"):
      pObj[1]=0x31;
      pObj[2]=0x80;
break;
    }*/
    //}
  }
}

void regcall hookReloadWeapon(reg *p) {
  fearData *pSdk = &handleData::instance()->pSdk;
  CWeapon *pWeapon = (CWeapon *)p->tcx;
  if (pWeapon) {
    CArsenal *pArsenal = pWeapon->m_pArsenal;
    if (pArsenal) {
      CPlayerObj *pPlayerObj = *(CPlayerObj **)((unsigned char *)pArsenal +
                                                pSdk->CArsenal_m_pPlayer);
      if (pPlayerObj) {
        HCLIENT hClient = (HCLIENT) * (HCLIENT *)((unsigned char *)pPlayerObj +
                                                  pSdk->CPlayerObj_m_hClient);
        unsigned clientId = pSdk->g_pLTServer->GetClientID(hClient);
        playerData *pPlData = &pSdk->pPlayerData[clientId];
        if (p->v0 == 1 && (pPlData->lastFireWeapon == pWeapon)) {
          pPlData->lastFireWeaponReload = 1;
        }
      }
    }
  }
}

void regcall hookComposeArchives(reg *p) {
  char *str = (char *)p->tdi;
  if (str) {
    if (strstr(str, "AdditionalContent")) {
      p->tcx = p->tcx | 1;
    }
  }
}

void appData::configHandle() {
  // DBGLOG("init!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
  SetPriorityClass((HANDLE)-1, HIGH_PRIORITY_CLASS);
  unsigned char moveax0[] = {0xB8, 0x00, 0x00, 0x00, 0x00};
  pSdk->fearDataInitServ();
  patchClientServer(gEServer, gEServerSz);

  {
    unsigned char *tmp = scanBytes(
        (unsigned char *)gServer, gServerSz,
        BYTES_SEARCH_FORMAT("8D4C24??51505756FF15????????83C410C6????????5F"));
    if (tmp) {
      unsigned char *pfunc = (unsigned char *)*(uintptr_t *)(tmp + 10);
      patchData::addCode(tmp + 8, 6);
      *(unsigned short *)(tmp + 8) = 0xE890;
      *(uintptr_t *)(tmp + 10) = getRel4FromVal(
          (tmp + 10), (unsigned char *)(void *)*(uintptr_t *)pfunc);
      patchData::addCode(pfunc, 4);
      void *func = (void *)GetProcAddress(GetModuleHandle(_T("ntdll.dll")),
                                          _C("_vsnprintf"));
      if (func)
        *(uintptr_t *)(pfunc) = (uintptr_t)func;
    }
  }
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  BYTES_SEARCH_FORMAT("508D442430508B430850FF524084C0"));
    if (tmp) {
      patchData::addCode(tmp + 15, 1); // allow connect with invalid assets
      *(tmp + 15) = 0xEB;
    }
  }
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  BYTES_SEARCH_FORMAT(
                      "FF5204E8????????8A??????????05")); // disable punkbuster
    if (tmp) {
      patchData::addCode(tmp, 4);
      *(unsigned *)(tmp) = 0xE8909090;
    }
  }
  {
    unsigned char *tmp = scanBytes(
        (unsigned char *)gEServer, gEServerSz,
        BYTES_SEARCH_FORMAT(
            "745D8B????????????8B????8B??C1????884C2414")); // show conn client
                                                            // ip
    if (tmp) {
      spliceUp(tmp - 7, (void *)fearData::hookUDPConnReq);
      patchData::addCode(tmp, 2);
      *(unsigned short *)(tmp) = 0x9090;
    }
  }
  {
    unsigned char *tmp = scanBytes(
        (unsigned char *)gEServer, gEServerSz,
        BYTES_SEARCH_FORMAT(
            "74608B??????????8B??C1????88??????33")); // show disco client ip
    if (tmp) {
      patchData::addCode(tmp, 2);
      *(unsigned short *)(tmp) = 0x9090;
    }
  }
  {
    unsigned char *tmp = scanBytes(
        (unsigned char *)gEServer, gEServerSz,
        BYTES_SEARCH_FORMAT(
            "8B5E0C81FB80000000732E8B06578B7C2410")); // FindObjects spam
    if (tmp) {
      patchData::addCode(tmp + 9, 2);
      *(unsigned short *)(tmp + 9) = 0x9090;
    }
  }
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  BYTES_SEARCH_FORMAT("750980E1??888E????????B001")); // combo
                                                                      // death
                                                                      // fix
    if (tmp) {
      patchData::codeswap((unsigned char *)tmp,
                          (unsigned char *)(const unsigned char[]){0xEB}, 1);
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
    unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  BYTES_SEARCH_FORMAT("8A42??84C076??33C90FB6D0"));
    if (tmp) {
      patchData::addCode((unsigned char *)tmp + 5, 1);
      *(tmp + 5) =
          0xEB; // disable cleaning of ammo quantity (for MID_DROP_GRENADE)
    }
  }
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gEServer, gEServerSz,
                  BYTES_SEARCH_FORMAT(
                      "83C404F6????74??8B??E8????????6A00E8????????8B??E8"));
    if (tmp) {
      spliceUp(tmp, (void *)fearData::hookOnMapLoaded);
    }
  }
  {
    unsigned char *tmp = scanBytes((unsigned char *)gEServer, gEServerSz,
                                   BYTES_SEARCH_FORMAT("7402893783FEFF7472"));
    if (tmp) {
      spliceUp(tmp, (void *)fearData::hookUDPRecvfrom);
    }
  }
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gEServer, gEServerSz,
                  BYTES_SEARCH_FORMAT("8B168BCEFF1284C0740E6A016A03568BCF"));
    if (tmp) {
      spliceUp(tmp, (void *)fearData::hookCheckUDPDisconnect);
    }
  }
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  BYTES_SEARCH_FORMAT("3B7C2410741F8B4E086A006A0351"));
    if (tmp) {
      patchData::addCode((unsigned char *)tmp + 4,
                         1); // ignore invalid world crc
      *(tmp + 4) = 0xEB;
    }
  }
  if (bRandWep) {

    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT("6A005256FF90??0000008BF085F60F84????????8B"));
      if (tmp) {
        spliceUp(tmp, (void *)hookRandomWeapon);
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    BYTES_SEARCH_FORMAT("8B0D????????8B168BF88B41??50A1????????"
                                        "508BCEFF52??8B168BD86A20"));
      if (tmp) {
        spliceUp(tmp, (void *)hookPickupRndWeapon);
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
      unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT("83EC10568B74241885F6578BF9")));
      if (tmp) {
        spliceUp(tmp, (void *)hookEnterSlowMo);
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
  if (bCoop) {

    bPreventNoclip = 2;
    // bIgnoreUnusedMsgID=0;
    bSyncObjects = 1;
    bBotsMP = 1;
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT("753F8D??????E8????????8B??????????8B"));
      if (tmp) {
        patchData::addCode((unsigned char *)tmp, 1);
        *(tmp) = 0xEB; // dont crush players on player
        // memcpy(tmp,moveax0,5);
      }
    }
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT(
              "8B??????85????8B??74??8B??????????85??74??3B??74??68")); // story
                                                                        // mode
                                                                        // on
      if (tmp) {
        spliceUp(tmp, (void *)hookStoryModeOn);
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    BYTES_SEARCH_FORMAT(
                        "8B??8B??????????85??75??68????????6A00E8????????"));
      if (tmp) {
        spliceUp(tmp, (void *)hookStoryModeOff);
      }
    }
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT(
              "E8????????84C074??8D??????68??????????E8")); // use SP MAPS
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    if (bCoop == 1) {
      spliceUp(
          scanBytes((unsigned char *)gServer, gServerSz,
                    BYTES_SEARCH_FORMAT(
                        "6A00????FF??????????8B??85??74??E8????????84??74")),
          (void *)fearData::hookLoadMaps1);
      spliceUp(scanBytes((unsigned char *)gServer, gServerSz,
                         BYTES_SEARCH_FORMAT(
                             "5357??FF??????????3B??8B??0F??????????8B???????"
                             "???8B??68??????????FF????8B????????????FF")),
               (void *)fearData::hookLoadMaps2);

      /*{
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"75??8B????8B????8B??FF??????????8B????8B");
      if (tmp) {
        spliceUp(tmp, (void *)fearData::hookUseSkin1);
      }
    }*/
    } else if (bCoop == 2) {
      spliceUp(
          scanBytes((unsigned char *)gEServer, gEServerSz,
                    BYTES_SEARCH_FORMAT("88861C0200008B44243C888E1D020000")),
          (void *)hookComposeArchives);
    }

    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT("8B??????????F6????74????8D??????????8B"));
      if (tmp) {
        unprotectCode(tmp);
        *(tmp + 9) = 0xEB;
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    BYTES_SEARCH_FORMAT("83E0FB68????????8B??89"));
      if (tmp) {
        unprotectCode(tmp);
        *(unsigned short *)(tmp + 1) = 0x0BC8;
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    BYTES_SEARCH_FORMAT("8B??????83????3B????75??8B??E8????????"
                                        "??????83????C3")); // cutscene
                                                            // player
                                                            // numbers
      if (tmp) {
        unprotectCode(tmp);
        *(unsigned short *)(tmp + 10) = 0x9090;
      }
    }
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT(
              "8A5C2408F6DB565768000100008BF9")); // CANACTIVATE always 1
      if (tmp) {
        patchData::addCode((unsigned char *)tmp, 4);
        *(unsigned *)(tmp) = 0x909001B3;
      }
    }

    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT(
              "D9????EB??D9??????8B??????????D9??????????89??????68????????8B??"
              "E8????????85??74??83??0375??D9??24EB??D9????0C"));
      if (tmp) {
        // unprotectCode(tmp);

        //*(unsigned short *)(tmp+1)=0x90E8;
        spliceUp(tmp - 14, (void *)hookDoorTimer);
      }
    }

    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT("EB??D9????0C8B??????????D9??????????68????????"
                              "8B??89??????E8????????85??74??83????75??D9????"
                              "EB??D9??????68????????D9??????????8B"));
      if (tmp) {
        spliceUp(tmp + 29, (void *)hookDoorTimer);
      }
    }
    {
      unsigned char *tmp = gServer;

      unsigned i = 0;
      while (true) {
        tmp = scanBytes((unsigned char *)tmp, (gServerSz + gServer) - tmp,
                        BYTES_SEARCH_FORMAT(
                            "D9??24EB??D9????0C8B??????????D9??????????68"));
        if (tmp) {
          spliceUp(tmp - 14, (void *)hookDoorTimer);
        }
        tmp++;
        i++;
        if (i == 2)
          break;
      }
    }
    {
      unsigned char *tmp = gServer;

      unsigned i = 0;
      while (true) {
        tmp = scanBytes((unsigned char *)tmp, (gServerSz + gServer) - tmp,
                        BYTES_SEARCH_FORMAT(
                            "508B??FF??????????8B??E8????????88??????????8A"));
        if (tmp) {
          spliceUp(tmp, (void *)fearData::hookUseSkin1);
        }
        tmp++;
        i++;
        if (i == 2)
          break;
      }
    }
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT("75??8B??????????68????????E8??????????8B??FF"));
      if (tmp) {
        unprotectCode(tmp);
        *(unsigned short *)(tmp) = 0x9090;
        aSkinStr = (char **)(tmp + 9);
        *(uintptr_t *)(tmp + 9) = (uintptr_t) "Player";
      }
    }
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT(
              "75??E8????????84??74??8B??????????8B??6A006A0068??????????FF"));
      if (tmp) {
        tmp += 2;
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT("75??E8????????84??74??8B??????????8B??6A00"));
      if (tmp) {
        tmp += 2;
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }

    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT(
              "E8????????84C074??8A??????????84C075??83??????75??8B"));
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    BYTES_SEARCH_FORMAT("74??0F????8B??????????89??????????EB??"
                                        "A1????????C7??????????010000008B"));
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp,
               (unsigned char *)(const unsigned char[]){0xB8, 0x03, 0x00, 0x00,
                                                        0x00},
               5);
      }
    }

    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT(
              "8B??85??????????????????C7??????????????C7??????????????74"));
      if (tmp) {
        spliceUp(tmp, (void *)fearData::hookRespawnStartPoint);
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    BYTES_SEARCH_FORMAT("A1????????8B??????????8B????????????"
                                        "8B????????????E8????????C3"));
      if (tmp) {
        spliceUp(tmp, (void *)fearData::hookCheckpointEvent);
      }
    }
    {
      unsigned char *tmp = gServer;
      tmp = scanBytes((unsigned char *)tmp, (gServerSz + gServer) - tmp,
                      BYTES_SEARCH_FORMAT("8A8424A000000084C074"));
      tmp += 9;
      unprotectCode(tmp);
      *(unsigned short *)(tmp) = 0x9090;
      tmp = scanBytes((unsigned char *)tmp, (gServerSz + gServer) - tmp,
                      BYTES_SEARCH_FORMAT("8A8424A000000084C074"));
      tmp += 9;
      unprotectCode(tmp);
      *(unsigned short *)(tmp) = 0x9090;
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    BYTES_SEARCH_FORMAT(
                        "56E8????????8B??????????E8????????8B??????????8B"));
      if (tmp) {
        unprotectCode(tmp);
        *(uintptr_t *)(tmp) = 0x90c301b0;
      }
    }
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT("8B????????????8D????????????83C4183B??74"));
      if (tmp) {
        spliceUp(tmp + 7, (void *)hookGameMode);
      }
    }
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT(
              "E8????????84C074??8B0D????????68????????E8????????D9"));
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    BYTES_SEARCH_FORMAT(
                        "E8????????84C08B??????????74??68????????EB??68"));
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    BYTES_SEARCH_FORMAT(
                        "E8????????84C075??8B??????????8B0D??????????6A01E8"));
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    BYTES_SEARCH_FORMAT("E8????????84C08B??????????74??68??????"
                                        "??EB??68????????E8????????83EC08"));
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT("E8????????84C074??8B??????????6A????6A00E8"));
      if (tmp) {
        unprotectCode(tmp);
        aSPModeSpawn = tmp;
        setCoopDoSpawn(1);
        // memcpy(tmp,moveax0,5);
      }
    }
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT("75??8D??????E8????????8B????8B????8B????89"));
      if (tmp) {
        aPatchHoleKillHives = tmp - 5;
        spliceUp(aPatchHoleKillHives, (void *)fearData::hookPatchHoleKillHives);
      }
    }
  }
  if (bBotsMP) {
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT(
              "E8????????84C074??8B??8B??FF??????????84??74??E8????????8A"));
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    BYTES_SEARCH_FORMAT("E8????????84??0F??????????E8????????"
                                        "8A??????????05????????84??74??8B"));
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
  }
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  BYTES_SEARCH_FORMAT("84C07408??8B??E8????????????C20800"));
    if (tmp) {
      // unprotectCode(tmp);
      patchData::addCode(tmp + 8, 4);
      *(unsigned *)(tmp + 8) = getRel4FromVal(
          (tmp + 8),
          (unsigned char *)(void *)CPlayerObj::handlePlayerPositionMessage);
    }
  }
  if (bPreventNoclip) {
    {
      if (!bCoop && pSdk->CPlayerObj_UpdateMovement) {
        unsigned char *tmp = (unsigned char *)pSdk->CPlayerObj_UpdateMovement;
        // unprotectCode(tmp);
        patchData::addCode(tmp, 5);
        *(unsigned *)(tmp + 1) = getRel4FromVal(
            (tmp + 1), (unsigned char *)(void *)CPlayerObj::updateMovement);
        *(unsigned char *)(tmp) = 0xE9;
      }
    }

    if (bPreventNoclip == 2) {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          BYTES_SEARCH_FORMAT(
              "8BCFFF90????????3BC3A3????????7410C705????????????????893D??????"
              "??391D????????75688B0D????????891D????????891D????????891D??????"
              "??8B1168????????8BF9FF92????????3BC3A3????????75??8B0753"));
      if (tmp) {
        tmp += 82;
        patchData::codeswap(
            (unsigned char *)tmp,
            (unsigned char *)(const unsigned char[]){
                0xB9, 0x00, 0x00, 0x80, 0x3F, 0x90, 0x90, 0x8B, 0x07, 0x51},
            10);
      }
    } else {
      pSdk->freeMovement = 1;
    }
  }

  //*(uintptr_t *)tmp = 0x900008C2;
  unsigned char *StringToDamageType =
      scanBytes((unsigned char *)gServer, gServerSz,
                BYTES_SEARCH_FORMAT("538B????????????8B????????33FF??8B"));
  if (StringToDamageType) {
    pSdk->uDT_CRUSH =
        ((unsigned(__cdecl *)(char *))StringToDamageType)((char *)"CRUSH");
    if (pSdk->uDT_CRUSH != 4)
      pSdk->isXP2 = 1;
  }
  // spliceUp(scanBytes((unsigned char *)gServer, gServerSz,(char
  // *)"83????3D????????0F??????????0F"),(void *)hookMID_);
  spliceUp(scanBytes((unsigned char *)gServer, gServerSz,
                     BYTES_SEARCH_FORMAT(
                         "53568B74????85F68BD90F??????????578B??????56")),
           (void *)hookMID);
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  BYTES_SEARCH_FORMAT("508B??E8????????84C074??8D??????50"));
    pSdk->g_pGetNetClientData = getVal4FromRel(tmp + 4);
    spliceUp(tmp, (void *)hookOnAddClient_CheckNickname);
  }

  if (bSyncObjects) {
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
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    BYTES_SEARCH_FORMAT("E8????????84C074??F6??????74??68????"
                                        "????8B??E8????????85C074"));
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    // } else
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    BYTES_SEARCH_FORMAT("834E08048D????85??8D??????????74"));
      pSdk->ObjectCreateStruct_m_Name = *(unsigned *)(tmp + 11);
      spliceUp(tmp + 9, (void *)hookSetObjFlags);
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

    if (bCoop) {
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
        unsigned char *tmp = scanBytes(
            (unsigned char *)gServer, gServerSz,
            BYTES_SEARCH_FORMAT("E8????????88??????8D??????????8B??8B??8B????"
                                "8B????89")); // ragdoll time Multi/SP
        if (tmp) {
          patchData::addCode(tmp, 5);
          memcpy(tmp, moveax0, 5);
        }
      }
      {
        unsigned char *tmp = scanBytes(
            (unsigned char *)gServer, gServerSz,
            BYTES_SEARCH_FORMAT(
                "E8????????8B????8A??88??????8B????????????E8")); // ragdoll
                                                                  // type
                                                                  // Multi/SP
        if (tmp) {
          patchData::addCode(tmp, 5);
          memcpy(tmp, moveax0, 5);
        }
      }
    } else {
    }
  }
}

__int64 FileSize(const TCHAR *name) {
  WIN32_FILE_ATTRIBUTE_DATA fad;
  if (!GetFileAttributesEx(name, GetFileExInfoStandard, &fad))
    return -1;
  LARGE_INTEGER size;
  size.HighPart = fad.nFileSizeHigh;
  size.LowPart = fad.nFileSizeLow;
  return size.QuadPart;
}

void appData::configParse(char *pathCfg) {
  bPreventNoclip = getCfgInt(pathCfg, (char *)"PreventNoclip");
  // bIgnoreUnusedMsgID = getCfgInt(pathCfg, (char *)"PreventSpecialMsg");
  bSyncObjects = getCfgInt(pathCfg, (char *)"SyncObjects");
  bCustomSkins = getCfgInt(pathCfg, (char *)"CustomSkins");
  bCoop = getCfgInt(pathCfg, (char *)"CoopMode");
  bRandWep = getCfgInt(pathCfg, (char *)"RandomWeapon");
  bBotsMP = getCfgInt(pathCfg, (char *)"BotsMP");
  // pSdk->freeMovement=bPreventNoclip;
  bPreventNoclip = 2;
  bIgnoreUnusedMsgID = 1;
  if (bCoop) {
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServerExe, gServerExeSz,
          BYTES_SEARCH_FORMAT("8B??????????????FF????8B??????89??????????8B"));
      if (tmp) {
        spliceUp(tmp, (void *)hookGameMode2);
      }
    }
  }
}

void regcall hookConfigLoad(reg *p) {
  appData *aData = &handleData::instance()->aData;
  if (p->tax) {
    aData->configParse((char *)p->tax);
  }
}

void regcall hookConfigLoad2(reg *p) {
  appData *aData = &handleData::instance()->aData;
  if (p->tax) {
    if (!aData->bConfigInitialized) {
      aData->bConfigInitialized = 1;
      aData->configHandle();
    }
  }
}

void regcall hookLoadGameServer(reg *p) {
  appData *aData = &handleData::instance()->aData;
  aData->gServer = (unsigned char *)p->tax;
  aData->gServerSz = GetModuleSize((HMODULE)aData->gServer);
  // aData->configHandle();
  {
    unsigned char *tmp = scanBytes(
        (unsigned char *)aData->gServer, aData->gServerSz,
        BYTES_SEARCH_FORMAT(
            "E8????????8BC8E8????????8D4C????FF15????????8B")); // Gamemode
                                                                // reset Config
                                                                // fix
    if (tmp) {
      patchData::addCode(tmp, 2);
      *(unsigned short *)(tmp) = 0x0AEB;
    }
  }
  spliceUp(scanBytes((unsigned char *)aData->gServer, aData->gServerSz,
                     BYTES_SEARCH_FORMAT(
                         "558BEC83E4??81EC????????????8B????85????89??????"
                         "0F??????????8B????85??0F??????????33C0")),
           (void *)hookConfigLoad2);
}

void regcall hookSwitchToModeXP(reg *p) {
  appData *aData = &handleData::instance()->aData;
  if (p->tax == 2) {
    p->tax = 0;
    *(aData->aRunGameModeXP + 13) = 0x3;
    aData->setMpGame(1);
  } else if (!p->tax) {
    *(aData->aRunGameModeXP + 13) = 0x2;
    aData->setMpGame(0);
  }
}

void regcall hookStoryModeView(reg *p) {
  appData *aData = &handleData::instance()->aData;
  unsigned char *adr = (unsigned char *)*(uintptr_t *)(aData->aStoryModeStruct);
  bool state = 0;
  if (!*(unsigned char *)(adr + 0x678))
    state = 0;
  else {
    if (!*(unsigned char *)(adr + 0x679))
      state = 0;
    else
      state = 1;
  }
  p->tax = state;
}

void regcall hookClientGameMode(reg *p) {
  appData *aData = &handleData::instance()->aData;
  char *str = (char *)*(uintptr_t *)p->tax;
  if (hash_rta(str) == hash_ct("SinglePlayer"))
    aData->setFlashlight(1);
  else
    aData->setFlashlight(0);
}

void regcall hookModelMsg(reg *p) {
  if (p->v1)
    switch (hash_rta((char *)p->v1)) {
    case hash_ct("THROW"):
      if (p->v0 == 0x19)
        p->origFunc = (uintptr_t)p->pt->origFunc + 2; // skip call ebx
      break;
    }
}

void regcall hookClientSettingsLoad(reg *p) {
  appData *aData = &handleData::instance()->aData;
  if (!aData->bSettingsLoaded) {
    aData->bSettingsLoaded = 1;
    {
      unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
          (unsigned char *)aData->gFearExe, aData->gFearExeSz,
          BYTES_SEARCH_FORMAT("8B0D????????85C9A3????????A1????????0F95C3")));
      if (tmp) {
        if (**(uintptr_t **)(tmp + 2)) {
          {
            unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
                (unsigned char *)aData->gFearExe, aData->gFearExeSz,
                BYTES_SEARCH_FORMAT("0F95C385C0885C24??74??B9")));
            if (tmp) {
              unprotectCode(tmp, 3);
              *(unsigned short *)(tmp) = 0xDB30;
              *(unsigned char *)(tmp + 2) = 0x90;
            }
          }
          {
            unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
                (unsigned char *)aData->gFearExe, aData->gFearExeSz,
                BYTES_SEARCH_FORMAT("0F????????89??????????8B??????????89")));
            if (tmp) {
              unprotectCode(tmp, 5);
              memcpy((unsigned char *)tmp,
                     (unsigned char *)(const unsigned char[]){0xB9, 0x01, 0x00,
                                                              0x00, 0x00},
                     5);
            }
          }
          {
            unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
                (unsigned char *)aData->gFearExe, aData->gFearExeSz,
                BYTES_SEARCH_FORMAT(
                    "7402B0018B????8B????E8????????E8????????5F")));
            if (tmp) {
              unprotectCode(tmp, 2);
              *(unsigned short *)(tmp) = 0x9090;
            }
          }
          {
            unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
                (unsigned char *)aData->gFearExe, aData->gFearExeSz,
                BYTES_SEARCH_FORMAT("6A00C705????????01000000C705????????"
                                    "000000008B116A06FF520C")));
            if (tmp) {
              unprotectCode(tmp, 15);
              *(unsigned short *)(tmp) = 0x9090;
              *(unsigned short *)(tmp + 12) = 0x43EB;
            }
          }
          {
            tmp = (unsigned char *)(unsigned *)(scanBytes(
                (unsigned char *)aData->gFearExe, aData->gFearExeSz,
                BYTES_SEARCH_FORMAT(
                    "753468????????FF15????????E8????????85C074206A006A30")));
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
#include "d3d9.h"

// bool isTimerInit;
//     LARGE_INTEGER frequency;
//     LARGE_INTEGER t1,t2;
//     unsigned frames;
void regcall hookPresent(reg *p) {
  // int i = 0;
  // QueryPerformanceCounter(&t2);
  // double elapsedTime;
  // elapsedTime=(float)(t2.QuadPart-t1.QuadPart)/frequency.QuadPart;
  // //  while(true){
  // //    Sleep(0);
  // //    ++i;
  // //    if(i>2300)
  // //      break;
  // // }
  //   QueryPerformanceFrequency(&frequency);
  //   if(elapsedTime>=1.0){
  //     DBGLOG("%d",frames)
  //     frames=0;
  //     QueryPerformanceCounter(&t1);
  //   }

  // ++frames;

  static LARGE_INTEGER PerformanceCount1;
  static LARGE_INTEGER PerformanceCount2;
  static bool bOnce1 = false;
  static double targetFrameTime = 1000.0 / 144.0f;
  static double t = 0.0;
  static DWORD i = 0;

  if (!bOnce1) {
    bOnce1 = true;
    QueryPerformanceCounter(&PerformanceCount1);
    PerformanceCount1.QuadPart = PerformanceCount1.QuadPart >> i;
  }

  while (true) {
    QueryPerformanceCounter(&PerformanceCount2);
    if (t == 0.0) {
      LARGE_INTEGER PerformanceCount3;
      static bool bOnce2 = false;

      if (!bOnce2) {
        bOnce2 = true;
        QueryPerformanceFrequency(&PerformanceCount3);
        i = 0;
        t = 1000.0 / (double)PerformanceCount3.QuadPart;
        auto v = t * 2147483648.0;
        if (60000.0 > v) {
          while (true) {
            ++i;
            v *= 2.0;
            t *= 2.0;
            if (60000.0 <= v)
              break;
          }
        }
      }
      SleepEx(0, 1);
      break;
    }

    if (((double)((PerformanceCount2.QuadPart >> i) -
                  PerformanceCount1.QuadPart) *
         t) >= targetFrameTime)
      break;

    SleepEx(0, 1);
  }
  QueryPerformanceCounter(&PerformanceCount2);
  PerformanceCount1.QuadPart = PerformanceCount2.QuadPart >> i;
}

void appData::initClient() {
  // IDirect3D9 *g_D3D=(IDirect3D9 *)malloc(32);
  // char * test;
  // char ** test2;
  // g_D3D->CreateDevice(0,(D3DDEVTYPE)0,(HWND)0,0,(D3DPRESENT_PARAMETERS*)test,
  // (IDirect3DDevice9**)test2);
  pSdk->aData = this;
  wchar_t str[1024];
  gFearExe = (uint8_t *)GetModuleHandle(0);
  gFearExeSz = GetModuleSize((HMODULE)gFearExe);
  {
    TCHAR cfg[1024];
    // char *cfg = _T("gamecfg.txt");
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
      iniBuffer = (TCHAR *)malloc(cfgSize);
      int sz = getGlobalCfgString(
          0, cfg, _T("Master"),
          _T("http://master.fear-combat.org/api/serverlist-ingame.php"),
          iniBuffer, cfgSize);
      strMaster = _conv2mb(iniBuffer);
      bShowIntro = getGlobalCfgInt(0, cfg, _T("ShowIntro"));
    }
  }
  {
    unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
        (unsigned char *)gFearExe, gFearExeSz,
        BYTES_SEARCH_FORMAT("8B??????????85F60F??????????A1????????F6C40174")));
    if (tmp) {
      spliceUp(tmp, (void *)hookClientSettingsLoad);
    }
  }
//  {
//    unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
//        (unsigned char *)gFearExe, gFearExeSz,
//        BYTES_SEARCH_FORMAT("75??B9????????E8????????B9????????E8????????E8")));
//    if (tmp) {
//      tmp += 2;
//      spliceUp(tmp, (void *)hookPresent);
//    }
//  }
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gFearExe, gFearExeSz,
                  BYTES_SEARCH_FORMAT("A1????????8B??????????8B????FF????33"));
    uintptr_t adr;
    aGameClientStruct = (unsigned char *)*(uintptr_t *)(tmp + 1);
    while (true) {
      adr = *(uintptr_t *)(tmp + 1);
      if (*(uintptr_t *)adr) {
        adr = *(uintptr_t *)adr;
        adr = adr + *(uintptr_t *)(tmp + 7);
        if (*(uintptr_t *)adr) {
          adr = *(uintptr_t *)adr;
          if (*(uintptr_t *)adr) {
            adr = *(uintptr_t *)adr;
            break;
          }
        }
      }
      Sleep(50);
    }
    gClient = (unsigned char *)adr;
  }
  gClientSz = GetModuleSize((HMODULE)gClient);

  unsigned char *gClient = this->gClient;
  unsigned char *gFearExe = this->gFearExe;
  uintptr_t gFearExeSz = this->gFearExeSz;
  uintptr_t gClientSz = this->gClientSz;
  int tmp;
  {
    unsigned char *tmp = (unsigned char *)(scanBytes(
        (unsigned char *)gClient, gClientSz,
        BYTES_SEARCH_FORMAT(
            "740E8B??????????85C00F??????????A1"))); // MOTD info
    if (tmp) {
      patchData::addCode(tmp, 2);
      *(unsigned short *)(tmp) = 0x9090;
    }
  }
  patchClientServer(gFearExe, gFearExeSz);
  {
    unsigned char *tmp =
        (unsigned char *)(scanBytes((unsigned char *)gFearExe, gFearExeSz,
                                    BYTES_SEARCH_FORMAT("837C24??0175??33D2")));
    if (tmp) {
      patchData::codeswap(tmp + 5,
                          (unsigned char *)(const unsigned char[]){0xEB},
                          1); //%s.available.gamespy.com
    }
  }
  {
    unsigned char *tmp = (unsigned char *)(scanBytes(
        (unsigned char *)gFearExe, gFearExeSz,
        BYTES_SEARCH_FORMAT("8B??74??E8????????68????????FF??????????85??89")));
    if (tmp) {
      patchData::codeswap(tmp + 2,
                          (unsigned char *)(const unsigned char[]){0xEB},
                          1); // ICMP disable
    }
  }
  {
    unsigned char *tmp = (unsigned char *)(scanBytes(
        (unsigned char *)gFearExe, gFearExeSz,
        BYTES_SEARCH_FORMAT("B9????????E8????????B9????????E8????????B9????????"
                            "C7"))); // Punkbuster disable
    if (tmp) {
      tmp = (unsigned char *)*(unsigned *)(tmp + 11);
      *(uintptr_t *)(tmp + 0x10) = 0;
    }
  }
  {
    unsigned char *tmp = (unsigned char *)(scanBytes(
        (unsigned char *)gClient, gClientSz,
        BYTES_SEARCH_FORMAT(
            "E8????????8A??????????B8????????33FF84C875"))); // fix
                                                             // keyboard
    if (tmp) {
      patchData::codeswap((unsigned char *)tmp,
                          (unsigned char *)(const unsigned char[]){
                              0x90, 0x90, 0x90, 0x90, 0x90},
                          5);
    }
  }
  {
    unsigned char *tmp = (unsigned char *)(scanBytes(
        (unsigned char *)gClient, gClientSz,
        BYTES_SEARCH_FORMAT("74??8B????FF????????????6A"))); // no weapon bug
    if (tmp) {
      patchData::addCode(tmp + 1, 1);
      *(unsigned char *)(tmp + 1) -= 7;
    }
  }
  {
    unsigned char *tmp = (unsigned char *)(scanBytes(
        (unsigned char *)gClient, gClientSz,
        BYTES_SEARCH_FORMAT(
            "6A1BFF??????????0FBFC085C079??E8????????"))); // No escape
                                                           // handling on
                                                           // downloading
                                                           // content
    if (tmp) {
      patchData::addCode(tmp, 2);
      *(unsigned short *)(tmp) = 0x3BEB;
    }
  }
  //#ifdef NOINTRO
  if (!bShowIntro) {
    /*{
      unsigned char *tmp = (unsigned char *)(scanBytes(
          (unsigned char *)gClient, gClientSz,
          (char *)"83??????????FD8B??????????8B??68????????FF??????????85??"
                  "74"));  // No intro
      if (tmp) {
        unprotectCode(tmp);
        *(unsigned short *)(tmp + 28) = 0x1EEB;
      }
    }*/
    {
      unsigned char *tmp = (unsigned char *)(scanBytes(
          (unsigned char *)gClient, gClientSz,
          BYTES_SEARCH_FORMAT(
              "5155568BF18B0D????????8B01FF90????????8BE8"))); // no splash
                                                               // videos
      if (tmp) {
        patchData::addCode(tmp, 4);
        *(unsigned *)(tmp) = 0x000004C2;
      }
    }

    {
      void *tmp = (unsigned *)(scanBytes(
          (unsigned char *)gClient, gClientSz,
          BYTES_SEARCH_FORMAT("8B0D??????????FF57??85ED89??????76")));
      if (tmp) {
        patchData::codeswap((unsigned char *)tmp + 16,
                            (unsigned char *)(const unsigned char[]){
                                0xE9, 0xF4, 0x00, 0x00, 0x00},
                            5); // no logos
      }
    }
  }
  //#endif
  {
    unsigned char *tmp = (unsigned char *)(scanBytes(
        (unsigned char *)gClient, gClientSz,
        BYTES_SEARCH_FORMAT(
            "FF????84??75??A1????????6A01??8D????????8B"))); // MOTD
    if (tmp) {
      patchData::addCode(tmp + 5, 1);
      *(tmp + 5) = 0xEB;
    }
  }
  {
    unsigned char *tmp = (unsigned char *)(scanBytes(
        (unsigned char *)gClient, gClientSz,
        BYTES_SEARCH_FORMAT(
            "7D??B22D33??381088??????75"))); // cdkey menu fix
    if (tmp) {
      patchData::addCode(tmp, 1);
      *(tmp) = 0xEB;
    }
  }
  /*{
    unsigned char *tmp = (unsigned char *)(scanBytes(
        (unsigned char *)gClient, gClientSz,
        (char *)"837E08FF74??E8"));  // weapon animation bug
    if (tmp) {
      patchData::addCode(tmp+4, 1);
      *(tmp + 4) = 0xEB;
    }
  }*/
  {
    void *tmp = (unsigned *)(scanBytes(
        (unsigned char *)gFearExe, gFearExeSz,
        BYTES_SEARCH_FORMAT("0F94C16A01518B8E??00000052")));
    if (tmp) {
      patchData::addCode((unsigned char *)tmp, 4);
      *(unsigned *)tmp = 0x6a90c931;
    }
  }

  doConnectIpAdrTramp = (unsigned char *)scanBytes(
      (unsigned char *)gFearExe, gFearExeSz,
      BYTES_SEARCH_FORMAT("8BCF89B71C060000E8????????8B871C060000"));
  spliceUp(scanBytes((unsigned char *)gFearExe, gFearExeSz,
                     BYTES_SEARCH_FORMAT("FFD38B54242083C40C80660CDF")),
           (void *)hookModelMsg);
  pSdk->fearDataInit();
  {
    unsigned char *tmp = scanBytes(
        (unsigned char *)gFearExe, gFearExeSz,
        BYTES_SEARCH_FORMAT(
            "0F8467010000A1????????85C08BC87522A1????????5068????????E8"));
    if (tmp) {
      patchData::addCode(tmp + 2, 2);
      *(unsigned short *)(tmp + 2) = 0x007D;
    }
  }

  {
    unsigned char *tmp = (unsigned char *)(scanBytes(
        (unsigned char *)gClient, gClientSz,
        BYTES_SEARCH_FORMAT("74078BCEE8????????8B166A008BCEFF5268"))); // MOTD
    if (tmp) {
      patchData::addCode(tmp, 1);
      *(tmp) = 0xEB;
    } else {
      // Extraction Point
      aRunGameModeXP =
          scanBytes((unsigned char *)gClient, gClientSz,
                    BYTES_SEARCH_FORMAT("FF????????????8B????????6A02FF5044"));
      spliceUp(aRunGameModeXP, (void *)hookSwitchToModeXP);
      aIsMultiplayerGameClient = scanBytes(
          (unsigned char *)gClient, gClientSz,
          BYTES_SEARCH_FORMAT("A1????????85??74??8B????83??0474??83??0574"));
      unprotectCode(aIsMultiplayerGameClient);
    }
  }

  if (doConnectIpAdrTramp) {
    spliceUp(scanBytes(
                 (unsigned char *)gClient, gClientSz,
                 BYTES_SEARCH_FORMAT(
                     "E8????????84C074218D4C2404E8????????6A016A008D44240C50")),
             (void *)hookSwitchToSP /*, (void *)6*/);
    void *tmp = scanBytes(
        (unsigned char *)gClient, gClientSz,
        BYTES_SEARCH_FORMAT(
            "E8????????84C0754B8D4C2404E8????????6A016A008D44240C50"));
    aIsMultiplayerGameClient = (unsigned char *)((unsigned char *)tmp + 1) +
                               (*(uintptr_t *)((unsigned char *)tmp + 1)) + 4;
    unprotectCode(aIsMultiplayerGameClient);
    spliceUp(tmp, (void *)hookSwitchToMP);

    // toggleIsMPGame();
  }

  {
    unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
        (unsigned char *)gClient, gClientSz,
        BYTES_SEARCH_FORMAT(
            "A1????????8A887806000084C9740E8A887906000084C974048AC3")));
    if (tmp) {
      aStoryModeStruct = (unsigned char *)*(uintptr_t *)(tmp + 1);
    }
  }
  {
    unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
        (unsigned char *)gClient, gClientSz,
        BYTES_SEARCH_FORMAT(
            "6A00FF??????????8B??????????8B????FF??????????8B")));
    if (tmp) {
      unprotectCode(tmp);
      *(unsigned short *)(tmp) = 0x9050;

      spliceUp(tmp, (void *)hookStoryModeView);
    }
  }
  {
    unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
        (unsigned char *)gClient, gClientSz,
        BYTES_SEARCH_FORMAT("50E8????????8B??E8????????84C00F????????????E8")));
    if (tmp) {
      spliceUp(tmp, (void *)hookClientGameMode);
    }
  }
  {
    unsigned char *tmp = gClient;
    unsigned i = 0;
    while (true) {
      tmp =
          scanBytes((unsigned char *)tmp, (gClientSz + gClient) - tmp,
                    BYTES_SEARCH_FORMAT(
                        "E8????????84C075??8B??????????8B??8D????????33??FF"));
      if (tmp) {
        unprotectCode(tmp);
        aFlashlight[i] = tmp;
        // memcpy(tmp,moveax0,5);
      }
      tmp++;
      i++;
      if (i == 2)
        break;
    }
  }
}

// void *appData::pointerScan(void * p1, void * p2) {
//   unsigned char *tmp = gEServer;
//   unsigned i = 0;
//   while (true) {
//     tmp = scanBytes(
//         (unsigned char *)tmp, (gEServerSz + gEServer) - tmp,
//         BYTES_SEARCH_FORMAT("6E61746E6567??2E67616D657370792E636F6D"));
//     if (tmp) {
//       unprotectMem(tmp);
//       *(tmp + 1) = *(tmp + 6);
//       *(tmp + 2) = '.';
//       memcpy(tmp + 3, _C("fear-combat.org"), sizeof("fear-combat.org"));
//     }
//     tmp++;
//     i++;
//     // if (i == 2)
//     //   break;
//   }
// }

void appData::init() {
  srand(__rdtsc());
  pSdk->aData = this;
  if (gEServer) {
    {
      TCHAR cfg[2048];
      // char *cfg = _T("gamecfg.txt");
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
        iniBuffer = (TCHAR *)malloc(cfgSize);
        TCHAR *pIniBuf = iniBuffer;
        int sz = getGlobalCfgString(
            1, cfg, _T("NS1"), _T("natneg1.gamespy.com"), pIniBuf, cfgSize);

        strNs1 = _conv2mb(pIniBuf);
        pIniBuf += sz;
        sz = getGlobalCfgString(1, cfg, _T("NS2"), _T("natneg2.gamespy.com"),
                                pIniBuf, cfgSize);
        strNs2 = _conv2mb(pIniBuf);
        pIniBuf += sz;
        sz = getGlobalCfgString(1, cfg, _T("Available"),
                                _T("%s.available.gamespy.com"), pIniBuf,
                                cfgSize);
        strMasterAvail = _conv2mb(pIniBuf);
        pIniBuf += sz;
        sz = getGlobalCfgString(1, cfg, _T("Master"),
                                _T("%s.master.gamespy.com"), pIniBuf, cfgSize);
        strMaster = _conv2mb(pIniBuf);
        pIniBuf += sz;
        sz = getGlobalCfgString(
            1, cfg, _T("MOTD"),
            (TCHAR
                 *)L"", // http://motd.gamespy.com/motd/vercheck.asp?userid=%d&productid=%d&versionuniqueid=%s&distid=%d&uniqueid=%s&gamename=%s
            pIniBuf, cfgSize);
        strMotd = _conv2mb(pIniBuf);
      }
    }
    gEServerSz = GetModuleSize((HMODULE)gEServer);
    {
      spliceUp(
          scanBytes((unsigned char *)gEServer, gEServerSz,
                    BYTES_SEARCH_FORMAT(
                        "84??75????68??????????E8????????8B??83C40885??74")),
          (void *)hookLoadGameServer);
    }
    gServerExe = (unsigned char *)GetModuleHandle(0);
    gServerExeSz = GetModuleSize((HMODULE)gServerExe);

    spliceUp(scanBytes(
                 (unsigned char *)gServerExe, gServerExeSz,
                 BYTES_SEARCH_FORMAT(
                     "8D????????????8B??E8????????8B????85C075??E8????????8B")),
             (void *)hookConfigLoad);

    {
      unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
          (unsigned char *)gEServer, gEServerSz,
          BYTES_SEARCH_FORMAT("A1????????508B??E8????????83")));
      if (tmp) {
        const char **arr = (const char **)*(uintptr_t *)(tmp + 1);
        unprotectMem((uint8_t *)arr);
        arr[0] = strNs1;
        arr[1] = strNs2;
      }
    }
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gEServer, gEServerSz,
          BYTES_SEARCH_FORMAT(
              "8B??????508D??????68??????????FF??????????83C4??8D??????68"));
      patchData::addCode(tmp + 1, 22);
      //*(tmp) = 0x68;
      // if (*strGame)
      //   *(uintptr_t *)(tmp + 1) = (uintptr_t)strGame;
      *(const char **)(tmp + 10) = strMaster;
      *(uint16_t *)(tmp + 15) = 0xE890;
      *(uintptr_t *)(tmp + 17) =
          getRel4FromVal((tmp + 17), (unsigned char *)strcpy);
    }
    {
      unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
          (unsigned char *)gEServer, gEServerSz,
          BYTES_SEARCH_FORMAT("68????????50FF15????????83C40C8D")));
      if (tmp) {
        patchData::addCode(tmp + 1, 12);
        *(const char **)(tmp + 1) = strMasterAvail;
        *(uint16_t *)(tmp + 6) = 0xE890;
        *(uintptr_t *)(tmp + 8) =
            getRel4FromVal((tmp + 8), (unsigned char *)strcpy);
      }
    }
    unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
        (unsigned char *)gEServer, gEServerSz,
        BYTES_SEARCH_FORMAT("6A1068????????8B??83????6A0083")));
    if (tmp) {
      patchData::addCode(tmp, 2);
      *(uint16_t *)(tmp) = 0x27EB; // disable magic value
    }
    {
      unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
          (unsigned char *)gEServer, gEServerSz,
          BYTES_SEARCH_FORMAT("68????????68????????FF15????????56")));
      if (tmp) {
        tmp += 1;
        patchData::addCode(tmp, 4);
        *(const char **)tmp = strMotd;
      }
    }
    { //%s.ms%d.gamespy.com disable
      unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
          (unsigned char *)gEServer, gEServerSz,
          BYTES_SEARCH_FORMAT("760DB8060000005D81C4????????C3")));
      if (tmp) {
        patchData::addCode(tmp, 2);
        *(uint16_t *)(tmp) = 0x9090;
      }
    }

    if (!*strMotd) { // modt disable
      unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
          (unsigned char *)gEServer, gEServerSz,
          BYTES_SEARCH_FORMAT(
              "833D????????01740333C0C3538B5C241085DB750433C0")));
      if (tmp) {
        patchData::addCode(tmp, 2);
        *(uint16_t *)(tmp + 7) = 0x9090;
      }
    }
  }
}

appData::~appData() {
  if (iniBuffer)
    free(iniBuffer);
}
