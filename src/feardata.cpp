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
#include "conv.h"
#include "patch.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifndef inet_pton
int __stdcall inet_pton(INT Family, LPCSTR pStringBuf, PVOID pAddr);
#endif
#ifdef __cplusplus
}
#endif

extern unsigned char *doConnectIpAdrTramp;
bool regcall isValidIpAddressPort(char *ipAddress) {
  unsigned char ip[16];
  char *pStr = strstr(ipAddress, ":"), *pSep;
  if (!pStr) return 0;
  if (atoi(pStr + 1) >= 0x10000) return 0;
  *pStr = 0;
  int result = inet_pton(AF_INET, ipAddress, ip);
  *pStr = ':';
  return result != 0;
}

bool fearData::checkPlayerStatus(GameClientData *pGameClientData) {
  bool res = 0;
  {
    if (*(unsigned *)(*(uintptr_t *)(pGameClientData + 0x80) +
                      sizeof(uintptr_t)) !=
        eClientConnectionState_InWorld) {  // connection state
      res = 1;
    }
    if (*(char *)(((GameModeMgr * (*)()) g_pGameModeMgr_instance)() +
                  0x18C) /*m_grbUseTeams*/) {
      if (*(unsigned char *)(pGameClientData + 0x74) == 0xFF) {  // team id
        res = 1;
      }
    }
  }
  return res;
}

GameClientData *fearData::getGameClientData(HCLIENT client) {
  return ((GameClientData * (objcall *)(ServerConnectionMgr *, HCLIENT))
              g_pGetGameClientData)(g_pServerConnectionMgr, client);
}

void fearData::BootWithReason(GameClientData *pGameClientData,
                              unsigned char code, char *reason) {
  ((GameClientData * (objcall *)(ServerConnectionMgr *, GameClientData *,
                                 unsigned, char *)) g_pBootWithReason)(
      g_pServerConnectionMgr, pGameClientData, code, reason);
};

HLOCALOBJ fearData::GetClientObject(HCLIENT hClient) {
  return ((HLOCALOBJ(objcall *)(void *, HCLIENT))g_pLTServer_GetClientObject)(
      g_pLTServer, hClient);
}

CPlayerObj *fearData::GetPlayerFromHClient(HCLIENT hClient) {
  HLOCALOBJ hLocal = GetClientObject(hClient);
  return ((CPlayerObj * (cdeclcall *)(HCLIENT))
              g_pLTServer_HandleToObject)(hLocal);
}

HWEAPONDATA fearData::GetWeaponData(HWEAPON hWep) {
  return ((HWEAPONDATA(__thiscall *)(
      void *, HWEAPON, bool))g_pWeaponDB_GetWeaponData)(g_pWeaponDB, hWep,
                                                        !USE_AI_DATA);
}

HWEAPONDATA fearData::GetAmmoData(HAMMO hAmmo) {
  return ((
      HWEAPONDATA(__thiscall *)(void *, HWEAPON, bool))g_pWeaponDB_GetAmmoData)(
      g_pWeaponDB, hAmmo, !USE_AI_DATA);
}

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

uint32_t fearData::getRealTimeMS() {
  return ((uint32_t(__thiscall *)(void *)) *
          (uintptr_t *)((unsigned char *)*(uintptr_t *)g_pLTServer +
                        ILTServer_GetRealTimeMS))(g_pLTServer);
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

void CPlayerObj::handlePlayerPositionMessage(CAutoMessageBase_Read *pMsg) {
  fearData *pSdk = &handleData::instance()->pSdk;
  pSdk->handlePlayerPositionMessage(this, pMsg);
}

void CPlayerObj::updateMovement(){
  fearData *pSdk = &handleData::instance()->pSdk;
  pSdk->updateMovement(this);
}

void fearData::updateMovement(CPlayerObj * pPlayerObj){
    HCLIENT hClient = (HCLIENT) * (HCLIENT *)((unsigned char *)pPlayerObj +
                                            CPlayerObj_m_hClient);
    if(!hClient)
      return;
  //GameClientData *pGameClientData = getGameClientData(hClient);
  unsigned clientId = g_pLTServer->GetClientID(hClient);
  playerData *pPlData = &pPlayerData[clientId];
  //if (!pGameClientData) {
  //  return;
  //}
  // If we're in multiplayer, handle leashing
  if(/* IsMultiplayerGameServer( ) && */ *(bool *)((unsigned char *)pPlayerObj + CPlayerObj_m_bUseLeash) ){

    // calculate the difference between the server's position and client's position of the player object
    LTVector curPos;
    g_pLTServer->GetObjectPos(pPlayerObj->m_hObject, &curPos);
    float fDistSqr = curPos.DistSqr(*(LTVector *)((unsigned char *)pPlayerObj + CPlayerObj_m_vLastClientPos));
    
    // get the velocity - this is used to scale the leash length
    LTVector vVelocity;
    g_pPhysicsLT->GetVelocity(pPlayerObj->m_hObject, &vVelocity);

    // this check determines if the object has moved outside of the movement dead-zone, which
    // is the allowable distance between the client's position for the player object and the
    // position of the object on the server
    if (fDistSqr > (*(float *)((unsigned char *)pPlayerObj + CPlayerObj_m_fLeashLen))* (*(float *)((unsigned char *)pPlayerObj + CPlayerObj_m_fLeashLen)))
    {
      // we're outside of the dead-zone, so interpolate toward the client's reported position
      LTVector vNewPos = curPos.Lerp(*(LTVector *)((unsigned char *)pPlayerObj + CPlayerObj_m_vLastClientPos), *(float *)((unsigned char *)pPlayerObj + CPlayerObj_m_fLeashSpringRate));

      // use physics to move the object toward the target point
      g_pPhysicsLT->MoveObject(pPlayerObj->m_hObject, vNewPos, 0);

      // determine the maximum allowed distance by scaling the velocity, but don't
      // reduce it below the minimum threshold
      float fLeashScale = 0.14f;
      if(aData->bPreventNoclip == 2)
        fLeashScale = 0.3f;
      float fMaxDistance = LTMAX(vVelocity.Mag() * fLeashScale/*(*(float *)((unsigned char *)pPlayerObj + CPlayerObj_m_fLeashScale))*/, (*(float *)((unsigned char *)pPlayerObj + CPlayerObj_m_fLeashSpring)));

      // recalculate the distance from the client's last reported position
      fDistSqr = vNewPos.DistSqr(*(LTVector *)((unsigned char *)pPlayerObj + CPlayerObj_m_vLastClientPos));

      // check to see if we've exceeded the maximum allowed distance
      if (fDistSqr > fMaxDistance*fMaxDistance)
      {
        unsigned timeMs = getRealTimeMS(); 
        //if (s_vtAlwaysForceClientToServerPos.GetFloat())
        if(*(unsigned *)((unsigned char *)pPlayerObj + CPlayerObj_m_ePlayerState) == ePlayerState_Alive && !freeMovement && !pPlData->bLadderInUse/*!*(HOBJECT *)((unsigned char *)pPlayerObj +
                                         CCharacter_m_hLadderObject)*/)
        {
          // force the client to our position
          ((void(__thiscall *)(CPlayerObj *,
                                   bool))CPlayerObj_TeleportClientToServerPos)(
                  pPlayerObj, false);

          // set the velocity to zero
          LTVector vVel(0.0f, 0.0f, 0.0f);
          g_pPhysicsLT->SetVelocity(pPlayerObj->m_hObject, vVel);

          // store the current time, which will be used to disregard player updates
          // that were sent by the client prior to the teleport event
          *(unsigned *)((unsigned char *)pPlayerObj + CPlayerObj_m_nLastPositionForcedTime) = timeMs;

          // turn off the leash
          *(bool *)((unsigned char *)pPlayerObj + CPlayerObj_m_bUseLeash) = false;
        }
        else
        {

            pPlData->leashBrokenExceedCnt++;
            if(pPlData->leashBrokenExceedCnt){
              unsigned fTimeDelta = timeMs - pPlData->leashBrokenTimeMS;
              if(fTimeDelta >= 3000)
                pPlData->leashBrokenExceedCnt = 0;
            }
            pPlData->leashBrokenTimeMS = timeMs;
            if (pPlData->leashBrokenExceedCnt > 2)
              g_pLTServer->KickClient(hClient);
              /*BootWithReason(pGameClientData, eClientConnectionError_InvalidAssets,
                             (char *)0);*/
        // use physics to move the object with collisions
        g_pPhysicsLT->MoveObject(pPlayerObj->m_hObject, *(LTVector *)((unsigned char *)pPlayerObj + CPlayerObj_m_vLastClientPos), 0);
        
        // force the position in case the object was blocked
        g_pLTServer->SetObjectPos(pPlayerObj->m_hObject, *(LTVector *)((unsigned char *)pPlayerObj + CPlayerObj_m_vLastClientPos));
        
        // turn off the leash if the object is not moving
        *(bool *)((unsigned char *)pPlayerObj + CPlayerObj_m_bUseLeash) = vVelocity.Mag() > 0.001f;
      }
    }
    }
    else if (vVelocity.Mag() < 0.001f)
    {
      // Turn off the leash, we're close enough
      *(bool *)((unsigned char *)pPlayerObj + CPlayerObj_m_bUseLeash) = false;
    }
  }
}

inline void fearData::handlePlayerPositionMessage(CPlayerObj *pPlayerObj,
                                                  CAutoMessageBase_Read *pMsg) {
  if (!pPlayerObj) return;
  CArsenal *pArsenal =
      (CArsenal *)((unsigned char *)pPlayerObj + CPlayerObj_m_Arsenal);
  if (!pArsenal) {
    return;
  }
  // Skip out if it's not included
  if (!pMsg->ReadBits(1)) return;
  LTVector newPos, newVel;
  uint8_t moveCode = pMsg->ReadBits(8);
  pMsg->ReadData((void *)&newPos, 0x60);
  pMsg->ReadData((void *)&newVel, 0x60);
  bool bOnGround = pMsg->ReadBits(1);
  *(unsigned *)((unsigned char *)pPlayerObj + CPlayerObj_m_eStandingOnSurface) =
      /*m_eStandingOnSurface = (SurfaceType)*/ pMsg->ReadBits(8);
  HCLIENT hClient = (HCLIENT) * (HCLIENT *)((unsigned char *)pPlayerObj +
                                            CPlayerObj_m_hClient);
  GameClientData *pGameClientData = getGameClientData(hClient);
  unsigned clientId = g_pLTServer->GetClientID(hClient);
  playerData *pPlData = &pPlayerData[clientId];
  if (!pGameClientData) {
    return;
  }
  if (moveCode == *(unsigned char *)((unsigned char *)pGameClientData +
                                     GameClientData_m_nClientMoveCode)) {
    LTVector curPos;
    *(bool *)((unsigned char *)pPlayerObj + CCharacter_m_bOnGround) = bOnGround;
    g_pLTServer->GetObjectPos(pPlayerObj->m_hObject, &curPos);
    // if (IsMultiplayerGameServer( ))
    if (1) g_pPhysicsLT->SetVelocity(pPlayerObj->m_hObject, newVel);
    *(LTVector *)((unsigned char *)pPlayerObj + CPlayerObj_m_vLastVelocity) =
        newVel;
unsigned timeMs = pPlData->thisMoveTimeMS;
    if (!((bool(__thiscall *)(void *))g_pGameServerShell_IsPaused)(
            g_pGameServerShell)) {
          bool bMove = 1;
          LTVector oldPos;
          if (pPlData->movePacketCnt >= 5) {
            pPlData->movePacketCnt = 0;
            oldPos = pPlData->lastPos;
            // SYSTEMTIME time;
            // GetSystemTime(&time);
            // unsigned timeMs = (time.wSecond * 1000) + time.wMilliseconds;
            // unsigned timeMs=getRealTimeMS();
            
            /*if(pPlData->bLadderInUse && (pPlData->thisMoveTimeMS - pPlData->ladderInUseTimer)>5120){
              pPlData->ladderInUseTimer=0;
              pPlData->bLadderInUse=0;
            }*/
            
            if (pPlData->lastMoveTimeMS) {
              HWEAPON hWeapon = *(HWEAPON *)((unsigned char *)pArsenal +
                                             CArsenal_m_hCurWeapon);
              if (pPlData->lastWep != hWeapon) {
                pPlData->lastWep = hWeapon;
                HWEAPONDATA hWpnData = GetWeaponData(hWeapon);
                pPlData->fMovementMultiplier =
                    getFloatDB(hWpnData, WDB_WEAPON_fMovementMultiplier);
                bMove = 1;
              } else {
                float x = (newPos.x - oldPos.x), y = (newPos.y - oldPos.y),
                      z = (newPos.z - oldPos.z);

                float runSpeed = getFloatDB(m_hPlayer,PLAYER_BUTE_RUNSPEED);

                float fMoveMultiplier =
                    *(float *)((unsigned char *)pPlayerObj +
                               CPlayerObj_m_fMoveMultiplier);
                // float fJumpMultiplier = *(float *)((unsigned char
                // *)pPlayerObj +
                //               CPlayerObj_m_fJumpMultiplier);
                float dist = ((fsqrt((x * x) + (y * y) + (z * z))) * 0.1) *
                             0.1;  // metre
                float speed;       // = 60.0f;
                if (oldPos.y == newPos.y) {
                  //speed = ((fMoveMultiplier * 2.0f) * 10.0f) *
                  //        pPlData->fMovementMultiplier;
                  speed = ((runSpeed+24.0f)*0.036f) *(fMoveMultiplier * pPlData->fMovementMultiplier);
                  float predict = (dist / speed) * 3600.0f;
                  float fTimeDelta = (float)(timeMs - pPlData->lastMoveTimeMS);
                  if ((fTimeDelta < predict)) {
                    //DBGLOG("%lf %lf %lf",predict,fTimeDelta,dist);
                    bMove = 0;
                  }
                }
              }
            }
            pPlData->lastMoveTimeMS = timeMs;
            pPlData->lastPos = newPos;
          } else {
            pPlData->movePacketCnt++;
          }
        if (!bMove) {
            pPlData->moveLimitExceedCnt++;
            if(pPlData->moveLimitLastTimeMS){
              float fTimeDelta = (float)(timeMs - pPlData->moveLimitLastTimeMS);
              if(fTimeDelta >= 15000)
                pPlData->moveLimitExceedCnt = 0;
            }
            pPlData->moveLimitLastTimeMS = timeMs;
            if (pPlData->moveLimitExceedCnt > 10)
              BootWithReason(pGameClientData, eClientConnectionError_PunkBuster,
                             (char *)"Unstable connection");
              /*((void(__thiscall *)(CPlayerObj *,
                                 bool))CPlayerObj_TeleportClientToServerPos)(
                pPlayerObj, true);*/
        }
      // m_PlayerRigidBody.Update( newPos );
      ((void(__thiscall *)(void *,
                           LTVector &))CPlayerObj_m_PlayerRigidBody_Update)(
          (void *)((unsigned char *)pPlayerObj + CPlayerObj_m_PlayerRigidBody),
          newPos);
      if (!1/*!freeMovement && !*(HOBJECT *)((unsigned char *)pPlayerObj +
                                         CCharacter_m_hLadderObject)*/) {
        if (!curPos.NearlyEquals(newPos, 0.1f)) {
          // Move it first so we get collisions and stuff
          // if (_fabs(_fabs(newPos.y) - _fabs(curPos.y)) >= 40.0f) bMove = 1;
          // if ((curPos.y) - (newPos.y) >= 10.0f) bMove = 1;
          // bMove=1;
          // if (oldPos.y != newPos.y)
          //  bMove = 1;
          // if (bMove)
          g_pPhysicsLT->MoveObject(pPlayerObj->m_hObject, newPos, 0);
          /*else {
            g_pPhysicsLT->MoveObject(pPlayerObj->m_hObject, oldPos, 0);
            ((void(__thiscall *)(CPlayerObj *,
                                 bool))CPlayerObj_TeleportClientToServerPos)(
                pPlayerObj, true);
            return;
          }*/



          LTVector vResultPos;
          g_pLTServer->GetObjectPos(pPlayerObj->m_hObject, &vResultPos);

          if (vResultPos != newPos) {
            // Then just teleport it there in case it didn't make it for some
            // reason
            // if ()
            g_pLTServer->SetObjectPos(pPlayerObj->m_hObject, newPos);
            //if (!curPos.NearlyEquals(newPos, 320.0f)) {
            //  ((void(__thiscall *)(CPlayerObj *,
            //                       bool))CPlayerObj_TeleportClientToServerPos)(
            //      pPlayerObj, true);
            //}
          }
        }
        //}
      } else {
        // Turn gravity on if they are moving in the y dir and the object is
        // solid.  Otherwise make sure it's off.
        bool bGravityOn = (newVel.y * newVel.y > 0.01f);
        uint32_t nFlags = 0;
        g_pCommonLT->GetObjectFlags(pPlayerObj->m_hObject, OFT_Flags, nFlags);
        bGravityOn = bGravityOn && (nFlags & FLAG_SOLID);
        g_pCommonLT->SetObjectFlags(pPlayerObj->m_hObject, OFT_Flags,
                                    bGravityOn ? FLAG_GRAVITY : 0,
                                    FLAG_GRAVITY);
        *(bool *)((unsigned char *)pPlayerObj + CPlayerObj_m_bUseLeash) = true;
      }
    }
  } else {
    /*Invalid client movement code received */
  }
  *(LTVector *)((unsigned char *)pPlayerObj + CPlayerObj_m_vLastClientPos) =
      newPos;
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
  g_pCommonLT = g_pLTServer->Common();
  g_pPhysicsLT = g_pLTServer->Physics();
  g_pModelLT = g_pLTServer->GetModelLT();
  {
    unsigned char *tmp = 0;
    tmp = scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"8D????????????8B??E8????????8B????85");
    if (tmp) {
      GameModeMgr_ServerSettings = *(unsigned *)(tmp + 2);
    }
  }
  {
    unsigned char *tmp = 0;
    tmp = scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"8B??FF??????????8B????8B??6A32");
    if (tmp) {
      ILTServer_GetRealTimeMS = *(unsigned *)(tmp + 4);
    }
  }
  {
    unsigned char *tmp = 0;
    tmp = scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"0D0800800089??????????8B??????????68");
    if (tmp) {
      g_pServerDB = *(CServerDB **)*(uintptr_t *)(tmp + 13);
    }
  }
  {
    unsigned char *tmp = scanBytes((unsigned char *)gServer, gServerSz,
                                   (char *)_C("83E8??746F48741948"));
    unsigned char i = *(tmp + 2);
    ukPEDropWeapon = i + 2;
    ukPENextSpawnPoint = i + 1;
    ukPEPrevSpawnPoint = i;
  }
  {
    void *tmp = scanBytes((unsigned char *)gServer, gServerSz,
                          (char *)_C("8D????FF??????0000??FF"));
    unsigned n = *(unsigned char *)((unsigned char *)tmp + 2);
    g_pLTServer_HandleToObject =
        (void *)(*(uintptr_t *)((unsigned char *)(g_pLTServer) + n));
    n = *(unsigned *)((unsigned char *)tmp + 5);
    g_pLTServer_GetClientObject = (void *)(*(
        uintptr_t *)((unsigned char *)(*(uintptr_t *)g_pLTServer) + n));
  }
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
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"8B????????????FF????8B????8B????8B??????8B");
    g_pArsenal = (CArsenal *)**(unsigned **)(tmp + 2);
    CArsenal_m_pAmmo = *(unsigned char *)(tmp + 12);
  }

  {
    unsigned char *tmp = scanBytes((unsigned char *)gServer, gServerSz,
                                   (char *)"E8????????8A??????????8A??????05");
    g_pGameModeMgr_instance = getVal4FromRel(tmp + 1);
  }
  {
    unsigned char *tmp = scanBytes((unsigned char *)gServer, gServerSz,
                                   (char *)"FF??0884C00F??????????E8????????8BC8E8");
    g_pServerVoteMgr_instance = getVal4FromRel(tmp + 12);
    g_pServerVoteMgr = ((ServerVoteMgr * (*)()) g_pServerVoteMgr_instance)();
  }
  {
    unsigned char *tmp = scanBytes((unsigned char *)gServer, gServerSz,
                                   (char *)"E8????????8B??????83????0F??????????FF????????????8B");
    g_pServerVoteMgr_ClearVote = getVal4FromRel(tmp + 1);
    
  }
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"E8????????8BC8E8????????85C074128B");
    g_pServerConnectionMgr_instance = getVal4FromRel(tmp + 1);
    g_pGetGameClientData = getVal4FromRel(tmp + 8);
    g_pServerConnectionMgr =
        ((ServerConnectionMgr * (*)()) g_pServerConnectionMgr_instance)();
  }
  {
    unsigned char *tmp = scanBytes((unsigned char *)gServer, gServerSz,
                                   (char *)"6A006A0351E8????????8BC8E8");
    g_pBootWithReason = getVal4FromRel(tmp + 13);
  }
  {
    unsigned char *tmp = scanBytes((unsigned char *)gServer, gServerSz,
                                   (char *)"508D442430508B430850FF524084C0");
    if (tmp) {
      patchData::addCode(tmp + 15, 1);
      *(tmp + 15) = 0xEB;
    }
  }
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"FF5204E8????????8A??????????05????????");
    if (tmp) {
      patchData::addCode(tmp, 4);
      *(unsigned *)(tmp) = 0xE8909090;
    }
  }

  {
        unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"D986????????D94424??D84C24??D94424??D84C24??DEC1D94424??D84C24??DEC1D9C1");
    CPlayerObj_m_fLeashLen = *(unsigned *)(tmp + 2);
  }
  {
        unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"8B86????????50578D4C24??518D4C24??E8");
    CPlayerObj_m_fLeashSpringRate = *(unsigned *)(tmp + 2);
  }
  {
        unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"D986????????D94424??D84C24??D94424??D84C24??DEC1D94424??D84C24??DEC1D9FA");
    CPlayerObj_m_fLeashSpring = *(unsigned *)(tmp + 2);
  }
  {
        unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"D88E????????D95424??D8D9DFE0F6C405");
    CPlayerObj_m_fLeashScale = *(unsigned *)(tmp + 2);
  }
  {
        unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"8986????????E9????????8B0D????????8B");
    CPlayerObj_m_nLastPositionForcedTime = *(unsigned *)(tmp + 2);
  }
  {
            unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"83????568BF18B86????????85C00F84????????E8");
    CPlayerObj_UpdateMovement = tmp;

  }
  {
    unsigned char *tmp = scanBytes((unsigned char *)gServer, gServerSz,
                          (char *)"8B0D????????535350E8????????8B4424??473BF8");
    pCmdMgr = **(void ***)(tmp+2);
    CCommandMgr_QueueCommand = getVal4FromRel(tmp+10);
  }
  {
        unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"6A016A016A006A01528BCFE8");
        CPlayerObj_ChangeWeapon = getVal4FromRel(tmp + 12);
  }
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"8D8B????????E8????????8BF88A4424??84C0");
    CPlayerObj_m_Arsenal = *(unsigned *)(tmp + 2);
    CArsenal_GetAmmoCount = getVal4FromRel(tmp + 7);
  }
  {
    unsigned char *tmp = scanBytes((unsigned char *)gServer, gServerSz,
                                   (char *)"83BF??????????74??????????C2");
    CPlayerObj_m_ePlayerState = *(unsigned *)(tmp + 2);
  }
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"4874??480F??????????8B??8B??FF??????????");
    CPlayerObj_DropCurrentWeapon = *(unsigned *)(tmp + 16);
  }
  {
    unsigned char * tmp=scanBytes((unsigned char *)gServer, gServerSz,
                        (char *)"8B????????????89??????????E8????????8B??E8????????85C00F??????????3A??????????0F??????????8A??????88");
    if (tmp) {
      CPlayerObj_m_hClient = *(unsigned *)(tmp + 2);
      CPlayerObj_m_eStandingOnSurface = *(unsigned *)(tmp + 9);
      GameClientData_m_nClientMoveCode = *(unsigned *)(tmp + 35);
      CCharacter_m_bOnGround = *(unsigned *)(tmp + 51);
    }
  }
  {
    unsigned char * tmp=scanBytes((unsigned char *)gServer, gServerSz,
                        (char *)"89??????????89??????????89??????????8B??????????E8????????84C00F??????????8D????????8D??????????E8");
    if (tmp) {
      CPlayerObj_m_vLastVelocity = *(unsigned *)(tmp + 2);
      g_pGameServerShell = (void *)*(uintptr_t *)*(unsigned *)(tmp + 20);
      g_pGameServerShell_IsPaused = getVal4FromRel(tmp + 25);
      CPlayerObj_m_PlayerRigidBody = *(unsigned *)(tmp + 44);
      CPlayerObj_m_PlayerRigidBody_Update = getVal4FromRel(tmp + 49);
    }
  }
  {
    unsigned char * tmp=scanBytes((unsigned char *)gServer, gServerSz,
                        (char *)"C6??????????01EB0F68????????6A01E8????????83C4088B??????8B??????8B??????89");
    if (tmp) {
      CPlayerObj_m_bUseLeash = *(unsigned *)(tmp + 2);
      CPlayerObj_m_vLastClientPos = *(unsigned *)(tmp + 38);
    }
  }
  {
    unsigned char *tmp = scanBytes(
        (unsigned char *)gServer, gServerSz,
        (char *)"D9??????????D8??????????8B??6A208B??D9??????8B442414");
    if (tmp) {
      CPlayerObj_m_fMoveMultiplier = *(unsigned *)(tmp + 8);
      CPlayerObj_m_fJumpMultiplier =
          CPlayerObj_m_fMoveMultiplier + sizeof(float);
    }
  }
  {
    unsigned char *tmp = scanBytes(
        (unsigned char *)gServer, gServerSz,
        (char *)"83EC????8B??8B??????????85??0F??????????8B??????????8B");
    if (tmp) {
      CPlayerObj_TeleportClientToServerPos = (void *)tmp;
    }
  }
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"8B86????????85C08BBE????????7405BFC8000000");
    if (tmp) {
      CCharacter_m_hLadderObject = *(unsigned *)(tmp + 2);
    }
  }
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"508B44????508BCBE8????????8B44242C");
    CArsenal_SetAmmo = getVal4FromRel(tmp + 9);
  }
  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"E8????????8B??????????8B??????8B??89??????6A01");
    CArsenal_m_hCurWeapon = *(unsigned *)(tmp + 7) - CPlayerObj_m_Arsenal;
    CArsenal_m_pPlayer = CArsenal_m_hCurWeapon - sizeof(uintptr_t);
    CArsenal_GetCurWeapon = getVal4FromRel(tmp + 1);
  }
  {
    unsigned char *tmp = scanBytes((unsigned char *)gServer, gServerSz,
                                   (char *)"8B??????85C0568BF1742A");
    CArsenal_DecrementAmmo = tmp;
  }

  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"8D??????????E8????????8B??????????3B??74??6A01");
    CPlayerObj_m_Inventory = *(unsigned *)(tmp + 2);
  }

  {
    unsigned char *tmp =
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"FF52??89??????8B????????6A00??8B??????????E8");
    g_pWeaponDB = (CWeaponDB *)*(uintptr_t *)*(uintptr_t *)(tmp + 17);
    g_pWeaponDB_GetWeaponData = getVal4FromRel(tmp + 22);
  }

  {
    unsigned char * tmp = scanBytes((unsigned char *)gServer, gServerSz,
                             (char *)"E8????????8B??????????6A006A0068??????????89??????E8????????83");
    g_pWeaponDB_GetAmmoData = getVal4FromRel(tmp + 1);
    g_pWeaponDB_GetInt32 = getVal4FromRel(tmp + 26);
  }
  m_hPlayer = g_pLTDatabase->GetRecord(
      *(HDATABASE *)((unsigned char *)g_pServerDB + 0x20), SrvDB_PlayerCat,
      SrvDB_Player);
  this->m_hMissionCat = g_pLTDatabase->GetCategory(
      *(HDATABASE *)((unsigned char *)g_pGameDatabaseMgr + 0x20),
      "Missions/Missions");

  m_hModelsCat = g_pLTDatabase->GetCategory(
      *(HDATABASE *)((unsigned char *)g_pGameDatabaseMgr + 0x20),
      "Character/Models");
  m_hCatWeapons = g_pLTDatabase->GetCategory(
      *(HDATABASE *)((unsigned char *)g_pGameDatabaseMgr + 0x20),
      "Arsenal/Weapons");
  m_hCatAmmo = g_pLTDatabase->GetCategory(
      *(HDATABASE *)((unsigned char *)g_pGameDatabaseMgr + 0x20),
      "Arsenal/Ammo");
   m_hCatGlobal = g_pLTDatabase->GetCategory(
      *(HDATABASE *)((unsigned char *)g_pGameDatabaseMgr + 0x20),
      "Arsenal/Global");
   m_hRecGlobal = g_pLTDatabase->GetRecord( m_hCatGlobal, WDB_GLOBAL_RECORD );
  HCATEGORY hGlobalCat = g_pLTDatabase->GetCategory(
      *(HDATABASE *)((unsigned char *)g_pGameDatabaseMgr + 0x20),
      "Character/Models/Global");
  m_hGlobalRecord = g_pLTDatabase->GetRecordByIndex(hGlobalCat, 0);
  m_hUnarmedRecord = g_pLTDatabase->GetRecordLink(
            g_pLTDatabase->GetAttribute(m_hRecGlobal, "Unarmed"), 0, 0);
  m_hUnarmedRecordAmmo = g_pLTDatabase->GetRecordLink(
            g_pLTDatabase->GetAttribute(GetWeaponData(m_hUnarmedRecord), _C("AmmoName")), 0, 0);
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
    void *tmp = scanBytes((unsigned char *)gClient, gClientSz,
                          (char *)"8B??????????85C95774??8B11FF????");
    ClientConnectionMgr_IGameSpyBrowser =
        *(uintptr_t *)((unsigned char *)tmp + 2);
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
  if (doConnectIpAdrTramp) {
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
                     ClientConnectionMgr_IGameSpyBrowser);
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
                           ClientConnectionMgr_IGameSpyBrowser);
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
  if (p->v0) {
    pSdk->g_pLTServer->GetObjectPos((HOBJECT)p->v0, &pSdk->checkPointPos);
    pSdk->checkPointState = 2;
  }
}

template <typename T>
void shuffle(T *array, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

void regcall fearData::hookOnMapLoaded(reg *p) {
  appData *aData = &handleData::instance()->aData;
  fearData *pSdk = aData->pSdk;
  if(aData->bRandWep){
    srand(__rdtsc());
    pSdk->currentRandomWeaponInd=0;
    unsigned char * in =pSdk->randomWepTable;
    int i;
    for(i=0;i!=17;i++){
      in[i]=i;
    }
    shuffle(in,17);
  }
  ((void(__thiscall *)(void *))pSdk->g_pServerVoteMgr_ClearVote)(pSdk->g_pServerVoteMgr);
  if(aData->bCoop){
    char *lvlName = pSdk->getCurrentLevelName();
    unsigned skinState = 9;
    aData->setPatchHoleKillHives(0);
    aData->setCoopDoSpawn(0);
    pSdk->checkPointState = 0;
    if (lvlName) {
      switch (hash_rta(lvlName)) {
        case hash_ct("07_ATC_Roof"):
          skinState = 0;
          //aData->setCoopDoSpawn(1);
          break;
        case hash_ct("02_Docks"):
          skinState = 0;
          //aData->setCoopDoSpawn(1);
          break;
        case hash_ct("XP2_W06"):
          skinState = 0;
          aData->setCoopDoSpawn(1);
          break;
        case hash_ct("13_Hives"):
          aData->setPatchHoleKillHives(1);
          break;
        //default:
        //  aData->setCoopDoSpawn(1);
        //  break;
      }
      
      aData->skinState = skinState;
      *aData->aSkinStr = (char *)"Player";
      aData->storyModeCnt = 0;
      if (aData->bPreventNoclip) pSdk->freeMovement = 0;
      //*(unsigned short *)(aData->aFreeMovement) = 0x9090;
    }
  }

}


void fearData::setRespawn(HOBJECT hObjResp){
  LTVector startPt;
  g_pLTServer->GetObjectPos(startPtObj, &startPt);
  if (checkPointState != 2) {
    if (memcmp(&startPt, &checkPointPos, sizeof(LTVector))) {
      checkPointState = 0;
    }
  }

  LTVector newPos = checkPointPos;
  if (checkPointState != 2) {
    char *lvlName = getCurrentLevelName();
    if (lvlName) {
      switch (hash_rta(lvlName)) {
        case hash_ct("07_ATC_Roof"):
          newPos = LTVector{1566.47f, 2085.51f, 3938.72f};
          checkPointState = 1;
          break;
        case hash_ct("11_Mapes_Elevator"):
          newPos = LTVector{3085.24f, -400.0f, -3066.04f};
          checkPointState = 1;
          break;
        case hash_ct("02_Docks"):
          newPos = LTVector{-11716.2f, -2833.13f, -10565.7f};
          checkPointState = 1;
          break;
        case hash_ct("18_Facility_Upper"):
          newPos = LTVector{-1340.0f, -1040.0f, -100.0f};
          checkPointState = 1;
          break;
        case hash_ct("09_Bishop_Rescue"):
          newPos = LTVector{18721.3f, 2741.3f, 389.176f};
          checkPointState = 1;
          break;
        case hash_ct("12_Badge"):
          newPos = LTVector{-8808.6f, -30.0f, -3414.9f};
          checkPointState = 1;
          break;
        case hash_ct("17_Factory"):
          newPos = LTVector{-117.221f, -929.996f, -16681.2f};
          checkPointState = 1;
          break;
        case hash_ct("08_Admin"):
          newPos = LTVector{-14026.0f, 2426.0f, 5882.0f};
          checkPointState = 1;
          break;
        case hash_ct("XP_CHU01"):
          newPos = LTVector{-704.0f, -1500.0f, -3072.0f};
          checkPointState = 1;
          break;
        case hash_ct("XP2_W01"):
          newPos = LTVector{15180.0f, 25179.0f, 47028.0f};
          checkPointState = 1;
          break;
        case hash_ct("XP_HOS01"):
          newPos = LTVector{4036.75f, 80.9f, 7598.07f};
          checkPointState = 1;
          break;
        case hash_ct("XP_HOS02"):
          newPos = LTVector{512.0f, 21424.1f, -3840.0f};
          checkPointState = 1;
          break;
        case hash_ct("XP2_W06"):
          newPos = LTVector{-24186.0f, -6460.0f, -65997.0f};
          checkPointState = 1;
          break;
        case hash_ct("XP2_W14"):
          newPos = LTVector{-8651.0f, 990.0f, -4160.0f};
          checkPointState = 1;
          break;
        case hash_ct("XP2_W16"):
          newPos = LTVector{-8972.0f, 2070.0f, -3833.0f};
          checkPointState = 1;
          break;
        case hash_ct("XP2_W17"):
          newPos = LTVector{-21728.0f, 1800.0f, 7588.0f};
          checkPointState = 1;
          break;
        default:
          checkPointState = 1;
          return;
      }
    }
  }
  /*
  CPlayerObj * pPlayerObj = (CPlayerObj *)p->tsi;
  if(*(unsigned char*)((unsigned char *)pPlayerObj+0xC5)){
    HCLIENT playerClient = *(HCLIENT*)((unsigned char *)pPlayerObj+0x2880);
    HOBJECT hPlayerObj = GetClientObject(playerClient);
    g_pLTServer->GetObjectPos(hPlayerObj, &checkPointPos);
    newPos = checkPointPos;
    checkPointState = 2;
  }*/
  if (checkPointState)
    g_pLTServer->SetObjectPos(startPtObj, newPos);
  // if (checkPointState == 2) checkPointState = 1;
}

///100175EC crash ebx 0
void regcall fearData::hookRespawnStartPoint(reg *p) {
  fearData *pSdk = &handleData::instance()->pSdk;
  pSdk->startPtObj = (HOBJECT) * (uintptr_t *)((unsigned char *)p->tax + 8);
  if(pSdk->startPtObj)
    pSdk->setRespawn(pSdk->startPtObj);
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
