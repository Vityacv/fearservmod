#ifndef _SPLICE_H
#define _SPLICE_H
#include "splice/dizahex.h"
#include <unordered_map>
typedef DIZAHEX_STRUCT DisassemlyInfo;
#ifdef _M_X64
#define getVal4FromRel(x)                                             \
    (unsigned char *)x +                                              \
        (*(uint32_t *)((unsigned char *)x) > 0x7FFFFFFF               \
             ? 0xFFFFFFFF00000000 | *(uint32_t *)((unsigned char *)x) \
             : *(uint32_t *)((unsigned char *)x)) +                   \
        4
#else
#define getVal4FromRel(x) \
    (unsigned char *)x + (*(uint32_t *)((unsigned char *)x)) + 4
#endif
#define getRel4FromVal(x, y) ((unsigned char *)y - (unsigned char *)x) - 4

class SpliceHandler/* : public DynObject*/ {
public:
    struct tramp {
        uint8_t *hookPoint;    // point of hook
        uint8_t *hookFunc;     // function to execute
        uint8_t *origFunc;     // need for custom codebuf
        uint32_t origProtect;  // original protection of page
        uint8_t
            codebuf[30];  // code buffer containing original code in most cases
#if defined _M_X64
        uint8_t jmpbuf[28];  // buffer of jump to hook function
#else
        uint8_t jmpbuf[14];
#endif
        uint8_t origLen;  // length of original code
        uint8_t inuse;
        //        tramp * next;
    };
    struct reg {
        uintptr_t origFunc;
        tramp *pt;
        uintptr_t state;
        uintptr_t argcnt;
#ifdef _M_X64
        uintptr_t r15;
        uintptr_t r14;
        uintptr_t r13;
        uintptr_t r12;
        uintptr_t r11;
        uintptr_t r10;
        uintptr_t r9;
        uintptr_t r8;
#endif
        uintptr_t tdi;
        uintptr_t tsi;
        uintptr_t tbp;
        uintptr_t tbx;
        uintptr_t tdx;
        uintptr_t tcx;
        uintptr_t tax;
        uintptr_t tflags;
        uintptr_t hook;
        uintptr_t retadr;
        uintptr_t v0;
        uintptr_t v1;
        uintptr_t v2;
        uintptr_t v3;
        uintptr_t v4;
        uintptr_t v5;
        uintptr_t v6;
        uintptr_t v7;
        uintptr_t v8;
        uintptr_t v9;
        uintptr_t v10;
        uintptr_t v11;
        uintptr_t v12;
        uintptr_t v13;
        uintptr_t v14;
        uintptr_t v15;
    };
    ~SpliceHandler();
    static uint32_t regcall getOpcodeLen(void *adr, DisassemlyInfo *mdiza = nullptr);
    tramp *regcall createTramp(uint8_t *hookPoint);
    tramp *regcall getTramp(void *hookPoint);
    int regcall getTrampCount();
    void regcall freeTramp(void *hookPoint);
    uint8_t regcall checkHookPoint(uint8_t *orig, tramp *pt,
                                   uint8_t *hookPoint);
    tramp *regcall spliceUp(void *hookPoint, void *hookFunc);
    uint8_t regcall spliceDown(void * hookPoint);
    void regcall blockAllTramp();
    void regcall freeAllTramp();
    void regcall freeSplice();
   private:
    std::unordered_map<void *, tramp*> m_data;
    //ska::unordered_map<void *, tramp *> m_data;
};

int _INLINE_TEMPLATE SpliceHandler::getTrampCount(){
    return m_data.size();
}
#endif
