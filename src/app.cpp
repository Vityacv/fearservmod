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
unsigned char *doConnectIpAdrTramp;
void regcall hookChangeStr(reg *p) {
  p->state = 2;
  memcpy((void *)p->tax, "fear", sizeof("fear"));
}

bool regcall getCfgInt(char *pathCfg, char *valStr) {
  return GetPrivateProfileIntA("Patches", valStr, 0, pathCfg);
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
  if (bPatchHoleKillHives != state) {
    memswap((unsigned char *)aPatchHoleKillHives,
            (unsigned char *)(const unsigned char[]){0xEB}, 1);
    bPatchHoleKillHives = !bPatchHoleKillHives;
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
hookOnConnectServerRet() {  //-fomit-frame-pointer required :troll:
  //appData *aData = &handleData::instance()->aData;
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
  *(unsigned short *)(aData->aFreeMovement) = 0x09EB;
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
    if (hash_rta(lvlName) == hash_ct("XP2_W01")) {
      return;  // no one will notice this xD
    }
  }
  *(unsigned short *)(aData->aFreeMovement) = 0x9090;
}

void regcall hookFilterObjMessage(reg *p) {
  p->state = 1;
  p->argcnt = 2;
  unsigned val = (*(unsigned char *)((unsigned char *)p->v1 + 0x10)) >> 4;
  switch (val) {
    case 0xA:
      return;
      break;
    case 0x5:
      return;
      break;
    case 0x4:
      return;
      break;
  }
  p->state = 0;
  p->argcnt = 0;
}

void appData::configHandle() {
  unsigned char moveax0[] = {0xB8, 0x00, 0x00, 0x00, 0x00};
  if (bCoop) {
    bPreventNoclip = 1;
    // bIgnoreUnusedMsgID=0;
    bSyncObjects = 1;
    bBotsMP = 1;

    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          (char
               *)"8B??????85????8B??74??8B??????????85??74??3B??74??68");  // story
                                                                           // mode
                                                                           // on
      if (tmp) {
        spliceUp(tmp, (void *)hookStoryModeOn);
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"8B??8B??????????85??75??68????????6A00E8????????");
      if (tmp) {
        spliceUp(tmp, (void *)hookStoryModeOff);
      }
    }

    spliceUp(
        scanBytes((unsigned char *)gServer, gServerSz,
                  (char *)"6A00????FF??????????8B??85??74??E8????????84??74"),
        (void *)fearData::hookLoadMaps1);
    spliceUp(scanBytes((unsigned char *)gServer, gServerSz,
                       (char *)"5357??FF??????????3B??8B??0F??????????8B???????"
                               "???8B??68??????????FF????8B????????????FF"),
             (void *)fearData::hookLoadMaps2);

    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"8B??????????F6????74????8D??????????8B");
      if (tmp) {
        unprotectCode(tmp);
        *(tmp + 9) = 0xEB;
      }
    }
    {
      unsigned char *tmp = scanBytes((unsigned char *)gServer, gServerSz,
                                     (char *)"83E0FB68????????8B??89");
      if (tmp) {
        unprotectCode(tmp);
        *(unsigned short *)(tmp + 1) = 0x0BC8;
      }
    }
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          (char
               *)"8B??????83????3B????75??8B??E8??????????????83????C3");  // cutscene
                                                                           // player
                                                                           // numbers
      if (tmp) {
        unprotectCode(tmp);
        *(unsigned short *)(tmp + 10) = 0x9090;
      }
    }

    {
      unsigned char * tmp=scanBytes((unsigned char *)gServer, gServerSz,
                        (char *)"D9????EB??D9??????8B??????????D9??????????89??????68????????8B??E8????????85??74??83??0375??D9??24EB??D9????0C");
      if (tmp) {
        // unprotectCode(tmp);

        //*(unsigned short *)(tmp+1)=0x90E8;
        spliceUp(tmp - 14, (void *)hookDoorTimer);
      }
    }

    {
      unsigned char * tmp=scanBytes((unsigned char *)gServer, gServerSz,
                        (char *)"EB??D9????0C8B??????????D9??????????68????????8B??89??????E8????????85??74??83????75??D9????EB??D9??????68????????D9??????????8B");
      if (tmp) {
        spliceUp(tmp + 29, (void *)hookDoorTimer);
      }
    }
    {
      unsigned char *tmp = gServer;

      unsigned i = 0;
      while (true) {
        tmp = scanBytes((unsigned char *)tmp, (gServerSz + gServer) - tmp,
                        (char *)"D9??24EB??D9????0C8B??????????D9??????????68");
        if (tmp) {
          spliceUp(tmp - 14, (void *)hookDoorTimer);
        }
        tmp++;
        i++;
        if (i == 2) break;
      }
    }
    {
      unsigned char *tmp = gServer;

      unsigned i = 0;
      while (true) {
        tmp =
            scanBytes((unsigned char *)tmp, (gServerSz + gServer) - tmp,
                      (char *)"508B??FF??????????8B??E8????????88??????????8A");
        if (tmp) {
          spliceUp(tmp, (void *)fearData::hookUseSkin1);
        }
        tmp++;
        i++;
        if (i == 2) break;
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"75??8B??????????68????????E8??????????8B??FF");
      if (tmp) {
        unprotectCode(tmp);
        *(unsigned short *)(tmp) = 0x9090;
        aSkinStr = (char **)(tmp + 9);
        *(uintptr_t *)(tmp + 9) = (uintptr_t) "Player";
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"75??8B????8B????8B??FF??????????8B????8B");
      if (tmp) {
        spliceUp(tmp, (void *)fearData::hookUseSkin1);
      }
    }
    {
      unsigned char * tmp=scanBytes((unsigned char *)gServer, gServerSz,
                        (char *)"75??E8????????84??74??8B??????????8B??6A006A0068??????????FF");
      if (tmp) {
        tmp += 2;
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"75??E8????????84??74??8B??????????8B??6A00");
      if (tmp) {
        tmp += 2;
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }

    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          (char *)"E8????????84C074??8A??????????84C075??83??????75??8B");
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }

    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"E8????????84C074??8D??????68??????????E8");
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char * tmp=scanBytes((unsigned char *)gServer, gServerSz,
                        (char *)"74??0F????8B??????????89??????????EB??A1????????C7??????????010000008B");
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
          (unsigned char *)gEServer, gEServerSz,
          (char *)"83C404F6????74??8B??E8????????6A00E8????????8B??E8");
      if (tmp) {
        spliceUp(tmp, (void *)fearData::hookOnMapLoaded);
      }
    }

    pSdk->fearDataInitServ();

    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          (char *)"8B??85??????????????????C7??????????????C7??????????????74");
      if (tmp) {
        spliceUp(tmp, (void *)fearData::hookRespawnStartPoint);
      }
    }
    {
      unsigned char * tmp=scanBytes((unsigned char *)gServer, gServerSz,
                        (char *)"A1????????8B??????????8B????????????8B????????????E8????????C3");
      if (tmp) {
        spliceUp(tmp, (void *)fearData::hookCheckpointEvent);
      }
    }
    {
      unsigned char *tmp = gServer;
      tmp = scanBytes((unsigned char *)tmp, (gServerSz + gServer) - tmp,
                      (char *)"8A8424A000000084C074");
      tmp += 9;
      unprotectCode(tmp);
      *(unsigned short *)(tmp) = 0x9090;
      tmp = scanBytes((unsigned char *)tmp, (gServerSz + gServer) - tmp,
                      (char *)"8A8424A000000084C074");
      tmp += 9;
      unprotectCode(tmp);
      *(unsigned short *)(tmp) = 0x9090;
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"56E8????????8B??????????E8????????8B??????????8B");
      if (tmp) {
        unprotectCode(tmp);
        *(uintptr_t *)(tmp) = 0x90c301b0;
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"8B????????????8D????????????83C4183B??74");
      if (tmp) {
        spliceUp(tmp + 7, (void *)hookGameMode);
      }
    }
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          (char *)"E8????????84C074??8B0D????????68????????E8????????D9");
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"E8????????84C08B??????????74??68????????EB??68");
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          (char *)"E8????????84C075??8B??????????8B0D??????????6A01E8");
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char * tmp=scanBytes((unsigned char *)gServer, gServerSz,
                        (char *)"E8????????84C08B??????????74??68????????EB??68????????E8????????83EC08");
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"E8????????84C074??8B??????????6A????6A00E8");
      if (tmp) {
        unprotectCode(tmp);
        aSPModeSpawn = tmp;
        setCoopDoSpawn(1);
        // memcpy(tmp,moveax0,5);
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"75??8D??????E8????????8B????8B????8B????89");
      if (tmp) {
        unprotectCode(tmp);
        aPatchHoleKillHives = tmp;
        // memcpy(tmp,moveax0,5);
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"753F8D??????E8????????8B??????????8B");
      if (tmp) {
        unprotectCode(tmp);
        *(tmp) = 0xEB;
        // memcpy(tmp,moveax0,5);
      }
    }
  }
  if (bBotsMP) {
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          (char *)"E8????????84C074??8B??8B??FF??????????84??74??E8????????8A");
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char * tmp=scanBytes((unsigned char *)gServer, gServerSz,
                        (char *)"E8????????84??0F??????????E8????????8A??????????05????????84??74??8B");
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
  }
  if (bPreventNoclip) {
    {
      while (true) {
        // teleport - 83EC????8B??8B??????????85??0F??????????8B??????????8B
        // update player pos end -
        // 8B4C24??8B5424??8B4424??89??????????89??????????89????????????????83
        // upd player middle - 508B????50FF????E9????????D9
        unsigned char *adr =
            scanBytes((unsigned char *)gServer, gServerSz,
                      (char *)"75??68????????8D????????8D??????E8");
        if (!adr) break;
        // aMPMovement1=(adr-7);
        // unprotectCode(aMPMovement1);
        unsigned char * updEnd = scanBytes((unsigned char *)gServer, gServerSz,
                          (char *)"8B4C24??8B5424??8B4424??89??????????89??????????89????????????????83");
        if (!updEnd) break;
        unsigned char *buf;
        {
          unsigned char *tmp = adr + 2;
          buf = (unsigned char *)AllocateBuffer(tmp);
          unsigned char *func = getVal4FromRel(tmp + 15);
          memcpy(buf, tmp, 15);
          *(uintptr_t *)((unsigned char *)buf + 1) = 0x43160000;
          *(uintptr_t *)(buf + 15) = getRel4FromVal((buf + 15), func);
          *(unsigned short *)((unsigned char *)buf + 19) = 0xc388;
          memcpy(buf + 21, tmp, 15);
          *(unsigned *)(buf + 21 + 15) = getRel4FromVal((buf + 21 + 15), func);
          *(buf + 21 + 15 + 4) = 0xE9;
          *(uintptr_t *)((unsigned char *)buf + 41) =
              getRel4FromVal((buf + 41), (tmp + 19));
        }
        unprotectCode(adr);
        *(adr) = 0xE9;
        *(uintptr_t *)(adr + 1) = getRel4FromVal((adr + 1), buf);
        {
          buf += 45;
          *(unsigned short *)(buf) = 0xf989;
          *(unsigned short *)(buf + 2) = 0x016a;
          *(unsigned char *)(buf + 4) = 0xe8;
          {
            unsigned char * adr = scanBytes((unsigned char *)gServer, gServerSz,
                          (char *)"83EC????8B??8B??????????85??0F??????????8B??????????8B");
            if (!adr) break;

            *(uintptr_t *)(buf + 5) = getRel4FromVal((buf + 5), adr);
          }
          *(unsigned char *)(buf + 9) = 0xe9;
          *(uintptr_t *)(buf + 10) = getRel4FromVal((buf + 10), updEnd);
        }
        {
          unsigned char *adr =
              scanBytes((unsigned char *)gServer, gServerSz,
                        (char *)"508B????50FF????E9????????D9");
          if (adr) {
            uintptr_t LadderObjOffset =
                *(uintptr_t *)(scanBytes((unsigned char *)gServer, gServerSz,
                                         (char *)"8B86????????85C08BBE????????"
                                                 "7405BFC8000000") +
                               2);
            memcpy(buf + 14,
                   (unsigned char *)(const unsigned char[]){
                       0x90, 0x90, 0x83, 0xBF, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x74, 0x08},
                   11);
            *(uintptr_t *)(buf + 14 + 4) = LadderObjOffset;
            memcpy(buf + 14 + 11, adr, 8);
            *(unsigned char *)(adr) = 0xe9;
            *(uintptr_t *)(adr + 1) = getRel4FromVal((adr + 1), buf + 14);
            *(unsigned char *)(buf + 14 + 19) = 0xe9;
            aFreeMovement = (buf + 14);
            *(unsigned short *)(aFreeMovement) = 0x9090;
            *(uintptr_t *)(buf + 14 + 20) =
                getRel4FromVal((buf + 14 + 20), adr + 8);
            adr += 8;
            *(uintptr_t *)(adr) = 0x850fdb84;
            *(uintptr_t *)(adr + 4) = getRel4FromVal((adr + 4), updEnd);
            *(unsigned char *)(adr + 8) = 0xE9;
            *(uintptr_t *)(adr + 9) = getRel4FromVal((adr + 9), buf);
          }
        }
        break;
      }
    }
  }
  if (bIgnoreUnusedMsgID) {
    unsigned char *tmp = scanBytes((unsigned char *)gServer, gServerSz,
                                   (char *)"568B??????85F674??8B068BCE");
    unprotectCode(tmp);
    *(uintptr_t *)tmp = 0x900008C2;
    // spliceUp(tmp,(void *)hookFilterObjMessage);
  }
  if (bSyncObjects) {
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          (char *)"E8????????84??0F??????????8B??????????8D????????8B");
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"E8????????8B????8A??88??????8B????????????E8");
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          (char *)"E8????????88??????8D??????????8B??8B??8B????8B????89");
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gServer, gServerSz,
          (char *)"E8????????84C074??8B??????????85C074??33??8A??????????6A00");
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServer, gServerSz,
                    (char *)"E8????????84C074??8B????50E8????????83C40484C074");
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
    {
      unsigned char * tmp=scanBytes((unsigned char *)gServer, gServerSz,
                        (char *)"E8????????84C074??F6??????74??68????????8B??E8????????85C074");
      if (tmp) {
        unprotectCode(tmp);
        memcpy(tmp, moveax0, 5);
      }
    }
  }
}

void appData::configParse(char *pathCfg) {
  bPreventNoclip = getCfgInt(pathCfg, (char *)"PreventNoclip");
  bIgnoreUnusedMsgID = getCfgInt(pathCfg, (char *)"PreventSpecialMsg");
  bSyncObjects = getCfgInt(pathCfg, (char *)"SyncObjects");
  bCoop = getCfgInt(pathCfg, (char *)"CoopMode");
  bBotsMP = getCfgInt(pathCfg, (char *)"BotsMP");
  if (bCoop) {
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gServerExe, gServerExeSz,
                    (char *)"8B??????????????FF????8B??????89??????????8B");
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
  spliceUp(scanBytes((unsigned char *)aData->gServer, aData->gServerSz,
                     (char *)"558BEC83E4??81EC????????????8B????85????89??????"
                             "0F??????????8B????85??0F??????????33C0"),
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

void appData::init() {
  pSdk->aData = this;
  gEServer =
      (unsigned char *)(unsigned char *)GetModuleHandle(_T("engineserver.dll"));
  if (gEServer) {
    gEServerSz = GetModuleSize((HMODULE)gEServer);
    {
      spliceUp(
          scanBytes((unsigned char *)gEServer, gEServerSz,
                    (char *)"84??75????68??????????E8????????8B??83C40885??74"),
          (void *)hookLoadGameServer);
    }
    gServerExe = (unsigned char *)GetModuleHandle(nullptr);
    gServerExeSz = GetModuleSize((HMODULE)gServerExe);
    spliceUp(
        scanBytes(
            (unsigned char *)gServerExe, gServerExeSz,
            (char *)"8D????????????8B??E8????????8B????85C075??E8????????8B"),
        (void *)hookConfigLoad);

    {
      unsigned char *tmp = gEServer;

      unsigned i = 0;
      while (true) {
        tmp = scanBytes((unsigned char *)tmp, (gEServerSz + gEServer) - tmp,
                        (char *)"6E61746E6567??2E67616D657370792E636F6D");
        if (tmp) {
          unprotectMem(tmp);
          *(tmp + 1) = *(tmp + 6);
          *(tmp + 2) = '.';
          memcpy(tmp + 3, "fear-combat.org", sizeof("fear-combat.org"));
        }
        tmp++;
        i++;
        if (i == 2) break;
      }
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gEServer, gEServerSz,
                    (char *)"25732E6D61737465722E67616D657370792E636F6D");
      unprotectMem(tmp);
      *(tmp + 4) = '.';
      memcpy(tmp + 5, "fear-combat.org", sizeof("fear-combat.org"));
    }
    {
      unsigned char * tmp = scanBytes((unsigned char *)gEServer, gEServerSz,
                      (char *)"687474703A2F2F6D6F74642E67616D657370792E636F6D2F6D6F74642F766572636865636B2E6173703F7573657269643D25642670726F6475637469643D25642676657273696F6E756E6971756569643D2573266469737469643D256426756E6971756569643D25732667616D656E616D653D2573");
      unprotectMem(tmp);
      memcpy(tmp + 7, "fear-combat.org", sizeof("fear-combat.org") - 1);
      *(tmp + 22) = '/';
      memcpy(tmp + 23, tmp + 24, 94);
    }
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gEServer, gEServerSz,
                    (char *)"25732E617661696C61626C652E67616D657370792E636F6D");
      unprotectMem(tmp);
      *(tmp + 8) = '.';
      memcpy(tmp + 9, "fear-combat.org", sizeof("fear-combat.org"));
    }
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gEServer, gEServerSz,
          (char *)"8B??????508D??????68??????????FF??????????83C4??8D??????68");
      unprotectCode(tmp);
      *(tmp) = 0x68;
      *(uintptr_t *)(tmp + 1) = (uintptr_t) "fear";
    }
    spliceUp(scanBytes((unsigned char *)gEServer, gEServerSz,
                       (char *)"4084C975??C7??????????FFFFFFFFE8????????0FBE"),
             (void *)hookChangeStr);
  } else {
    wchar_t str[1024];
    gFearExe = (unsigned char *)GetModuleHandle(0);
    gFearExeSz = GetModuleSize((HMODULE)gFearExe);
    {
      unsigned char *tmp =
          scanBytes((unsigned char *)gFearExe, gFearExeSz,
                    (char *)"A1????????8B??????????8B????FF????33");
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
          (char *)"83??????????FD8B??????????8B??68????????FF??????????85??"
                  "74"));  // No intro
      if (tmp) {
        unprotectCode(tmp);
        *(unsigned short *)(tmp + 28) = 0x1EEB;
      }
    }
    {
      unsigned char *tmp = (unsigned char *)(scanBytes(
          (unsigned char *)gClient, gClientSz,
          (char *)"FF????84??75??A1????????6A01??8D????????8B"));  // MOTD
      if (tmp) {
        unprotectCode(tmp);
        *(tmp + 5) = 0xEB;
      }
    }
    {
      void *tmp = (unsigned *)(scanBytes((unsigned char *)gFearExe, gFearExeSz,
                                         (char *)"0F94C16A01518B8E??00000052"));
      if (tmp) {
        unprotectCode((unsigned char *)tmp);
        *(unsigned *)tmp = 0x6a90c931;
      }
    }
    doConnectIpAdrTramp = (unsigned char *)scanBytes(
        (unsigned char *)gFearExe, gFearExeSz,
        (char *)"8BCF89B71C060000E8????????8B871C060000");
    pSdk->fearDataInit();
    {
      unsigned char *tmp = scanBytes(
          (unsigned char *)gFearExe, gFearExeSz,
          (char *)"0F8467010000A1????????85C08BC87522A1????????5068????????E8");
      if (tmp) {
        unprotectCode(tmp);
        *(unsigned short *)(tmp + 2) = 0x007D;
      }
    }
    {
      unsigned char *tmp = (unsigned char *)(scanBytes(
          (unsigned char *)gClient, gClientSz,
          (char *)"74078BCEE8????????8B166A008BCEFF5268"));  // MOTD
      if (tmp) {
        unprotectCode(tmp);
        *(tmp) = 0xEB;
      } else {
        // Extraction Point
        aRunGameModeXP =
            scanBytes((unsigned char *)gClient, gClientSz,
                      (char *)"FF????????????8B????????6A02FF5044");
        spliceUp(aRunGameModeXP, (void *)hookSwitchToModeXP);
        aIsMultiplayerGameClient =
            scanBytes((unsigned char *)gClient, gClientSz,
                      (char *)"A1????????85??74??8B????83??0474??83??0574");
        unprotectCode(aIsMultiplayerGameClient);
      }
    }

    if (doConnectIpAdrTramp) {
      spliceUp(
          scanBytes(
              (unsigned char *)gClient, gClientSz,
              (char *)"E8????????84C074218D4C2404E8????????6A016A008D44240C50"),
          (void *)hookSwitchToSP /*, (void *)6*/);
      void *tmp = scanBytes(
          (unsigned char *)gClient, gClientSz,
          (char *)"E8????????84C0754B8D4C2404E8????????6A016A008D44240C50");
      aIsMultiplayerGameClient = (unsigned char *)((unsigned char *)tmp + 1) +
                                 (*(uintptr_t *)((unsigned char *)tmp + 1)) + 4;
      unprotectCode(aIsMultiplayerGameClient);
      spliceUp(tmp, (void *)hookSwitchToMP);

      // toggleIsMPGame();
    }

    {
      unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
          (unsigned char *)gClient, gClientSz,
          (char *)"A1????????8A887806000084C9740E8A887906000084C974048AC3"));
      if (tmp) {
        aStoryModeStruct = (unsigned char *)*(uintptr_t *)(tmp + 1);
      }
    }
    {
      unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
          (unsigned char *)gClient, gClientSz,
          (char *)"6A00FF??????????8B??????????8B????FF??????????8B"));
      if (tmp) {
        unprotectCode(tmp);
        *(unsigned short *)(tmp) = 0x9050;

        spliceUp(tmp, (void *)hookStoryModeView);
      }
    }
    {
      unsigned char *tmp = (unsigned char *)(unsigned *)(scanBytes(
          (unsigned char *)gClient, gClientSz,
          (char *)"50E8????????8B??E8????????84C00F????????????E8"));
      if (tmp) {
        spliceUp(tmp, (void *)hookClientGameMode);
      }
    }

    {
      unsigned char *tmp = gClient;
      unsigned i = 0;
      while (true) {
        tmp = scanBytes(
            (unsigned char *)tmp, (gClientSz + gClient) - tmp,
            (char *)"E8????????84C075??8B??????????8B??8D????????33??FF");
        if (tmp) {
          unprotectCode(tmp);
          aFlashlight[i] = tmp;
          // memcpy(tmp,moveax0,5);
        }
        tmp++;
        i++;
        if (i == 2) break;
      }
    }
  }
}
