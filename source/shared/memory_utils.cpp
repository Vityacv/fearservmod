#include "pch.h"
#include "shared/string_utils.h"
#include "shared/memory_utils.h"
#include "shared/debug.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

template <typename T>
void *GetProcAddressByHashT(T pOptHdr, HMODULE hMod, unsigned hashName) {
    uintptr_t ExportDirVA = pOptHdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]
                                .VirtualAddress,
              ExportDirSz =
                  pOptHdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
    IMAGE_EXPORT_DIRECTORY *pExportDir =
        (IMAGE_EXPORT_DIRECTORY *)((uintptr_t)hMod + ExportDirVA);
    unsigned int *NameVA =
        (unsigned int *)((uintptr_t)hMod + pExportDir->AddressOfNames);
    uintptr_t funcAddress = pExportDir->AddressOfFunctions;
    uintptr_t ordAddress = pExportDir->AddressOfNameOrdinals;
    intptr_t i = pExportDir->NumberOfNames;
    do {
        i--;
        if (hashName <= 0xFFFF) {
            i = hashName;
            i -= pExportDir->Base;
        } else if (StringUtil::hash_rt((char *)((uintptr_t)hMod + NameVA[i])) ==
                   hashName) {
        } else {
            if (i < 0) break;
            continue;
        }
        {
            unsigned short NameOrdinal =
                ((unsigned short *)((uintptr_t)hMod + ordAddress))[i];
            unsigned int procAddrVA =
                ((unsigned int *)((uintptr_t)hMod + funcAddress))[NameOrdinal];
            {
//            if (procAddrVA > ExportDirVA && ExportDirVA + ExportDirSz) {
//                {
//                    return (void *)GetProcAddress(
//                        hMod, (char *)((uintptr_t)hMod + NameVA[i]));
//                }
//            } else {
                return (void *)((uintptr_t)hMod + procAddrVA);
            }
        }
    } while (i > 0);
    return 0;
}

void *regcall GetProcAddressByHash(void * hMod, unsigned hashName) {
    IMAGE_NT_HEADERS *pNtHdr =
        (IMAGE_NT_HEADERS *)((uintptr_t)hMod +
                             (uintptr_t)(((IMAGE_DOS_HEADER *)hMod)->e_lfanew));
    IMAGE_FILE_HEADER *pFileHdr =
        (IMAGE_FILE_HEADER *)((uintptr_t)pNtHdr +
                              FIELD_OFFSET(IMAGE_NT_HEADERS, FileHeader));
    IMAGE_OPTIONAL_HEADER *pOptHdr =
        (IMAGE_OPTIONAL_HEADER *)((uintptr_t)pNtHdr +
                                  offsetof(IMAGE_NT_HEADERS, OptionalHeader));
    switch (pFileHdr->Machine) {
        case IMAGE_FILE_MACHINE_I386: {
            return GetProcAddressByHashT((IMAGE_OPTIONAL_HEADER32 *)pOptHdr,
                                         static_cast<HMODULE>(hMod), hashName);
        }
        case IMAGE_FILE_MACHINE_AMD64: {
            return GetProcAddressByHashT((IMAGE_OPTIONAL_HEADER64 *)pOptHdr,
                                         static_cast<HMODULE>(hMod), hashName);
        }
    }
    return 0;
}

uint8_t regcall isMemoryExist(void *adr) {
    MEMORY_BASIC_INFORMATION minfo;
    VirtualQuery(adr, &minfo, sizeof(MEMORY_BASIC_INFORMATION));
    return minfo.State != MEM_FREE;
}

uint8_t regcall isModuleExist(uintptr_t mod) {
    wchar_t str[1024];
    return GetModuleFileNameW(reinterpret_cast<HMODULE>(mod), str, 511) != 0;
}

void regcall unprotectCode(uint8_t *adr, uint32_t sz) {
    uint8_t *tmp;
    VirtualProtect(adr, sz, PAGE_EXECUTE_READWRITE,
                   reinterpret_cast<PDWORD>(&tmp));
}

void regcall unprotectMem(uint8_t *adr, uint32_t sz) {
    uint8_t *tmp;
    VirtualProtect(adr, sz, PAGE_READWRITE, reinterpret_cast<PDWORD>(&tmp));
}

//void *regcall memmem(uint8_t *haystack, size_t haystack_len, uint8_t *needle,
//                     size_t needle_len) {
//    uint8_t *begin = haystack;
//    uint8_t *last_possible = begin + haystack_len - needle_len;
//    uint8_t *tail = needle;
//    char point;

//    /*
//     * The first occurrence of the empty string is deemed to occur at
//     * the beginning of the string.
//     */
//    if (needle_len == 0) return (void *)begin;

//    /*
//     * Sanity check, otherwise the loop might search through the whole
//     * memory.
//     */
//    if (haystack_len < needle_len) return NULL;

//    point = *tail++;
//    for (; begin <= last_possible; begin++) {
//        if (*begin == point && !memcmp(begin + 1, tail, needle_len - 1))
//            return (void *)begin;
//    }

//    return NULL;
//}

#endif

void regcall memswap(uint8_t *src, uint8_t *dst, unsigned len) {
    for (unsigned char buf; len--;) {
        buf = *(uint8_t *)(src + len);
        *(uint8_t *)(src + len) = *(uint8_t *)(dst + len);
        *(uint8_t *)(dst + len) = buf;
    }
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
    return (uint8_t *)(d) + n;
}

//int regcall GetRandomInt(int range) {
//    if (range == -1)  // check for divide-by-zero case
//    {
//        return ((rand() % 2) - 1);
//    }

//    return (rand() % (range + 1));
//}

//int regcall GetRandomIntRng(int lo, int hi) {
//    if ((hi - lo + 1) == 0)  // check for divide-by-zero case
//    {
//        if (rand() & 1)
//            return (lo);
//        else
//            return (hi);
//    }

//    return ((rand() % (hi - lo + 1)) + lo);
//}

//float regcall GetRandomFloat(float min, float max) {
//    float randNum = (float)rand() / RAND_MAX;
//    float num = min + (max - min) * randNum;
//    return num;
//}

uint8_t *regcall searchBytes(uint8_t *pBuff, uintptr_t pBuffSize,
                             uint8_t *pPattBuf) {
    size_t pPattSize = *(unsigned short *)(pPattBuf);
    pPattBuf += 2;
    uint8_t *pPattMaskBuf = pPattBuf + pPattSize,
            *pPattEnd = pPattBuf + pPattSize, *pBuffEnd = pBuff + pBuffSize;
    for (uint8_t *pBuffCur = pBuff; pBuffCur != pBuffEnd; pBuffCur++) {
        if (*pBuffCur == *pPattBuf) {
            uint8_t *bMask = pPattBuf, *pData = pBuffCur;
            uintptr_t i = 0;
            while (pData != pBuffEnd) {
                ++i, ++pData, ++bMask;
                if (bMask == pPattEnd) return pBuffCur;
                if ((pPattMaskBuf[(uintptr_t)(i / 8)] & (1 << (i % 8))) == 0 &&
                    *pData != *bMask)
                    break;
            }
        }
    }
    return 0;
}

uint8_t *regcall scanBytes(uint8_t *pBuff, uintptr_t pBuffSize,
                           uint8_t *pPattBuf) {
    uint8_t *addr = searchBytes(pBuff, pBuffSize, pPattBuf);
#ifdef _FORCE_DBGLOG
    if (!addr) {
        char strPattBuf[1024];
        int sz = *reinterpret_cast<uint16_t *>(pPattBuf);
        uint8_t * p = pPattBuf+2;
        for(uintptr_t i = 0; i!= sz; ++i){
            uintptr_t pos = i*2;
            uint8_t var = p[i];
            strPattBuf[pos] = static_cast<char>((var>>4) > 9 ? (var>>4)+0x37 : (var>>4)+0x30);
            strPattBuf[pos+1] = static_cast<char>((static_cast<uint8_t>(var<<4)>>4) > 9  ? (static_cast<uint8_t>(var<<4)>>4)+0x37 : (static_cast<uint8_t>(var<<4)>>4)+0x30);
        }
        strPattBuf[sz*2]=0;
        DBGLOG("Pattern not found %p %p %s", pBuff, (void *)pBuffSize,
               strPattBuf);
    }
#endif
    return addr;
}

uint8_t *searchSkipBytes(uint8_t *pBuff, uintptr_t pBuffSize, ...) {
    va_list vl;
    uint8_t *current = pBuff, *result = pBuff;
    va_start(vl, pBuffSize);
    int i = 0;
    uintptr_t skip;
    do {
        uint8_t *pPatt = va_arg(vl, uint8_t *), *tmp;
        uintptr_t pPattSz = *(unsigned short *)pPatt;
        if (i)
            tmp = scanBytes(current, skip + pPattSz, pPatt);
        else
            tmp = scanBytes(result + 1, (pBuffSize + pBuff) - (result + 1),
                            pPatt);
        skip = va_arg(vl, uintptr_t);
        if (!i) {
            if (tmp) {
                i++;
                current = tmp + pPattSz;
                result = tmp;
            } else {
                return nullptr;
            }
        } else
            current = tmp + pPattSz;
        if (!tmp) break;
        if (!skip && tmp) return result;
    } while (skip);
    va_end(vl);
    return nullptr;
}
