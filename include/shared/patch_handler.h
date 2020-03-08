#ifndef _PATCH_HANDLER_H
#define _PATCH_HANDLER_H
#include <unordered_map>
#include "shared/common_macro.h"
class PatchHandler {
    struct patchDef {
        uint32_t protect;
        uint8_t *adr;
        size_t sz;
    };
    void regcall addPatch(uint8_t *adr, size_t sz, uint32_t protect);
    patchDef * regcall addCodeSwapPatch(uint8_t *adr, const uint8_t *dst,
                               size_t sz, uint32_t protect);
    patchDef * regcall getPatch(uint8_t *adr);
    void regcall freePatch(patchDef *pd);
    std::unordered_map<uint8_t *, patchDef*> m_data;

    //::unordered_map<uint8_t *, patchDef *> m_data;

   public:
    ~PatchHandler();
    void regcall restorePatches();
    int regcall getPatchCount();
    void regcall restorePatch(uint8_t *adr);
    void restoreProtection();
    void regcall addCode(uint8_t *adr, size_t sz = 100);
    void regcall codeswap(uint8_t *src, const uint8_t *dst,
                          uint32_t len);
};
#endif
