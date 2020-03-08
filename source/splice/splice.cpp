#include "pch.h"

//#include "shared/unordered_map.hpp"
#include "splice/splice.h"
#include "splice/splicealloc.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
extern "C" void trampoline();

uint32_t SpliceHandler::getOpcodeLen(void *adr, DIZAHEX_STRUCT *mdiza) {
    DIZAHEX_STRUCT diza = {};
#if defined _M_X64
    diza.mode = DISASM_MODE_64;
#else
    diza.mode = DISASM_MODE_32;
#endif
    int len = dizahex_disasm(static_cast<uint8_t *>(adr), &diza);
    if (mdiza) {
        memcpy(mdiza, &diza, sizeof(DIZAHEX_STRUCT));
    }
    return static_cast<uint32_t>(len);
}

SpliceHandler::tramp *SpliceHandler::createTramp(uint8_t *hookPoint) {
    tramp *pt;
    pt = static_cast<tramp *>(AllocateBuffer(hookPoint));
    if (!pt) return nullptr;
    {
        if (!VirtualProtect(static_cast<void *>(hookPoint), 32,
                            PAGE_EXECUTE_READWRITE,
                            reinterpret_cast<LPDWORD>(&pt->origProtect)))
            return nullptr;
    }
    pt->inuse = false;
    pt->hookPoint = hookPoint;
    m_data[hookPoint] = pt;
    return pt;
}

SpliceHandler::tramp *SpliceHandler::getTramp(void *hookPoint) {
    if (m_data.find(hookPoint) != m_data.end()) {
        return m_data[hookPoint];
    }
    return nullptr;
}

void SpliceHandler::freeTramp(void *hookPoint) {
    tramp *pt = m_data[hookPoint];
    m_data.erase(hookPoint);
    FreeBuffer(pt);
}

uint8_t SpliceHandler::checkHookPoint(unsigned char *orig, tramp *pt,
                                      unsigned char *hookPoint) {
    uint32_t origLen = 0;

    if (!orig) orig = pt->codebuf;
    do {
        uint32_t len = getOpcodeLen(hookPoint);
        if (!len) return 0;

        if (5 == len && (0xE8 == *hookPoint || 0xE9 == *hookPoint)) {
            pt->codebuf[origLen] = *hookPoint;
            uint8_t *dest =
                (hookPoint + 5) + *reinterpret_cast<uint32_t *>(hookPoint + 1);
            uint32_t offs = static_cast<uint32_t>(dest - (orig + origLen + 5));
            *reinterpret_cast<uint32_t *>(pt->codebuf + origLen + 1) = offs;
        }
#ifdef _M_X64
        else if (7 == len && (0x8D48 == *reinterpret_cast<uint16_t *>(
                                            hookPoint))) {  // lea x,[rip+x]
            *reinterpret_cast<uint32_t *>(&pt->codebuf[origLen]) =
                *reinterpret_cast<uint32_t *>(hookPoint);
            uint8_t *dest = (hookPoint + 7) +
                            *reinterpret_cast<uint32_t *>((hookPoint + 3));
            uint32_t offs = static_cast<uint32_t>(dest - (orig + origLen + 7));
            *reinterpret_cast<uint32_t *>((pt->codebuf + origLen + 3)) = offs;
        } else if (6 == len &&
                   (0x8B == *reinterpret_cast<uint8_t *>(hookPoint) ||
                    0x15FF == *reinterpret_cast<uint16_t *>(hookPoint) ||
                    0x25FF == *reinterpret_cast<uint16_t *>(
                                  hookPoint))) {  // mov x,[rip+x] & call [x]
            *reinterpret_cast<uint16_t *>(&pt->codebuf[origLen]) =
                *reinterpret_cast<uint16_t *>(hookPoint);
            uint8_t *dest =
                (hookPoint + 6) + *reinterpret_cast<uint32_t *>(hookPoint + 2);
            uint32_t offs = static_cast<uint32_t>(dest - (orig + origLen + 6));
            *reinterpret_cast<uint32_t *>(pt->codebuf + origLen + 2) = offs;
        } /*else if(6 == len && (0x25FF == *(uint16_t *)hookPoint)){// jmp [x]
       *(uint16_t*)&pt->codebuf[origLen] = *(uint16_t*)hookPoint;
       unsigned char *dest = (hookPoint + 6) + *(unsigned *)(hookPoint + 2);
       hookPoint=(unsigned char*)*(uintptr_t*)dest;
       origLen=0;
       continue;
     }*/
#endif
        else {
            if (1 == len && (0xC3 == *hookPoint || 0xCB == *hookPoint) &&
                origLen + 1 < 5)
                return 0;

            else if (3 == len && (0xC2 == *hookPoint || 0xCA == *hookPoint) &&
                     origLen + 3 < 5)
                return 0;

            memcpy(pt->codebuf + origLen, hookPoint, len);
        }
        hookPoint += len;
        origLen += len;

    } while (origLen < 5);

    pt->origLen = static_cast<uint8_t>(origLen);
    return 1;
}

SpliceHandler::tramp * SpliceHandler::spliceUp(void *hookPoint, void *hookFunc) {
    if (!hookPoint) return nullptr;
    tramp *pt = getTramp(hookPoint);
    if (pt) return pt;
    pt = createTramp(static_cast<uint8_t *>(hookPoint));
    if (!pt) return nullptr;
    if (!checkHookPoint(nullptr, pt, static_cast<uint8_t *>(hookPoint))) {
        freeTramp(hookPoint);
        return nullptr;
    }
    if (hookFunc != nullptr)
        pt->hookFunc = static_cast<uint8_t *>(hookFunc);
    else
        pt->hookFunc = nullptr;
    pt->origFunc = pt->codebuf;
    uint32_t origLen = pt->origLen;
#if defined _M_X64
    *reinterpret_cast<uint16_t *>(static_cast<uint8_t *>(pt->codebuf) +
                                  pt->origLen) = 0x25FF;
    *reinterpret_cast<unsigned long *>(static_cast<uint8_t *>(pt->codebuf) +
                                       pt->origLen + 2) = 0;

    *reinterpret_cast<uintptr_t *>(static_cast<uint8_t *>(pt->codebuf) +
                                   pt->origLen + 6) =
        reinterpret_cast<uintptr_t>(static_cast<uint8_t *>(hookPoint) +
                                    origLen);
#else
    {
        uint8_t *adr = pt->codebuf + pt->origLen;
        *(adr) = 0xE9;
        *(uint32_t *)(adr + 1) =
            (static_cast<uint8_t *>(hookPoint) + pt->origLen) -
            (pt->codebuf + origLen + 5);
    }
#endif
    unsigned rel32;

#if defined _M_X64
    rel32 = static_cast<uint32_t>(((static_cast<uint8_t *>(pt->jmpbuf) -
                                    static_cast<uint8_t *>(hookPoint)) -
                                   5));
    //*(unsigned*)(pt->jmpbuf)=0x58d4850;
    //*(unsigned*)(pt->jmpbuf+4)=0xFFFFFFC4;//-0x34
    *reinterpret_cast<uintptr_t *>(pt->jmpbuf) = 0x158D48E024548948;
    *reinterpret_cast<uintptr_t *>(pt->jmpbuf + 8) = 0x000025FFFFFFFFba;
    //*(unsigned short *)(pt->jmpbuf+8) = 0x25FF;
    //*(unsigned long *)((char *)pt->jmpbuf + 13) = 0;
    *reinterpret_cast<uintptr_t *>(static_cast<uint8_t *>(pt->jmpbuf) + 18) =
        reinterpret_cast<uintptr_t>(trampoline);
#else
    rel32 = (((unsigned char *)pt->jmpbuf - (unsigned char *)hookPoint) - 5);
    *(unsigned *)(pt->jmpbuf) = 0xF0245489;
    *(unsigned char *)(pt->jmpbuf + 4) = 0xBA;
    *(uintptr_t *)(pt->jmpbuf + 5) = (uintptr_t)pt;
    *(pt->jmpbuf + 9) = 0xE9;
    *(uintptr_t *)(pt->jmpbuf + 10) =
        getRel4FromVal((pt->jmpbuf + 10), trampoline);
#endif
    *(unsigned char *)(hookPoint) = 0xE8;
    *(unsigned *)((unsigned char *)hookPoint + 1) = rel32;
    uintptr_t oldProtect;
    VirtualProtect((void *)hookPoint, 32, pt->origProtect,
                   (LPDWORD)&oldProtect);
    return pt;
}
uint8_t regcall SpliceHandler::spliceDown(void *hookPoint) {
    tramp *pt = getTramp(hookPoint);
    if (!pt) return 0;
    // if(IsExecutableAddress((void *)hookPoint)){
    {
        uintptr_t oldProtect;
        if (VirtualProtect((void *)hookPoint, 32, PAGE_EXECUTE_READWRITE,
#ifdef _WIN32
                           (LPDWORD)&oldProtect
#else
                           0
#endif
                           )) {
            checkHookPoint((unsigned char *)hookPoint, pt,
                           (unsigned char *)pt->codebuf);
            memcpy(hookPoint, pt->codebuf, pt->origLen);
            VirtualProtect((void *)hookPoint, 32, pt->origProtect,
#ifdef _WIN32
                           (LPDWORD)&oldProtect
#else
                           0
#endif
            );
        }
    }
    freeTramp(hookPoint);
    return 1;
}

void SpliceHandler::blockAllTramp() {
    for (auto item : m_data) {
        uintptr_t oldProtect;
        if (VirtualProtect(static_cast<void *>(item.second->hookPoint), 32,
                           PAGE_EXECUTE_READWRITE,
#ifdef _WIN32
                           reinterpret_cast<LPDWORD>(&oldProtect)
#else
                           0  // dont request for old protection value
#endif
                               )) {
            *reinterpret_cast<uint16_t *>(item.second->hookPoint) = 0xFEEB;
            VirtualProtect(static_cast<void *>(item.second->hookPoint), 32,
                           item.second->origProtect,
#ifdef _WIN32
                           reinterpret_cast<LPDWORD>(&oldProtect)
#else
                           0
#endif
            );
        }
    }
}
void SpliceHandler::freeAllTramp() {
    while(getTrampCount()) {
        auto& it = *m_data.begin();
        void * key = it.first;
        tramp * item = it.second;
        if (!item->inuse) {
            spliceDown(item->hookPoint);
            m_data.erase(key);
        }
    }
}
void SpliceHandler::freeSplice() {
    while (getTrampCount()) {
        blockAllTramp();
        freeAllTramp();
    }
}

SpliceHandler::~SpliceHandler()
{
    printf("exiting...");
    freeSplice();
}
