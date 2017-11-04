#include "pch.h"
#include <tlhelp32.h>
#include <imagehlp.h>
#include <psapi.h>
#include <tchar.h>
#include "splice.h"
#include "memory.h"

bool regcall IsMemoryExist(void *adr) {
  MEMORY_BASIC_INFORMATION minfo;
  VirtualQuery(adr, &minfo, sizeof(MEMORY_BASIC_INFORMATION));
  return minfo.State != MEM_FREE;
}

void regcall unprotectCode(unsigned char *adr, unsigned sz) {
  unsigned char *tmp;
  VirtualProtect(adr, 100, PAGE_EXECUTE_READWRITE, (PDWORD)&tmp);
}

void regcall unprotectMem(unsigned char *adr, unsigned sz) {
  unsigned char *tmp;
  VirtualProtect(adr, 100, PAGE_READWRITE, (PDWORD)&tmp);
}

unsigned char *regcall scanBytes(unsigned char *pBuff, uintptr_t pBuffSize,
                                 char *pPattStr) {
  int tmp;
  unsigned char *addr = searchBytes(pBuff, pBuffSize, pPattStr);
  if (!addr) {
    DBGLOG("Pattern not found %p %p %s", pBuff, (void *)pBuffSize, pPattStr);
  }
  return addr;
}

// pBuff - scan buffer
// pBuffSize - buffer size
// pPattStr - pattern string, ?? - byte skip (example: 11 22 ?? 33)
unsigned char *regcall searchBytes(unsigned char *pBuff, uintptr_t pBuffSize,
                                   char *pPattStr) {
  uintptr_t pPattSize;
  unsigned char pPatt[512], pPattMask[512];
  {
    uintptr_t pCur1 = 0, pCur2 = 0;
    unsigned short uSym = 0;
    do {
      char cSym = pPattStr[pCur1];
      if (pCur1 % 2 == 0 && pCur1 != 0) {
        pPatt[pCur2] = (uSym >> 8) << 4 | uSym;
        uSym = 0;
        pCur2++;
      }
      uSym = uSym << 8;
      if (cSym == '?') {
        pPattMask[pCur2] = 1;
        pCur1 += 2;
        continue;
      } else {
        pPattMask[pCur2] = 0;
        cSym >= 'A' ? cSym -= '7' : cSym -= '0';
      }
      uSym = uSym | cSym;
      pCur1++;
    } while (pPattStr[pCur1 - 1]);
    pPattSize = (pCur1 - 1) / 2;
  }
  {
    unsigned char *pBuffEnd = pBuff + pBuffSize,
                  *pPattEnd = pPattSize + pPatt - 1;
    for (unsigned char *pBuffCur = pBuff; pBuffCur != pBuffEnd; pBuffCur++) {
      unsigned char *bMask = pPatt, *pMask = pPattMask, *pData = pBuffCur;
      for (; pData != pBuffEnd; ++pMask, ++pData, ++bMask) {
        if (*pMask == 0 && *pData != *bMask) break;
        if (bMask == pPattEnd) return pBuffCur;
      }
    }
  }
  return 0;
}

void regcall memswap(unsigned char *src, unsigned char *dst, unsigned len) {
  for (unsigned char buf; len--;) {
    buf = *(unsigned char *)(src + len);
    *(unsigned char *)(src + len) = *(unsigned char *)(dst + len);
    *(unsigned char *)(dst + len) = buf;
  }
}

void stdcall FreeLibraryThread(void *hDllHandle) {
  FreeLibraryAndExitThread((HMODULE)hDllHandle, 0);
}

void *regcall memcpyl(void *d, const void *s, size_t n) {
  const char *sc, *se;
  char *dc;

  dc = (char *)d;
  sc = (const char *)s;
  se = sc + n;

  if (se != sc) {
    do {
      *dc++ = *sc++;
    } while (sc < se);
  }
  return (unsigned char *)(d) + n;
}

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

bool regcall isModuleExist(uintptr_t mod) {
  wchar_t str[1024];
  return GetModuleFileName((HMODULE)mod, str, 511) != 0;
}

int regcall floatToInt(float x) { return int(x); }

unsigned regcall floatToUInt32(float x) { return unsigned(x); }

int regcall GetRandomInt(int range) {
  if (range == -1)  // check for divide-by-zero case
  {
    return ((rand() % 2) - 1);
  }

  return (rand() % (range + 1));
}

int regcall GetRandomIntRng(int lo, int hi) {
  if ((hi - lo + 1) == 0)  // check for divide-by-zero case
  {
    if (rand() & 1)
      return (lo);
    else
      return (hi);
  }

  return ((rand() % (hi - lo + 1)) + lo);
}

float regcall GetRandomFloat(float min, float max) {
  float randNum = (float)rand() / RAND_MAX;
  float num = min + (max - min) * randNum;
  return num;
}
