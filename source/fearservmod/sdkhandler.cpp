#include "pch.h"
#include "shared/common_macro.h"
#include "shared/memory_utils.h"
#include "shared/debug.h"
//#include "shared/unordered_map.hpp"
#include "splice/splice.h"
#include "shared/patch_handler.h"
//#include "shared/stl.h"
#include "sdk.h"
#include "sdkhandler.h"
#include "executionhandler.h"
#include "shared/string_utils.h"
#include "apphandler.h"
#include <cstdint>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#endif

bool SdkHandler::isValidIpAddressPort(char *ipAddress) {
    sockaddr adr;
    int adrSz = sizeof(sockaddr);
    char *pStr = strstr(ipAddress, ":");
    if (!pStr) return 0;
    if (atoi(pStr + 1) >= 0x10000) return 0;
    *pStr = 0;
    int result = WSAStringToAddressA(ipAddress, AF_INET, 0, &adr, &adrSz);
    // int result = inet_pton(AF_INET, ipAddress, ip);
    *pStr = ':';
    return result == 0;
}

bool SdkHandler::checkPlayerStatus(GameClientData *pGameClientData) {
    bool res = 0;
    {
        if (*(unsigned *)(*(uintptr_t *)(pGameClientData + 0x80) +
                          sizeof(uintptr_t)) !=
            eClientConnectionState_InWorld) {  // connection state
            res = 1;
        }
        if (*(char *)(((GameModeMgr * (*)()) g_pGameModeMgr_instance)() +
                      0x18C) /*m_grbUseTeams*/) {
            if (*(unsigned char *)(pGameClientData + 0x74) ==
                0xFF) {  // team id
                res = 1;
            }
        }
    }
    return res;
}

GameClientData *SdkHandler::getGameClientData(HCLIENT client) {
    return ((GameClientData * (objcall *)(ServerConnectionMgr *, HCLIENT))
                g_pGetGameClientData)(g_pServerConnectionMgr, client);
}

void SdkHandler::BootWithReason(GameClientData *pGameClientData,
                                unsigned char code, char *reason) {
    ((GameClientData * (objcall *)(ServerConnectionMgr *, GameClientData *,
                                   unsigned, char *)) g_pBootWithReason)(
        g_pServerConnectionMgr, pGameClientData, code, reason);
};

void *SdkHandler::HandleToObject(HOBJECT hObj) {
    return ((void *(cdeclcall *)(HOBJECT))g_pLTServer_HandleToObject)(hObj);
}

HLOCALOBJ SdkHandler::GetClientObject(HCLIENT hClient) {
    return ((HLOCALOBJ(objcall *)(void *, HCLIENT))g_pLTServer_GetClientObject)(
        g_pLTServer, hClient);
}

CPlayerObj *SdkHandler::GetPlayerFromHClient(HCLIENT hClient) {
    HLOCALOBJ hLocal = GetClientObject(hClient);
    return (CPlayerObj *)HandleToObject(hLocal);
}

HWEAPONDATA SdkHandler::GetWeaponData(HWEAPON hWep) {
    return ((HWEAPONDATA(__thiscall *)(
        void *, HWEAPON, bool))g_pWeaponDB_GetWeaponData)(g_pWeaponDB, hWep,
                                                          !USE_AI_DATA);
}

HWEAPONDATA SdkHandler::GetAmmoData(HAMMO hAmmo) {
    return ((HWEAPONDATA(__thiscall *)(
        void *, HWEAPON, bool))g_pWeaponDB_GetAmmoData)(g_pWeaponDB, hAmmo,
                                                        !USE_AI_DATA);
}

char *SdkHandler::getCurrentLevelName() {
    if (g_pServerMissionMgrAdr && *g_pServerMissionMgrAdr) {
        CServerMissionMgr *g_pServerMissionMgr = *g_pServerMissionMgrAdr;
        HRECORD hMission = g_pLTDatabase->GetRecordByIndex(
            m_hMissionCat,
            g_pServerMissionMgr->GetCurrentMission());  //+0x98 \\38
        if (!hMission) return nullptr;
        return *(char **)hMission;
    }
    return nullptr;
}

uint32_t SdkHandler::getRealTimeMS() {
    return ((uint32_t(__thiscall *)(void *)) *
            (uintptr_t *)((unsigned char *)*(uintptr_t *)g_pLTServer +
                          ILTServer_GetRealTimeMS))(g_pLTServer);
}

LTRESULT SdkHandler::getClientAddr(HCLIENT hClient, uint8_t pAddr[4],
                                   uint16_t *pPort) {
    return ((uint32_t(__thiscall *)(void *, HCLIENT, uint8_t[4], uint16_t *)) *
            (uintptr_t *)((unsigned char *)*(uintptr_t *)g_pLTServer +
                          ILTServer_GetClientAddr))(g_pLTServer, hClient, pAddr,
                                                    pPort);
}

IDatabaseMgr *SdkHandler::getDatabaseMgr() {
    return ((IDatabaseMgr * (__stdcall *)()) GetProcAddress(
        (HMODULE)ExecutionHandler::instance()->appHandler()->m_database,
        "GetIDatabaseMgr"))();
}

void SdkHandler::getLTServerClient(unsigned char *baseMod, uintptr_t szMod) {
    {
        unsigned char *tmp = baseMod;
        for (unsigned i = 0; i != 2; i++) {
            tmp =
                scanBytes((unsigned char *)tmp, (szMod + baseMod) - tmp,
                          BYTES_SEARCH_FORMAT("6A6968????????68????????E8??????"
                                              "??68????????E8????????83C410"));
            if (!tmp) break;
            if (!memcmp((char *)*(uintptr_t *)((unsigned char *)tmp + 3),
                        "ILTClient", 9))
                g_pLTClient =
                    (ILTClient *)*(uintptr_t *)((unsigned char *)tmp + 8);
            else if (!memcmp((char *)*(uintptr_t *)((unsigned char *)tmp + 3),
                             "ILTServer", 9))
                g_pLTServer =
                    (ILTServer *)*(uintptr_t *)((unsigned char *)tmp + 8);
            tmp++;
        }
    }
    if (!g_pLTServer) {
        unsigned char *tmp = 0;
        tmp = scanBytes((unsigned char *)baseMod, szMod,
                        BYTES_SEARCH_FORMAT("6A69E9????????68????????E8"));
        g_pLTServer = (ILTServer *)*(uintptr_t *)((unsigned char *)tmp + 8);
    }
}

HRECORD SdkHandler::getModelStruct(char *model) {
    return g_pLTDatabase->GetRecordB(m_hModelsCat, model);
}

void CPlayerObj::handlePlayerPositionMessage(CPlayerObj *arg,
                                             CAutoMessageBase_Read *pMsg) {
    auto &inst = *ExecutionHandler::instance()->sdkHandler();
    inst.handlePlayerPositionMessage(arg, pMsg);
}

void CPlayerObj::updateMovement(CPlayerObj *arg) {
    auto &inst = *ExecutionHandler::instance()->sdkHandler();
    inst.updateMovement(arg);
}

void SdkHandler::updateMovement(CPlayerObj *pPlayerObj) {
    HCLIENT hClient = (HCLIENT) * (HCLIENT *)((unsigned char *)pPlayerObj +
                                              CPlayerObj_m_hClient);
    if (!hClient)
      return;
    GameClientData *pGameClientData = getGameClientData(hClient);
    unsigned clientId = g_pLTServer->GetClientID(hClient);
    playerData *pPlData = &pPlayerData[clientId];
    if (!pGameClientData) {
      return;
    }
    // If we're in multiplayer, handle leashing
    if (/* IsMultiplayerGameServer( ) && */ *(
        bool *)((unsigned char *)pPlayerObj + CPlayerObj_m_bUseLeash)) {

      // calculate the difference between the server's position and client's
      // position of the player object
      LTVector curPos;
      g_pLTServer->GetObjectPos(pPlayerObj->m_hObject, &curPos);
      float fDistSqr = curPos.DistSqr(*(LTVector *)((unsigned char *)pPlayerObj +
                                                    CPlayerObj_m_vLastClientPos));

      // get the velocity - this is used to scale the leash length
      LTVector vVelocity;
      g_pPhysicsLT->GetVelocity(pPlayerObj->m_hObject, &vVelocity);

      // this check determines if the object has moved outside of the movement
      // dead-zone, which is the allowable distance between the client's position
      // for the player object and the position of the object on the server
      if (fDistSqr >
          (*(float *)((unsigned char *)pPlayerObj + CPlayerObj_m_fLeashLen)) *
              (*(float *)((unsigned char *)pPlayerObj +
                          CPlayerObj_m_fLeashLen))) {
        // we're outside of the dead-zone, so interpolate toward the client's
        // reported position
        LTVector vNewPos = curPos.Lerp(*(LTVector *)((unsigned char *)pPlayerObj +
                                                     CPlayerObj_m_vLastClientPos),
                                       *(float *)((unsigned char *)pPlayerObj +
                                                  CPlayerObj_m_fLeashSpringRate));

        // use physics to move the object toward the target point
        g_pPhysicsLT->MoveObject(pPlayerObj->m_hObject, vNewPos, 0);

        // determine the maximum allowed distance by scaling the velocity, but
        // don't reduce it below the minimum threshold
        float fLeashScale = 0.14f;
        if (m_appHandler->m_bCoop) //aData->bPreventNoclip == 2)
          fLeashScale = 0.3f;
        float fMaxDistance =
            LTMAX(vVelocity.Mag() *
                      fLeashScale /*(*(float *)((unsigned char *)pPlayerObj +
                                     CPlayerObj_m_fLeashScale))*/
                  ,
                  (*(float *)((unsigned char *)pPlayerObj +
                              CPlayerObj_m_fLeashSpring)));

        // recalculate the distance from the client's last reported position
        fDistSqr = vNewPos.DistSqr(*(LTVector *)((unsigned char *)pPlayerObj +
                                                 CPlayerObj_m_vLastClientPos));

        // check to see if we've exceeded the maximum allowed distance
        if (fDistSqr > fMaxDistance * fMaxDistance) {
          unsigned timeMs = getRealTimeMS();
          // if (s_vtAlwaysForceClientToServerPos.GetFloat())
          if(*(unsigned *)((unsigned char *)pPlayerObj + CPlayerObj_m_ePlayerState) == ePlayerState_Alive && !m_bfreeMovement && !pPlData->bLadderInUse/*!*(HOBJECT *)((unsigned char *)pPlayerObj +
                                           CCharacter_m_hLadderObject)*/)
          {
            // force the client to our position
            ((void(__thiscall *)(CPlayerObj *, bool))
                 CPlayerObj_TeleportClientToServerPos)(pPlayerObj, false);

            // set the velocity to zero
            LTVector vVel(0.0f, 0.0f, 0.0f);
            g_pPhysicsLT->SetVelocity(pPlayerObj->m_hObject, vVel);

            // store the current time, which will be used to disregard player
            // updates that were sent by the client prior to the teleport event
            *(unsigned *)((unsigned char *)pPlayerObj +
                          CPlayerObj_m_nLastPositionForcedTime) = timeMs;

            // turn off the leash
            *(bool *)((unsigned char *)pPlayerObj + CPlayerObj_m_bUseLeash) =
                false;
          } else {

            pPlData->leashBrokenExceedCnt++;
            if (pPlData->leashBrokenExceedCnt) {
              unsigned fTimeDelta = timeMs - pPlData->leashBrokenTimeMS;
              if (fTimeDelta >= 3000)
                pPlData->leashBrokenExceedCnt = 0;
            }
            pPlData->leashBrokenTimeMS = timeMs;
            if (pPlData->leashBrokenExceedCnt > 4)
              // g_pLTServer->KickClient(hClient);
              BootWithReason(pGameClientData, eClientConnectionError_PunkBuster,
                             (char *)"Unstable connection or noclip");
            // DBGLOG("noclip fail")
            /*BootWithReason(pGameClientData,
               eClientConnectionError_InvalidAssets, (char *)0);*/
            // use physics to move the object with collisions
            g_pPhysicsLT->MoveObject(pPlayerObj->m_hObject,
                                     *(LTVector *)((unsigned char *)pPlayerObj +
                                                   CPlayerObj_m_vLastClientPos),
                                     0);

            // force the position in case the object was blocked
            g_pLTServer->SetObjectPos(pPlayerObj->m_hObject,
                                      *(LTVector *)((unsigned char *)pPlayerObj +
                                                    CPlayerObj_m_vLastClientPos));

            // turn off the leash if the object is not moving
            *(bool *)((unsigned char *)pPlayerObj + CPlayerObj_m_bUseLeash) =
                vVelocity.Mag() > 0.001f;
          }
        }
      } else if (vVelocity.Mag() < 0.001f) {
        // Turn off the leash, we're close enough
        *(bool *)((unsigned char *)pPlayerObj + CPlayerObj_m_bUseLeash) = false;
      }
    }
}

void SdkHandler::handlePlayerPositionMessage(CPlayerObj *pPlayerObj,
                                             CAutoMessageBase_Read *pMsg) {
    if (!pPlayerObj)
      return;
    CArsenal *pArsenal =
        (CArsenal *)((unsigned char *)pPlayerObj + CPlayerObj_m_Arsenal);
    if (!pArsenal) {
      return;
    }
    // Skip out if it's not included
    if (!pMsg->ReadBits(1))
      return;
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
      if (1)
        g_pPhysicsLT->SetVelocity(pPlayerObj->m_hObject, newVel);
      *(LTVector *)((unsigned char *)pPlayerObj + CPlayerObj_m_vLastVelocity) =
          newVel;
      unsigned timeMs = pPlData->thisMoveTimeMS;
      if (!((bool(__thiscall *)(void *))g_pGameServerShell_IsPaused)(
              g_pGameServerShell)) {
        bool bMove = 1;
  //      LTVector oldPos;
  //      if (pPlData->movePacketCnt >= 5) {
  //        pPlData->movePacketCnt = 0;
  //        oldPos = pPlData->lastPos;
  //        // SYSTEMTIME time;
  //        // GetSystemTime(&time);
  //        // unsigned timeMs = (time.wSecond * 1000) + time.wMilliseconds;
  //        // unsigned timeMs=getRealTimeMS();

  //        /*if(pPlData->bLadderInUse && (pPlData->thisMoveTimeMS -
  //        pPlData->ladderInUseTimer)>5120){ pPlData->ladderInUseTimer=0;
  //          pPlData->bLadderInUse=0;
  //        }*/

  //        if (pPlData->lastMoveTimeMS) {
  //          HWEAPON hWeapon =
  //              *(HWEAPON *)((unsigned char *)pArsenal + CArsenal_m_hCurWeapon);
  //          if (pPlData->lastWep != hWeapon) {
  //            pPlData->lastWep = hWeapon;
  //            HWEAPONDATA hWpnData = GetWeaponData(hWeapon);
  //            pPlData->fMovementMultiplier =
  //                getFloatDB(hWpnData, WDB_WEAPON_fMovementMultiplier);
  //            bMove = 1;
  //          } else {
  //            float x = (newPos.x - oldPos.x), y = (newPos.y - oldPos.y),
  //                  z = (newPos.z - oldPos.z);

  //            float runSpeed = getFloatDB(m_hPlayer, PLAYER_BUTE_RUNSPEED);

  //            float fMoveMultiplier = *(float *)((unsigned char *)pPlayerObj +
  //                                               CPlayerObj_m_fMoveMultiplier);
  //            // float fJumpMultiplier = *(float *)((unsigned char
  //            // *)pPlayerObj +
  //            //               CPlayerObj_m_fJumpMultiplier);
  //            float dist =
  //                ((fsqrt((x * x) + (y * y) + (z * z))) * 0.1) * 0.1; // metre
  //            float speed;                                            // = 60.0f;
  //            if (oldPos.y == newPos.y) {
  //              // speed = ((fMoveMultiplier * 2.0f) * 10.0f) *
  //              //        pPlData->fMovementMultiplier;
  //              speed = ((runSpeed + 60.0f) * 0.036f) *
  //                      (fMoveMultiplier * pPlData->fMovementMultiplier);
  //              float predict = (dist / speed) * 3600.0f;
  //              float fTimeDelta = (float)(timeMs - pPlData->lastMoveTimeMS);
  //              if ((fTimeDelta < predict)) {
  //                // DBGLOG("%lf %lf %lf",predict,fTimeDelta,dist);
  //                bMove = 0;
  //              }
  //            }
  //          }
  //        }
  //        pPlData->lastMoveTimeMS = timeMs;
  //        pPlData->lastPos = newPos;
  //      } else {
  //        pPlData->movePacketCnt++;
  //      }
  //      if (!bMove) {
  //        pPlData->moveLimitExceedCnt++;
  //        if (pPlData->moveLimitLastTimeMS) {
  //          float fTimeDelta = (float)(timeMs - pPlData->moveLimitLastTimeMS);
  //          if (fTimeDelta >= 15000)
  //            pPlData->moveLimitExceedCnt = 0;
  //        }
  //        pPlData->moveLimitLastTimeMS = timeMs;
  //        if (pPlData->moveLimitExceedCnt > 15) {
  //          pPlData->moveLimitExceedCnt=0;
  //          //BootWithReason(pGameClientData, eClientConnectionError_PunkBuster,
  //          //               (char *)"Unstable connection or speedhack");
  //          // DBGLOG("runspeed fail")
  //        }
  //        /*((void(__thiscall *)(CPlayerObj *,
  //                           bool))CPlayerObj_TeleportClientToServerPos)(
  //          pPlayerObj, true);*/
  //      }
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
              // if (!curPos.NearlyEquals(newPos, 320.0f)) {
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
const char *slowmostr[] = {
    "Debug",   "Default",        "Demo",
    "FastForward",         "FireHall", "Multiplayer",
    "Nightmare1-3", "NoSlowMo",        "SlowMo",
    "TimeStop",   "Vision", "Vision1-2",
    "Vision1-3", 0};

void SdkHandler::initServer() {
    auto &happ = *appHandler();
    uint8_t *Server = happ.m_Server;
    uintptr_t ServerSz = happ.m_ServerSz;
    unsigned char *gEServer = happ.m_eServer;
    uintptr_t gEServerSz = happ.m_eServerSz;
    g_ipchunkSection.reset(static_cast<CRITICAL_SECTION*>(new CRITICAL_SECTION()));
    InitializeCriticalSection(static_cast<CRITICAL_SECTION*>(g_ipchunkSection.get()));
    happ.m_database = (unsigned char *)GetModuleHandle(L"GameDatabase");
    happ.m_databaseSz = GetModuleSize((HMODULE)happ.m_database);
    g_pLTDatabase = getDatabaseMgr();
    getLTServerClient(gEServer, gEServerSz);
    g_pCommonLT = g_pLTServer->Common();
    g_pPhysicsLT = g_pLTServer->Physics();
    g_pModelLT = g_pLTServer->GetModelLT();
    {
        unsigned char *tmp = 0;
        tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("8D????????????8B??E8????????8B????85"));
        if (tmp) {
            GameModeMgr_ServerSettings = *(unsigned *)(tmp + 2);
        }
    }
    {
        unsigned char *tmp = 0;
        tmp = scanBytes((unsigned char *)Server, ServerSz,
                        BYTES_SEARCH_FORMAT("8B??FF??????????8B????8B??6A32"));
        if (tmp) {
            ILTServer_GetRealTimeMS = *(unsigned *)(tmp + 4);
        }
    }
    {
        unsigned char *tmp = 0;
        tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT(
                "FF92????????0FB64C24130FB65424128B44241051520FB6CC"));
        if (tmp) {
            ILTServer_GetClientAddr = *(unsigned *)(tmp + 2);
        }
    }
    {
        unsigned char *tmp = 0;
        tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("0D0800800089??????????8B??????????68"));
        if (tmp) {
            g_pServerDB = *(CServerDB **)*(uintptr_t *)(tmp + 13);
        }
    }
    {
        unsigned char *tmp =
            scanBytes((unsigned char *)Server, ServerSz,
                      BYTES_SEARCH_FORMAT("83E8??746F48741948"));
        unsigned char i = *(tmp + 2);
        ukPEDropWeapon = i + 2;
        ukPENextSpawnPoint = i + 1;
        ukPEPrevSpawnPoint = i;
    }
    {
        void *tmp = scanBytes((unsigned char *)Server, ServerSz,
                              BYTES_SEARCH_FORMAT("8D????FF??????0000??FF"));
        unsigned n = *(unsigned char *)((unsigned char *)tmp + 2);
        g_pLTServer_HandleToObject =
            (void *)(*(uintptr_t *)((unsigned char *)(g_pLTServer) + n));
        n = *(unsigned *)((unsigned char *)tmp + 5);
        g_pLTServer_GetClientObject = (void *)(*(
            uintptr_t *)((unsigned char *)(*(uintptr_t *)g_pLTServer) + n));
    }
    {
        unsigned char *tmp = 0;
        tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT(
                "8B0D??????????6A00E8????????84C075??85??74??8B??8B"));
        if (tmp) {
            g_pServerMissionMgrAdr =
                (CServerMissionMgr **)*(uintptr_t *)(tmp + 2);
        }
    }
    g_pGameDatabaseMgr = (CGameDatabaseMgr *)*(uintptr_t *)*(
        uintptr_t *)(scanBytes((unsigned char *)Server, ServerSz,
                               BYTES_SEARCH_FORMAT(
                                   "8B0D??????????E8????????85C074??80380074?"
                                   "???8D????85")) +
                     2);
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("8B????????????FF????8B????8B????8B??????8B"));
        g_pArsenal = (CArsenal *)**(unsigned **)(tmp + 2);
        CArsenal_m_pAmmo = *(unsigned char *)(tmp + 12);
    }

    {
        unsigned char *tmp =
            scanBytes((unsigned char *)Server, ServerSz,
                      BYTES_SEARCH_FORMAT("E8????????8A??????????8A??????05"));
        g_pGameModeMgr_instance = getVal4FromRel(tmp + 1);
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("FF??0884C00F??????????E8????????8BC8E8"));
        g_pServerVoteMgr_instance = getVal4FromRel(tmp + 12);
        g_pServerVoteMgr =
            ((ServerVoteMgr * (*)()) g_pServerVoteMgr_instance)();
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT(
                "E8????????8B??????83????0F??????????FF????????????8B"));
        g_pServerVoteMgr_ClearVote = getVal4FromRel(tmp + 1);
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("E8????????8BC8E8????????85C074128B"));
        g_pServerConnectionMgr_instance = getVal4FromRel(tmp + 1);
        g_pGetGameClientData = getVal4FromRel(tmp + 8);
        g_pServerConnectionMgr =
            ((ServerConnectionMgr * (*)()) g_pServerConnectionMgr_instance)();
    }
    {
        unsigned char *tmp =
            scanBytes((unsigned char *)Server, ServerSz,
                      BYTES_SEARCH_FORMAT("6A006A0351E8????????8BC8E8"));
        g_pBootWithReason = getVal4FromRel(tmp + 13);
    }

    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("D986????????D94424??D84C24??D94424??"
                                "D84C24??DEC1D94424??D84C24??DEC1D9C1"));
        CPlayerObj_m_fLeashLen = *(unsigned *)(tmp + 2);
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("8B86????????50578D4C24??518D4C24??E8"));
        CPlayerObj_m_fLeashSpringRate = *(unsigned *)(tmp + 2);
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("D986????????D94424??D84C24??D94424??"
                                "D84C24??DEC1D94424??D84C24??DEC1D9FA"));
        CPlayerObj_m_fLeashSpring = *(unsigned *)(tmp + 2);
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("D88E????????D95424??D8D9DFE0F6C405"));
        CPlayerObj_m_fLeashScale = *(unsigned *)(tmp + 2);
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("8986????????E9????????8B0D????????8B"));
        CPlayerObj_m_nLastPositionForcedTime = *(unsigned *)(tmp + 2);
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("83????568BF18B86????????85C00F84????????E8"));
        CPlayerObj_UpdateMovement = tmp;
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("8B0D????????535350E8????????8B4424??473BF8"));
        pCmdMgr = **(void ***)(tmp + 2);
        CCommandMgr_QueueCommand = getVal4FromRel(tmp + 10);
    }
    {
        unsigned char *tmp =
            scanBytes((unsigned char *)Server, ServerSz,
                      BYTES_SEARCH_FORMAT("6A016A016A006A01528BCFE8"));
        CPlayerObj_ChangeWeapon = getVal4FromRel(tmp + 12);
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("8D8B????????E8????????8BF88A4424??84C0"));
        CPlayerObj_m_Arsenal = *(unsigned *)(tmp + 2);
        CArsenal_GetAmmoCount = getVal4FromRel(tmp + 7);
    }
    {
        unsigned char *tmp =
            scanBytes((unsigned char *)Server, ServerSz,
                      BYTES_SEARCH_FORMAT("83BF??????????74??????????C2"));
        CPlayerObj_m_ePlayerState = *(unsigned *)(tmp + 2);
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("4874??480F??????????8B??8B??FF??????????"));
        CPlayerObj_DropCurrentWeapon = *(unsigned *)(tmp + 16);
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT(
                "8B????????????89??????????E8????????8B??E8????????"
                "85C00F??????????3A??????????0F??????????8A??????88"));
        if (tmp) {
            CPlayerObj_m_hClient = *(unsigned *)(tmp + 2);
            CPlayerObj_m_eStandingOnSurface = *(unsigned *)(tmp + 9);
            GameClientData_m_nClientMoveCode = *(unsigned *)(tmp + 35);
            CCharacter_m_bOnGround = *(unsigned *)(tmp + 51);
        }
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT(
                "89??????????89??????????89??????????8B??????????E8??????"
                "??84C00F??????????8D????????8D??????????E8"));
        if (tmp) {
            CPlayerObj_m_vLastVelocity = *(unsigned *)(tmp + 2);
            g_pGameServerShell = (void *)*(uintptr_t *)*(unsigned *)(tmp + 20);
            g_pGameServerShell_IsPaused = getVal4FromRel(tmp + 25);
            CPlayerObj_m_PlayerRigidBody = *(unsigned *)(tmp + 44);
            CPlayerObj_m_PlayerRigidBody_Update = getVal4FromRel(tmp + 49);
        }
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("C6??????????01EB0F68????????6A01E8??????"
                                "??83C4088B??????8B??????8B??????89"));
        if (tmp) {
            CPlayerObj_m_bUseLeash = *(unsigned *)(tmp + 2);
            CPlayerObj_m_vLastClientPos = *(unsigned *)(tmp + 38);
        }
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT(
                "D9??????????D8??????????8B??6A208B??D9??????8B442414"));
        if (tmp) {
            CPlayerObj_m_fMoveMultiplier = *(unsigned *)(tmp + 8);
            CPlayerObj_m_fJumpMultiplier =
                CPlayerObj_m_fMoveMultiplier + sizeof(float);
        }
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT(
                "83EC????8B??8B??????????85??0F??????????8B??????????8B"));
        if (tmp) {
            CPlayerObj_TeleportClientToServerPos = (void *)tmp;
        }
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("8B86????????85C08BBE????????7405BFC8000000"));
        if (tmp) {
            CCharacter_m_hLadderObject = *(unsigned *)(tmp + 2);
        }
    }
    {
        unsigned char *tmp =
            scanBytes((unsigned char *)Server, ServerSz,
                      BYTES_SEARCH_FORMAT("8B4708508BCBE8????????84C0"));
        if (tmp) {
            CGrenadeProximity_IsEnemy = getVal4FromRel(tmp + 7);
        }
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("508B44????508BCBE8????????8B44242C"));
        CArsenal_SetAmmo = getVal4FromRel(tmp + 9);
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("508B44????508BCBE8????????8B44242C"));
        CArsenal_SetAmmo = getVal4FromRel(tmp + 9);
    }
    {
        unsigned char *tmp =
            scanBytes((unsigned char *)Server, ServerSz,
                      BYTES_SEARCH_FORMAT(
                          "E8????????8B??????????8B??????8B??89??????6A01"));
        CArsenal_m_hCurWeapon = *(unsigned *)(tmp + 7) - CPlayerObj_m_Arsenal;
        CArsenal_m_pPlayer = CArsenal_m_hCurWeapon - sizeof(uintptr_t);
        CArsenal_GetCurWeapon = getVal4FromRel(tmp + 1);
    }
    {
        unsigned char *tmp =
            scanBytes((unsigned char *)Server, ServerSz,
                      BYTES_SEARCH_FORMAT("8B??????85C0568BF1742A"));
        CArsenal_DecrementAmmo = tmp;
    }

    {
        unsigned char *tmp =
            scanBytes((unsigned char *)Server, ServerSz,
                      BYTES_SEARCH_FORMAT(
                          "8D??????????E8????????8B??????????3B??74??6A01"));
        CPlayerObj_m_Inventory = *(unsigned *)(tmp + 2);
    }

    {
        unsigned char *tmp =
            scanBytes((unsigned char *)Server, ServerSz,
                      BYTES_SEARCH_FORMAT(
                          "FF52??89??????8B????????6A00??8B??????????E8"));
        g_pWeaponDB = (CWeaponDB *)*(uintptr_t *)*(uintptr_t *)(tmp + 17);
        g_pWeaponDB_GetWeaponData = getVal4FromRel(tmp + 22);
    }

    {
        unsigned char *tmp =
            scanBytes((unsigned char *)Server, ServerSz,
                      BYTES_SEARCH_FORMAT("E8????????8B??????????6A006A0068????"
                                          "??????89??????E8????????83"));
        g_pWeaponDB_GetAmmoData = getVal4FromRel(tmp + 1);
        g_pWeaponDB_GetInt32 = getVal4FromRel(tmp + 26);
    }
    {
        unsigned char *tmp = scanBytes(
            (unsigned char *)Server, ServerSz,
            BYTES_SEARCH_FORMAT("6A006A0068????????56E8????????84C07514"));
        g_pWeaponDB_GetBool = getVal4FromRel(tmp + 11);
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
    m_hCatGear = g_pLTDatabase->GetCategory(
        *(HDATABASE *)((unsigned char *)g_pGameDatabaseMgr + 0x20),
        "Arsenal/Gear");
    m_hCatGlobal = g_pLTDatabase->GetCategory(
        *(HDATABASE *)((unsigned char *)g_pGameDatabaseMgr + 0x20),
        "Arsenal/Global");
    m_hRecGlobal = g_pLTDatabase->GetRecordB(m_hCatGlobal, WDB_GLOBAL_RECORD);
    HCATEGORY hGlobalCat = g_pLTDatabase->GetCategory(
        *(HDATABASE *)((unsigned char *)g_pGameDatabaseMgr + 0x20),
        "Character/Models/Global");
    for(int i = 0; i != 13; ++i)
        m_hSlowMoHandles[i] = g_pLTDatabase->GetAttribute(
        g_pLTDatabase->GetCategory(
            *(HDATABASE *)((unsigned char *)g_pGameDatabaseMgr + 0x20),
            "SlowMo"),
        slowmostr[i]);
    m_hGlobalRecord = g_pLTDatabase->GetRecordByIndex(hGlobalCat, 0);
    m_hUnarmedRecord = g_pLTDatabase->GetRecordLink(
        g_pLTDatabase->GetAttribute(m_hRecGlobal, "Unarmed"), 0, 0);
    m_hUnarmedRecordAmmo = g_pLTDatabase->GetRecordLink(
        g_pLTDatabase->GetAttribute(GetWeaponData(m_hUnarmedRecord),
                                    _C("AmmoName")),
        0, 0);
}

void SdkHandler::initClient() {
    AppHandler &handler = *m_appHandler;
    SpliceHandler &hsplice = *handler.spliceHandler();
    uint8_t *gClient = handler.m_Client;
    unsigned char *gFearExe = handler.m_Exec;
    uintptr_t gFearExeSz = handler.m_ExecSz;
    uintptr_t gClientSz = handler.m_ClientSz;
    handler.m_database = (unsigned char *)GetModuleHandle(L"GameDatabase");
    handler.m_databaseSz = GetModuleSize((HMODULE)handler.m_database);
    g_pLTDatabase = getDatabaseMgr();
    getLTServerClient(gFearExe, gFearExeSz);
    {
        void *tmp = scanBytes(
            (unsigned char *)gClient, gClientSz,
            BYTES_SEARCH_FORMAT("8B0D????????83C40481C1????????56894C242CE8"));
        g_pInterfaceMgr = (CInterfaceMgr *)*(uintptr_t *)*(
            uintptr_t *)((unsigned char *)tmp + 2);
        while (true)
            if (!g_pInterfaceMgr->GetGameState())
                Sleep(50);
            else
                break;
    }
    printf("clientfx");
    handler.serverPreinitPatches();
    {
        uint8_t *tmp = scanBytes(
            (uint8_t *)gClient, gClientSz,
            BYTES_SEARCH_FORMAT("B9????????E8????????A1????????33F6"));
        auto clientfx_ptr = *reinterpret_cast<uint32_t *>(tmp+1);
        tmp = reinterpret_cast<uint8_t *>(*reinterpret_cast<uint32_t *>(clientfx_ptr+0x20));
        auto modSz = GetModuleSize(tmp);
        auto currentTmp = tmp;
        for(int i = 0; i != 2; ++i){
            currentTmp = scanBytes(
                (unsigned char *)currentTmp, modSz-(currentTmp-tmp),
                BYTES_SEARCH_FORMAT("C74018000080BF"));
            auto patch_point = currentTmp+3;
            unprotectCode(patch_point, 4);
            memset(patch_point, 0, 4);
        }
    }
    {
        void *tmp =
            scanBytes((unsigned char *)gClient, gClientSz,
                      BYTES_SEARCH_FORMAT("8B??????????85C95774??8B11FF????"));
        ClientConnectionMgr_IGameSpyBrowser =
            *(uintptr_t *)((unsigned char *)tmp + 2);
    }

    {
        void *tmp = scanBytes(
            (unsigned char *)gClient, gClientSz,
            BYTES_SEARCH_FORMAT(
                "8B0D????????5053E8????????8AD88B44240CF6DB1ADB8D4C2410"));
        g_pClientConnectionMgr =
            (ClientConnectionMgr *)*(uintptr_t *)((unsigned char *)tmp + 2);
    }
    {
        unsigned char *tmp = (unsigned char *)(uintptr_t *)(scanBytes(
            (unsigned char *)gClient, gClientSz,
            BYTES_SEARCH_FORMAT(
                "892D????????E8????????A1????????508D8D????????E8????????"
                "33DBC685????????01")));
        if (tmp) g_pScreenMultiAdr = tmp + 2;
    }
    hsplice.spliceUp(
        scanBytes((unsigned char *)gClient, gClientSz,
                  BYTES_SEARCH_FORMAT(
                      "8B??68????????E8????????8B????8B??????????8B??33?"
                      "?????68??????????FF")),
        (void *)hookGetScreenMulti);
    if (g_pScreenMultiAdr) {
        g_pScreenMulti_RequestServerDetails =
            scanBytes((unsigned char *)gClient, gClientSz,
                      BYTES_SEARCH_FORMAT("8B44240483EC1885C0744E8038007449"));
    }

    /*spliceUp(scanBytes((unsigned char *)gFearExe, gFearExeSz,
                    (char *)"6A0AFF520883F8010F84"),SdkHandlerUpdate);*/
    /*spliceUp(scanBytes((unsigned char *)gClient, gClientSz,
                    (char
     *)"8B??????81??03020000??8B??????0F"),SdkHandlerUpdate);*/
    if(handler.m_isHttpMaster)
        hsplice.spliceUp(
            scanBytes((unsigned char *)gFearExe, gFearExeSz,
                      BYTES_SEARCH_FORMAT("A1????????81EC????????85C0558BAC")),
            (void *)SdkHandlerUpdate);

    g_doConnectIpAdrExit = reinterpret_cast<uint8_t *>(reinterpret_cast<uintptr_t>(g_doConnectIpAdrTramp) - 0xc);
    if (g_doConnectIpAdrTramp) {
        {
            void *tmp = scanBytes(
                (unsigned char *)gFearExe, gFearExeSz,
                BYTES_SEARCH_FORMAT("6A00??6A00E8????????8B??85??7417"));
            tmp = (unsigned char *)tmp + ((sizeof("6A00??6A00E8") - 1) / 2);
            tmp = (unsigned char *)tmp + (*(uintptr_t *)tmp) + 4;
            unknownFunc2 = tmp;
        }
        getStructFromString1 =
            scanBytes((unsigned char *)gFearExe, gFearExeSz,
                      BYTES_SEARCH_FORMAT("5355568BE98B45145733FF85C07638"));

        {
            void *tmp =
                scanBytes((unsigned char *)gFearExe, gFearExeSz,
                          BYTES_SEARCH_FORMAT(
                              "8B0D????????50E8????????85C08B4C24108901740C"));
            unknownStruct1 = (void *)*(uintptr_t *)((unsigned char *)tmp + 2);
        }
        hsplice.spliceUp(scanBytes((unsigned char *)gFearExe, gFearExeSz,
                      BYTES_SEARCH_FORMAT("83E8028BCE74488B166A01FF12")), (void *)hookOnConnectServer);
    }
    g_pGameDatabaseMgr = (CGameDatabaseMgr *)*(uintptr_t *)*(
        uintptr_t *)(scanBytes((unsigned char *)gClient, gClientSz,
                               BYTES_SEARCH_FORMAT(
                                   "8B0D????????8B41248B1650A1????????"
                                   "508BCEFF526885C08BCF751350E8")) +
                     2);
    m_hModelsCat = g_pLTDatabase->GetCategory(
        *(HDATABASE *)((unsigned char *)g_pGameDatabaseMgr + 0x20),
        "Character/Models");
}

void SdkHandler::Update() {
    AppHandler &handler = *m_appHandler;
    if (*(uintptr_t *)handler.m_gameClientStruct &&
        *(unsigned *)((
            unsigned char *)((*(uintptr_t *)(handler.m_gameClientStruct)) +
                             0x224))) {
        g_pGameSpyBrowser = (IGameSpyBrowser *)*(
            uintptr_t *)((unsigned char *)(*(
                             uintptr_t *)g_pClientConnectionMgr) +
                         ClientConnectionMgr_IGameSpyBrowser);
        if (g_pGameSpyBrowser && (g_pScreenMulti || g_pScreenMultiAdr)) {
            unsigned char *g_pScreenMulti;
            if (this->g_pScreenMulti)
                g_pScreenMulti = this->g_pScreenMulti;
            else
                g_pScreenMulti = (unsigned char *)*(uintptr_t *)*(
                    uintptr_t *)g_pScreenMultiAdr;
            if (!g_pGameSpyBrowser_RequestServerListAdr) {
                int tmp;
                g_pGameSpyBrowser_RequestServerListAdr =
                    ((unsigned char *)(*(uintptr_t *)(g_pGameSpyBrowser)) +
                     0x8);
                unprotectMem(
                    (unsigned char *)g_pGameSpyBrowser_RequestServerListAdr,
                    100);
                g_pGameSpyBrowser_RequestServerList = (void *)*(
                    uintptr_t *)g_pGameSpyBrowser_RequestServerListAdr;
            }
//            _asm{
//                int 3
//            }
            if (g_pScreenMulti &&
                !*((unsigned char *)g_pScreenMulti +
                   0x2F4))  // FEARXP 0x238 INTERNET - 1; LAN - 0
                *(uintptr_t *)g_pGameSpyBrowser_RequestServerListAdr =
                    (uintptr_t)requestMasterServer;
            else
                *(uintptr_t *)g_pGameSpyBrowser_RequestServerListAdr =
                    (uintptr_t)g_pGameSpyBrowser_RequestServerList;
        }
        if (m_isDoGameSpy) {
            unsigned tmp = GetTickCount();
            if (m_timerGamespyServ == 0) {
                m_timerGamespyServ = tmp;
            } else {
                tmp = tmp - m_timerGamespyServ;
                if (tmp > 5000) {
                    g_pGameSpyBrowser = (IGameSpyBrowser *)*(
                        uintptr_t *)((unsigned char *)(*(
                                         uintptr_t *)g_pClientConnectionMgr) +
                                     ClientConnectionMgr_IGameSpyBrowser);
                    if (g_pGameSpyBrowser)
                        *((unsigned char *)g_pGameSpyBrowser + 0x84) = 1;
                    m_timerGamespyServ = 0;
                    m_isDoGameSpy = 0;
                }
            }
        }
    }
}

void SdkHandler::setRespawn(HOBJECT hObjResp) {
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
                switch (StringUtil::hash_rt(lvlName)) {
                case StringUtil::hash_ct("07_ATC_Roof"):
                    newPos = LTVector{1566.47f, 2085.51f, 3938.72f};
                    checkPointState = 1;
                    break;
                case StringUtil::hash_ct("11_Mapes_Elevator"):
                    newPos = LTVector{3085.24f, -400.0f, -3066.04f};
                    checkPointState = 1;
                    break;
                case StringUtil::hash_ct("02_Docks"):
                    newPos = LTVector{-11716.2f, -2833.13f, -10565.7f};
                    checkPointState = 1;
                    break;
                case StringUtil::hash_ct("18_Facility_Upper"):
                    newPos = LTVector{-1340.0f, -1040.0f, -100.0f};
                    checkPointState = 1;
                    break;
                case StringUtil::hash_ct("09_Bishop_Rescue"):
                    newPos = LTVector{18721.3f, 2741.3f, 389.176f};
                    checkPointState = 1;
                    break;
                case StringUtil::hash_ct("12_Badge"):
                    newPos = LTVector{-8808.6f, -30.0f, -3414.9f};
                    checkPointState = 1;
                    break;
                case StringUtil::hash_ct("17_Factory"):
                    newPos = LTVector{-117.221f, -929.996f, -16681.2f};
                    checkPointState = 1;
                    break;
                case StringUtil::hash_ct("08_Admin"):
                    newPos = LTVector{-14026.0f, 2426.0f, 5882.0f};
                    checkPointState = 1;
                    break;
                case StringUtil::hash_ct("XP_CHU01"):
                    newPos = LTVector{-704.0f, -1500.0f, -3072.0f};
                    checkPointState = 1;
                    break;
                case StringUtil::hash_ct("XP2_W01"):
                    newPos = LTVector{15180.0f, 25179.0f, 47028.0f};
                    checkPointState = 1;
                    break;
                case StringUtil::hash_ct("XP_HOS01"):
                    newPos = LTVector{4036.75f, 80.9f, 7598.07f};
                    checkPointState = 1;
                    break;
                case StringUtil::hash_ct("XP_HOS02"):
                    newPos = LTVector{512.0f, 21424.1f, -3840.0f};
                    checkPointState = 1;
                    break;
                case StringUtil::hash_ct("XP2_W06"):
                    newPos = LTVector{-24186.0f, -6460.0f, -65997.0f};
                    checkPointState = 1;
                    break;
                case StringUtil::hash_ct("XP2_W14"):
                    newPos = LTVector{-8651.0f, 990.0f, -4160.0f};
                    checkPointState = 1;
                    break;
                case StringUtil::hash_ct("XP2_W16"):
                    newPos = LTVector{-8972.0f, 2070.0f, -3833.0f};
                    checkPointState = 1;
                    break;
                case StringUtil::hash_ct("XP2_W17"):
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
        HCLIENT playerClient = *(HCLIENT*)((unsigned char
        *)pPlayerObj+0x2880); HOBJECT hPlayerObj =
        GetClientObject(playerClient); g_pLTServer->GetObjectPos(hPlayerObj,
        &checkPointPos); newPos = checkPointPos; checkPointState = 2;
      }*/
        if (checkPointState)
            g_pLTServer->SetObjectPos(startPtObj, newPos);
        // if (checkPointState == 2) checkPointState = 1;
}

void SdkHandler::setExeType(bool state){
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
            (uintptr_t)m_appHandler->m_isMultiplayerGameClient;
    else
        *(uintptr_t *)((unsigned char *)(*(uintptr_t *)g_pLTClient) + 0x1E0) =
            (uintptr_t)m_appHandler->m_isMultiplayerGameClient;
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
                (uintptr_t)m_appHandler->m_isMultiplayerGameClient;
        else
            *(uintptr_t *)((unsigned char *)(*(uintptr_t *)g_pLTServer) + 0x1EC) =
                (uintptr_t)m_appHandler->m_isMultiplayerGameClient;
    }
    if (state) {
        m_appHandler->setMpGame(1);
    } else
        m_appHandler->setMpGame(0);
    m_appHandler->m_bExeType = state;
};

void SdkHandler::requestMasterServerCallback(const char *pBuffer,
                                             const unsigned nBufferLen,
                                             void *pCallbackParam) {
    SdkHandler &hsdk = *ExecutionHandler::instance()->sdkHandler();
    AppHandler &happ = *hsdk.appHandler();
    if (!nBufferLen) {
        hsdk.setMasterServerRequestWorking(false);
        return;
    }
    char *pStr = (char *)pBuffer, *pN = (char *)pBuffer,
         *pEnd = (char *)(pBuffer + nBufferLen);
    while (true) {
        if (!pN) break;
        pN = strstr(pN, "\n");
        if (pN >= pEnd) break;
        if (pN) {
            *pN = 0;
            pN++;
        }
        if (!hsdk.isValidIpAddressPort(pStr)) break;
        hsdk.addInternetServer((char *)pStr);
        pStr = pN;
    }
    hsdk.m_isDoGameSpy = 1;
    *((unsigned char *)hsdk.g_pGameSpyBrowser + 0x84) = 1;
    hsdk.setMasterServerRequestWorking(false);
};

void SdkHandler::requestMasterServer(bool, unsigned short, char const *) {
    SdkHandler &hsdk = *ExecutionHandler::instance()->sdkHandler();
    AppHandler &happ = *hsdk.appHandler();
    if (hsdk.IsMasterServerRequestWorking()) return;
    hsdk.setMasterServerRequestWorking(true);
    *((unsigned char *)hsdk.g_pGameSpyBrowser + 0x84) = 0;
    hsdk.g_pGameSpyBrowser->RequestURLData(
        happ.strMaster(), (void *)requestMasterServerCallback, nullptr);
}

void SdkHandler::addInternetServer(char *str) {
    unsigned char *g_pScreenMulti =
        (unsigned char *)*(uintptr_t *)*(uintptr_t *)g_pScreenMultiAdr;
    ((void(__thiscall *)(void *, char *, unsigned,
                         unsigned))g_pScreenMulti_RequestServerDetails)(
        g_pScreenMulti, str, 0, 1);
}

void SdkHandler::SdkHandlerUpdate() {
    SdkHandler &hsdk = *ExecutionHandler::instance()->sdkHandler();
    hsdk.Update();
}

HRECORD SdkHandler::hookLoadMaps(uintptr_t index, uintptr_t *all) {
    SdkHandler *handler = ExecutionHandler::instance()->sdkHandler();
    if (handler->g_pServerMissionMgrAdr && *handler->g_pServerMissionMgrAdr) {
        CServerMissionMgr *g_pServerMissionMgr =
            *handler->g_pServerMissionMgrAdr;
        *all = handler->g_pLTDatabase->GetNumRecords(handler->m_hMissionCat);
        uintptr_t i = index;
        if (*all == 65) {  // FEARXP2 GOG
            i = m_mapsIndex[index];
            *all = sizeof(m_mapsIndex);
        }
        return handler->g_pLTDatabase->GetRecordByIndex(handler->m_hMissionCat,
                                                        i);
    }
    return nullptr;
}

void SdkHandler::hookGetScreenMulti(SpliceHandler::reg *p) {
    SdkHandler &hsdk = *ExecutionHandler::instance()->sdkHandler();

    hsdk.g_pScreenMulti = (uint8_t *)p->tcx;
}

void SdkHandler::hookCheckpointEvent(SpliceHandler::reg *p) {
    auto &sdk = *ExecutionHandler::instance()->sdkHandler();
        if (p->v0) {
            sdk.g_pLTServer->GetObjectPos((HOBJECT)p->v0,
            &sdk.checkPointPos); sdk.checkPointState = 2;
        }
}

void SdkHandler::hookPatchHoleKillHives(SpliceHandler::reg *p) {
        auto &app = *ExecutionHandler::instance()->appHandler();
        if (app.flagPatchHoleKillHives != -1) {
            auto &sdk = *ExecutionHandler::instance()->sdkHandler();
            app.flagPatchHoleKillHives = 1;
            sdk.checkPointPos = LTVector{-2702.40f, -2900.85f, 686.73f};
            sdk.checkPointState = 2;
        }
        if (app.flagPatchHoleKillHives == 1) {
            p->state = 2;
            p->tflags &= ~(1 << 6); // set not zero flag
        }
}

void SdkHandler::hookOnMapLoaded(SpliceHandler::reg *p) {
    auto &app = *ExecutionHandler::instance()->appHandler();
    auto &sdk = *ExecutionHandler::instance()->sdkHandler();
        if (app.m_bRandWep) {
            srand(time(0));
            app.m_bDisableLights = rand() % 2;
//            sdk.m_currentRandomWeaponInd = 0;
//            unsigned char *in = sdk.m_randomWepTable;
//            int i;
//            for (i = 0; i != 17; i++) {
//                in[i] = i;
//            }
//            shuffle(in, 17);
        }
        ((void(__thiscall *)(void *))sdk.g_pServerVoteMgr_ClearVote)(
            sdk.g_pServerVoteMgr);
        if (app.m_bCoop) {
            char *lvlName = sdk.getCurrentLevelName();
            unsigned skinState = 9;
            // aData->setPatchHoleKillHives(0);
            app.flagPatchHoleKillHives = -1;
            app.setCoopDoSpawn(0);
            sdk.checkPointState = 0;
            if (lvlName) {
                switch (StringUtil::hash_rt(lvlName)) {
                case StringUtil::hash_ct("07_ATC_Roof"):
                    skinState = 0;
                    // aData->setCoopDoSpawn(1);
                    break;
                case StringUtil::hash_ct("02_Docks"):
                    skinState = 0;
                    // aData->setCoopDoSpawn(1);
                    break;
                case StringUtil::hash_ct("XP2_W06"):
                    skinState = 0;
                    app.setCoopDoSpawn(1);
                    break;
                case StringUtil::hash_ct("13_Hives"):
                    app.flagPatchHoleKillHives = 0;
                    // aData->setPatchHoleKillHives(1);
                    break;
                    // default:
                    //  aData->setCoopDoSpawn(1);
                    //  break;
                }

                app.skinState = skinState;
                *app.m_skinStr = (char *)"Player";
                app.m_storyModeCnt = 0;
                if (app.m_preventNoclip)
                    sdk.m_bfreeMovement = 0;
                //*(unsigned short *)(aData->aFreeMovement) = 0x9090;
            }
        }
}

// uintptr_t IPChunk::foreach (nodeData<IPChunk> &Chunk, IPData * Block,
//                            uintptr_t(*func)(uintptr_t index, IPChunk *Chunk,
//                                             IPData *Block)) {
//    int cnt = Chunk.count();
//    uintptr_t result = 0;
//    for (int i = 0; i != cnt; ++i) {
//        IPChunk *p = Chunk.at(i);
//        for (int j = 0; j != p->count; ++j) {
//            result = func(j, p, Block);
//            if (result == -1 || result != 0)
//                return result;
//        }
//        // IPData * block = ipchunk.unused();
//        // block->ip = sin_addr.
//    }
//    return result;
//}

// IPData *IPChunk::add(nodeData<IPChunk> &Chunk, IPData *Block) {
//    int cnt = Chunk.count();
//    for (int i = 0; i != cnt; ++i) {
//        IPChunk *p = Chunk.at(i);
//        for (int j = 0; j != p->count; ++j) {
//            if (p->buf[j].ip == Block->ip) {
//                if (p->buf[j].port != Block->port)
//                    return nullptr;
//                return &p->buf[j];
//            }
//            if (p->buf[j].ip == 0)
//                return &p->buf[j];
//        }
//        if (p->count != IPChunk::countMax) {
//            IPData *next = &p->buf[p->count];
//            ++p->count;
//            return next;
//        }
//    }
//    IPChunk *p = Chunk.add();
//    ++p->count;
//    return &p->buf[0];
//}

//// inline uintptr_t IPChunk::exist(uintptr_t index, IPChunk *Chunk, IPData
//// *Block) {
////   if (Chunk->buf[index].ip == Block->ip)
////     return 1;
////   return 0;
//// }

//// inline uintptr_t IPChunk::clear(uintptr_t index, IPChunk *Chunk, IPData
//// *Block) {
////   if (Chunk->buf[index].ip == Block->ip)
////     return 1;
////   return 0;
//// }

void SdkHandler::hookUDPRecvfrom(SpliceHandler::reg *p) {
    auto &inst = *ExecutionHandler::instance()->sdkHandler();
    if (p->tsi != -1 && p->tsi) {
        sockaddr_in *sock = (sockaddr_in *)(uintptr_t *)(p->tdi + 4);
        EnterCriticalSection(static_cast<CRITICAL_SECTION*>(inst.g_ipchunkSection.get()));
        uint32_t ip = sock->sin_addr.s_addr;
        uint16_t port = htons(sock->sin_port);
        auto& data = inst.m_ipData;
        if(data.find(ip) != data.end()){
            //DBGLOG("%p %p %p\n",data[ip].size(), data[ip].begin()->second, port);
            if(data[ip].find(port) == data[ip].end()){
                p->tsi = -1;
                WSASetLastError(WSAEWOULDBLOCK);
            }
        }
        LeaveCriticalSection(static_cast<CRITICAL_SECTION*>(inst.g_ipchunkSection.get()));
    }
    //    SdkHandler *pSdk = &handleData::instance()->pSdk;
    //    if (p->tsi != -1 && p->tsi) {
    //        sockaddr_in *sock = (sockaddr_in *)(uintptr_t *)(p->tdi + 4);
    //        IPData block = {sock->sin_addr.s_addr, htons(sock->sin_port)};
    //        EnterCriticalSection(&pSdk->g_ipchunkSection);
    //        if (IPChunk::foreach (
    //                pSdk->g_ipchunk, &block,
    //                [](uintptr_t index, IPChunk *Chunk, IPData *Block) ->
    //                uintptr_t {
    //                    if (Chunk->buf[index].ip == Block->ip) {
    //                        if (Chunk->buf[index].port == Block->port)
    //                            return 2;
    //                        return 1;
    //                    }
    //                    return 0;
    //                }) == 1) {
    //            p->tsi = -1;
    //            WSASetLastError(WSAEWOULDBLOCK);
    //        }
    //        LeaveCriticalSection(&pSdk->g_ipchunkSection);
    //    }
}

void SdkHandler::hookUDPConnReq(SpliceHandler::reg *p) {
    auto &inst = *ExecutionHandler::instance()->sdkHandler();
    sockaddr_in *sock = *(sockaddr_in **)((uintptr_t)(&p->retadr) + 0xC0);
    //        SdkHandler *pSdk = &handleData::instance()->pSdk;
    {
            EnterCriticalSection(static_cast<CRITICAL_SECTION*>(inst.g_ipchunkSection.get()));
            //IpData data = {sock->sin_addr.s_addr};//, sock->sin_port};
            std::unordered_map<uint16_t, uint32_t> data;
            uint16_t port = htons(sock->sin_port);
            data[port] = inst.getRealTimeMS();
            inst.m_ipData[sock->sin_addr.s_addr] = data;
            //DBGLOG("new connect port %p %p\n", port, inst.m_ipData[sock->sin_addr.s_addr].begin()->second);
            LeaveCriticalSection(static_cast<CRITICAL_SECTION*>(inst.g_ipchunkSection.get()));
    }
    //        sockaddr_in *sock = *(sockaddr_in **)((uintptr_t)(&p->retadr) +
    //        0xC0); IPData block = {sock->sin_addr.s_addr,
    //        htons(sock->sin_port),
    //                        pSdk->getRealTimeMS()};
    //        IPData *data = IPChunk::add(pSdk->g_ipchunk, &block);
    //        data->ip = block.ip;
    //        data->port = block.port;
    //        data->timestamp = block.timestamp;
    //        LeaveCriticalSection(&pSdk->g_ipchunkSection);
}

void SdkHandler::hookCheckUDPDisconnect(SpliceHandler::reg *p) {
        SdkHandler &inst = *ExecutionHandler::instance()->sdkHandler();
        sockaddr_in *sock = (sockaddr_in *)(p->tsi + 0x94);
        {
        EnterCriticalSection(static_cast<CRITICAL_SECTION*>(inst.g_ipchunkSection.get()));
        uint32_t ip = sock->sin_addr.s_addr;
        uint16_t port = sock->sin_port;
        uint32_t timestamp = inst.getRealTimeMS();
        auto& data = inst.m_ipData;
        if(data.find(ip) != data.end() && data[ip].find(port) != data[ip].end()){
            uint32_t delta = timestamp - data[ip][port];
            if(delta > 10000 || data.count(ip) > 1) {
                p->tax = 1;
                p->state = 2;
            }
        } else {
            for(auto it = data.begin(); it != data.end();) {
                auto key = it->first;
                bool erased = false;
                for(auto item : data[key]) {
                    uint32_t delta = timestamp - item.second;
                    if(delta > 30000) {
                        it = data.erase(it);
                        erased = true;
                        break;
                    }
                }
                if(erased)
                    continue;
                ++it;
            }
//            uint32_t buf[1024];
//            int index = 0;
//            auto itEnd = data.end();
//            for(auto it = data.begin(); it != itEnd; ++it){
//                auto key = it->first;
//                for(auto item : data[key]){
//                    uint32_t delta = timestamp - item.second;
//                    if(delta > 30000) {
//                        buf[index] = key;
//                        ++index;
//                    }
//                    if(index>1000)
//                        break;
//                }
//                if(index>1000)
//                    break;
//            }
//            for(int i = 0; i != index; ++i){
//                data.erase(buf[index]);
//            }
        }
        LeaveCriticalSection(static_cast<CRITICAL_SECTION*>(inst.g_ipchunkSection.get()));
        }
        //if()
        //IPData block = {htons(sock->sin_port), inst.getRealTimeMS()};
        //uint32_t delta = block.timestamp - item.timestamp;
        //IPData& pBlock = inst.m_ipData[sock->sin_addr.s_addr];
        //if(block.port == pBlock.port)
//                                Chunk->buf[index].ip) {
//                                    if (Block->port == Chunk->buf[index].port) {
//                                        if (delta > 10000)
//                                            return 1;
//                                        return 0;
//                                    }
//                                }
//                                if (delta > 30000 // 300000
//                                    ) {
//                                    Chunk->buf[index].ip = 0;
//                                }
//                                return 0;
//                            }) == 1) {
//                        p->tax = 1;
//                        p->state = 2;
//                    LeaveCriticalSection(&pSdk->g_ipchunkSection);
        //}
//        if (IPChunk::foreach (
//                pSdk->g_ipchunk, &block,
//                [](uintptr_t index, IPChunk *Chunk, IPData *Block) ->
//                uintptr_t {
//                    uint32_t delta = Block->timestamp -
//                    Chunk->buf[index].timestamp; if (Block->ip ==
//                    Chunk->buf[index].ip) {
//                        if (Block->port == Chunk->buf[index].port) {
//                            if (delta > 10000)
//                                return 1;
//                            return 0;
//                        }
//                    }
//                    if (delta > 30000 // 300000
//                        ) {
//                        Chunk->buf[index].ip = 0;
//                    }
//                    return 0;
//                }) == 1) {
//            p->tax = 1;
//            p->state = 2;
//        }
//        LeaveCriticalSection(&pSdk->g_ipchunkSection);
}

///// 100175EC crash ebx 0
void regcall SdkHandler::hookRespawnStartPoint(SpliceHandler::reg *p) {
    SdkHandler &handler = *ExecutionHandler::instance()->sdkHandler();
        handler.startPtObj = (HOBJECT) * (uintptr_t *)((unsigned char *)p->tax +
        8); if (handler.startPtObj)
            handler.setRespawn(handler.startPtObj);
}

void SdkHandler::hookOnConnectServer(
    SpliceHandler::reg *p) {  // only FEAR.exe 1.08
    //    SdkHandler *pSdk = &handleData::instance()->pSdk;
    SdkHandler &handler = *ExecutionHandler::instance()->sdkHandler();
    // if(p->tax-2 = 0 )
    //     return;
    DBGLOG("current %p", p->tax)
    unsigned char *structEsi = (unsigned char *)p->tsi,
                  *structEdi = (unsigned char *)p->tbp,
                  *structEbp = (unsigned char *)p->tdi,
                  state = (unsigned char)p->tax;
    if (state == 1) {
        unsigned char *struct2 = ((unsigned char *(
            __thiscall *)(void *, char *))(void *)handler.getStructFromString1)(
            ((unsigned char *)(*(uintptr_t *)handler.unknownStruct1) + 0x198),
            (char *)"internet");
        if (struct2) {
            bool flag = *(uintptr_t *)((unsigned char *)structEdi + 0x68) == -1;
            unsigned char *struct3 = (unsigned char *)*(uintptr_t *)struct2;
            auto status = ((uintptr_t(__thiscall *)(void *, bool, unsigned char *)) *
                  (uintptr_t *)((unsigned char *)struct3 + 0x54))(
                    struct2, flag, structEdi + 0x6C);
            DBGLOG("%p tramp return? status %p", p->tax, status)
            if (status == 0) {
                *(uintptr_t *)((unsigned char *)structEsi + 0x1C) =
                    (uintptr_t)struct2;
                p->origFunc = (uintptr_t)hookOnConnectServerRet;
                DBGLOG("%p tramp return", p->tax)
            } else {
                p->origFunc = (uintptr_t)handler.g_doConnectIpAdrExit;
                p->tax = status;
            }
        }
    }// else if (state == 0) {
    //    return;
    //}
    else if (!state) {
        unsigned char *struct4 = ((unsigned char *)(*(
            uintptr_t *)((unsigned char *)structEbp + 0x1D8)));
        if (struct4) {
            unsigned char *struct5 = (unsigned char *)*(uintptr_t *)struct4;
            if (!((bool(__thiscall *)(void *, unsigned char *))(void *)*(
                    uintptr_t *)((unsigned char *)struct5 + 0x30))(
                    struct4,
                    (unsigned char *)((unsigned char *)structEdi + 0xD4))) {
                ((bool(__thiscall *)(void *, unsigned char *, unsigned char *,
                                     unsigned))(void *)handler.unknownFunc2)(
                    structEsi, struct4, structEdi, 1);
                auto status = *((unsigned char *)structEdi + 4);
                if (status == 0) {
                    p->origFunc = (uintptr_t)hookOnConnectServerRet;
                    DBGLOG("%p tramp return 2", p->tax)
                } else {
                    p->origFunc = (uintptr_t)handler.g_doConnectIpAdrExit;
                    p->tax = status;
                }
            }
        }
    }
    DBGLOG("%p return 32", p->tax)
}

void SdkHandler::hookUseSkin1(SpliceHandler::reg *p) {
    auto &app = *ExecutionHandler::instance()->appHandler();
    auto &sdk = *ExecutionHandler::instance()->sdkHandler();
    if (!app.skinState) {
      p->tax = (uintptr_t)sdk.getModelStruct((char *)"Player");
      *app.m_skinStr = (char *)"Player";
      app.skinState++;
    } else {
      p->tax = (uintptr_t)sdk.getModelStruct((char *)"DeltaForce_multi");
      *app.m_skinStr = (char *)"DeltaForce_multi";
    }
}

void SdkHandler::hookLoadMaps1(SpliceHandler::reg *p) {
    p->tax =
        (uintptr_t)ExecutionHandler::instance()->sdkHandler()->hookLoadMaps(
            p->tax, &p->tbx);
    p->state = 2;
}

void SdkHandler::hookLoadMaps2(SpliceHandler::reg *p) {
    p->tax =
        (uintptr_t)ExecutionHandler::instance()->sdkHandler()->hookLoadMaps(
            p->tdi, &p->v5);
    p->state = 2;
}

void SdkHandler::hookCGrenadeProximity_HandleActivateMsg(
    SpliceHandler::reg *p) {
    // printf("%p\n",p->tcx);
    // SdkHandler *pSdk = &handleData::instance()->pSdk;
    SdkHandler &handler = *ExecutionHandler::instance()->sdkHandler();
    CPlayerObj *pPlayerObj = (CPlayerObj *)p->tcx;
    if (((bool(__thiscall *)(uintptr_t,
                             HOBJECT))handler.CGrenadeProximity_IsEnemy)(
            p->tdi, pPlayerObj->m_hObject)) {
        p->state = 2;
        p->tax = 0;
        p->tflags |= 1 << 6;  // set zero flag
    }
}

char *SdkHandler::getCurrentLevelNameStatic() {
    auto &inst = *ExecutionHandler::instance()->sdkHandler();
    return inst.getCurrentLevelName();
}

SdkHandler::SdkHandler() {}

SdkHandler::~SdkHandler() {}
