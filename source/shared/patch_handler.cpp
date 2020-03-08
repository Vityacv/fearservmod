#include "pch.h"
#include "shared/common_macro.h"
#include "shared/memory_utils.h"
#include "shared/patch_handler.h"


#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
void PatchHandler::addCode(uint8_t *adr, size_t sz) {
    addPatch(adr, sz, PAGE_EXECUTE_READWRITE);
};


void PatchHandler::codeswap(uint8_t *src,
                                    const uint8_t *dst, uint32_t len) {
    patchDef *pDef = addCodeSwapPatch(src, dst, len, PAGE_EXECUTE_READWRITE);
    memswap(src,
            (uint8_t *)((uint8_t *)pDef + sizeof(patchDef) + len),
            len);
    uint32_t tmp;
    VirtualProtect(src, len, pDef->protect, (PDWORD)&tmp);
    pDef->protect = 0;
}

PatchHandler::patchDef *PatchHandler::addCodeSwapPatch(uint8_t *adr,
                                                       const uint8_t *dst,
                                                       size_t sz,
                                                       uint32_t protect) {
    patchDef *pd = getPatch(adr);
    if (adr && !pd) {
        pd = (patchDef *)malloc(sizeof(patchDef) + sz * 2);
        VirtualProtect(adr, sz, protect, (PDWORD)&pd->protect);
        memcpy((uint8_t *)pd + sizeof(patchDef), adr, sz);
        memcpy((uint8_t *)pd + sizeof(patchDef) + sz, dst, sz);

        pd->adr = adr;
        pd->sz = sz;
        m_data[adr] = pd;
    } else if (!pd->protect) {
        VirtualProtect(adr, sz, protect, (PDWORD)&pd->protect);
    }
    return pd;
}

void PatchHandler::addPatch(uint8_t *adr, size_t sz, uint32_t protect) {
    patchDef *pdef = getPatch(adr);
    if (adr && !pdef) {
        pdef = (patchDef *)malloc(sizeof(patchDef) + sz);
        VirtualProtect(adr, sz, protect, (PDWORD)&pdef->protect);
        memcpy((uint8_t *)pdef + sizeof(patchDef), adr, sz);
        pdef->adr = adr;
        pdef->sz = sz;
        m_data[adr] = pdef;
    } else if (!pdef->protect) {
        VirtualProtect(adr, sz, protect, (PDWORD)&pdef->protect);
    }
}

void PatchHandler::freePatch(patchDef *pd) {
    MEMORY_BASIC_INFORMATION minfo;
    VirtualQuery(pd->adr, &minfo, sizeof(MEMORY_BASIC_INFORMATION));
    if (minfo.State != MEM_FREE) {
        uint32_t tmp;
        VirtualProtect(pd->adr, pd->sz, PAGE_EXECUTE_READWRITE, (PDWORD)&tmp);
        memcpy(pd->adr, (uint8_t *)pd + sizeof(patchDef), pd->sz);
        if (pd->protect) tmp = pd->protect;
        VirtualProtect(pd->adr, pd->sz, tmp, (PDWORD)&tmp);
    }
    free(pd);
}

PatchHandler::~PatchHandler()
{
    restorePatches();
}

void PatchHandler::restorePatch(uint8_t *adr) {
    freePatch(m_data[adr]);
    m_data.erase(adr);
}

void PatchHandler::restoreProtection() {
    while (getPatchCount()) {
        auto& it = *m_data.begin();
        patchDef &item = *it.second;
        if (item.protect) {
            uint32_t protect;
            VirtualProtect(item.adr, item.sz, item.protect, (PDWORD)&protect);
            item.protect = 0;
        }
    }
}

void PatchHandler::restorePatches() {
    while (getPatchCount()) {
        auto& it = *m_data.begin();
        uint8_t * key = it.first;
        patchDef &item = *it.second;
        freePatch(&item);
        m_data.erase(key);
    }
}

int PatchHandler::getPatchCount() { return m_data.size(); }

PatchHandler::patchDef *PatchHandler::getPatch(uint8_t *adr) {
    if (m_data.find(adr) != m_data.end()) {
        return m_data[adr];
    }
    return nullptr;
}

