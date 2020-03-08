/***********************************************************************************************************************\
*
* *
*
*                 xxxxxxxx      xxxxxxxx   xxxxxxxxx     xxxxxxx     xxxx   xxxx
xxxxxxxx   xxxx    xxxx              *
*                 xxxxxxxxxx    xxxxxxxx   xxxxxxxxx    xxxxxxxxx    xxxx   xxxx
xxxxxxxx    xxxx  xxxx               *
*                 xxxx   xxxx     xxxx         xxxx    xxxx   xxxx   xxxx   xxxx
xxxx         xxxxxxxx                *
*                 xxxx   xxxx     xxxx       xxxx      xxxx   xxxx   xxxxxxxxxxx
xxxxxxx       xxxxxx                 *
*                 xxxx   xxxx     xxxx      xxxx       xxxxxxxxxxx   xxxxxxxxxxx
xxxxxxx       xxxxxx					*
*                 xxxx   xxxx     xxxx     xxxx        xxxxxxxxxxx   xxxx   xxxx
xxxx         xxxxxxxx				*
*                 xxxxxxxxxx    xxxxxxxx   xxxxxxxxx   xxxx   xxxx   xxxx   xxxx
xxxxxxxx    xxxx  xxxx               *
*                 xxxxxxxx      xxxxxxxx   xxxxxxxxx   xxxx   xxxx   xxxx   xxxx
xxxxxxxx   xxxx    xxxx              *
* * *
*
*************************************************************************************************************************
*
* *
xxx xxxx
* *
xxx xxxxx
* *
xx  xx
* *
xx  xx
* *
xxx xxxxx
* *
xxx xxxx
* *
*
*************************************************************************************************************************
*
*
*
DIZAHEX DISASSEMBLER ENGINE
* *
*
* [+] Дизассемблер инструкций x86/x86-64 (+ 16 bits): *
* general-purpose, system, fpu, mmx, sse, sse2, sse3, 3dnow!, undocumented; *
* [+] пригоден для пермутации (тогда массив dizahex_table перенести в функу
dizahex_disasm()); *
* [+] лёгкость добавления новых инструкций; *
* [+] дока по дизасму в файле man.txt; *
* [+] пример использования смотри в man.txt и в test.c; *
* [+] дизассемблирование инструкции и её сборка (dizahex_disasm(),
dizahex_asm()); *
* [+] etc; *
* *
\***********************************************************************************************************************/
// v1.0.1;

// pr0mix
// pr0mix@mail.ru
//вирмэйкинг для себя - искусство вечно

#include "pch.h"
#include "splice/dizahex.h"
#include "splice/dh_tbl.h"

int dizahex_disasm(uint8_t *pcode, DIZAHEX_STRUCT *pdiza) {
    uint32_t i, opcode;
    uint8_t mod, reg, rm, ch, disp_size, pfx_66, pfx_67, data_64;
    uint8_t flags, *pdt = dizahex_table, *pc = pcode;

    opcode = disp_size = pfx_66 = pfx_67 = data_64 =
        0;  //обнуление нужных переменных;

    for (i = sizeof(uint8_t); i < sizeof(DIZAHEX_STRUCT);
         i++)  //обнуление структы (кроме первого элемента, pdiza->mode);
        *(reinterpret_cast<char *>(pdiza) + i) = 0;

    while ((get_flags(pdt, *pc) & C_PREFIX) &&
           ((pc - pcode) < 15))  //обработка префиксов;
    {
        ch = *pc++;

        if (ch == 0x66)  // 0x66
        {
            pfx_66 = 1;
            pdiza->pfx_66 = ch;
            pdiza->flags |= F_PFX_66;
        }

        if (ch == 0x67)  // 0x67
        {
            pfx_67 = 1;
            pdiza->pfx_67 = ch;
            pdiza->flags |= F_PFX_67;
        }

        if (ch == 0xF0)  // LOCK
        {
            pdiza->pfx_lock = ch;
            pdiza->flags |= F_PFX_LOCK;
        }

        if ((ch == 0xF2) || (ch == 0xF3))  // REPE / REPNE
        {
            pdiza->pfx_rep = ch;
            pdiza->flags |= F_PFX_REP;
        }

        if ((ch == 0x26) || (ch == 0x2E) || (ch == 0x36) || (ch == 0x3E) ||
            (ch == 0x64) || (ch == 0x65))  // SEGMENTS
        {
            pdiza->pfx_seg = ch;
            pdiza->flags |= F_PFX_SEG;
        }
    }

    ch = *pc++;

    if (pdiza->mode & DISASM_MODE_64)  //если включён режим x64,
    {
        if ((ch & 0xF0) == 0x40)  //то проверим наличие префикса REX;
        {
            pdiza->flags |= F_PFX_REX;
            pdiza->pfx_rex = ch;
            ch = *pc++;

            if ((pdiza->pfx_rex & 0x08) &&
                ((ch & 0xF8) ==
                 0xB8))  //если REX есть, REX.W есть и опкод = [0xb8..0xbf]
                data_64 = 1;  //то будет imm64;
        }
    }

    pdiza->opcode = ch;  //обработка опкодов;

    if (ch == 0x0F)  // 2opcode;
    {
        pdiza->opcode_2 = ch = *pc++;
        opcode |= C_OP_EXTENDED;
    } else if ((ch >= 0xA0) && (ch <= 0xA3)) {
        pfx_66 = pfx_67;

        if (pdiza->mode & DISASM_MODE_64)  //если x64 и опкод = [0xa0..0a3];
        {
            data_64 = !pfx_66;
            pfx_66 = 0;
        }
    }

    opcode |= ch;
    flags = get_flags(pdt, opcode);  //получение характеристик опкода;

    if (flags & C_MODRM)  //обработка modrm;
    {
        pdiza->flags |= F_MODRM;
        pdiza->modrm = ch = *pc++;
        mod = ch >> 6;
        reg = (ch & 0x38) >> 3;
        rm = ch & 7;

        if (reg <= 1) {
            if (opcode == 0xF6) flags |= C_DATA_8;
            if (opcode == 0xF7) flags |= C_DATA_PFX_66_67;
        }

        if (!mod)  // mod = 0;
        {
            if (pfx_67) {
                if ((rm == 6) && !(pdiza->mode & DISASM_MODE_64))
                    disp_size = 2;  // disp = 16bits только в 16-битном режиме;
                                    // в 32 и 64
                // disp = 32bits;
            } else {
                if (rm == 5) disp_size = 4;
            }

            if ((rm == 5) && (pdiza->mode & DISASM_MODE_64))
                pdiza->flags |= F_RIP_REL;  // rip-relative адресация;
        } else if (mod == 1)                // mod = 1;
        {
            disp_size = 1;
        } else if (mod == 2)  // mod = 2;
        {
            if (pfx_67 && !(pdiza->mode & DISASM_MODE_64))
                disp_size = 2;
            else
                disp_size = 4;
        }

        if ((mod != 3) && (rm == 4) &&
            (!pfx_67 || (pdiza->mode & DISASM_MODE_64)))  //обработка SIB; SIB
        //отсутствует только в
        // 16-разрядном режиме;
        {
            pdiza->flags |= F_SIB;
            pdiza->sib = ch = *pc++;

            if (((ch & 7) == 5) && !(mod & 1)) {
                disp_size = 4;
            }
        }

        if (disp_size == 1)  //обработка смещения (DISP);
        {
            pdiza->flags |= F_DISP_8;
            pdiza->disp.disp_8 = *pc;
        } else if (disp_size == 2) {
            pdiza->flags |= F_DISP_16;
            pdiza->disp.disp_16 = *reinterpret_cast<uint16_t *>(pc);
        } else if (disp_size == 4) {
            pdiza->flags |= F_DISP_32;
            pdiza->disp.disp_32 = *reinterpret_cast<uint32_t *>(pc);
        }

        pc += disp_size;
    }

    if (flags & C_DATA_PFX_66_67)  //обработка операнда (IMM);
    {
        if (data_64) {
            pdiza->flags |= F_IMM_64;
            pdiza->imm.imm_64 = *reinterpret_cast<uint64_t *>(pc);
            pc += 8;
        } else if (pfx_66) {
            pdiza->flags |= F_IMM_16;
            pdiza->imm.imm_16 = *reinterpret_cast<uint16_t *>(pc);
            pc += 2;
        } else {
            pdiza->flags |= F_IMM_32;
            pdiza->imm.imm_32 = *reinterpret_cast<uint32_t *>(pc);
            pc += 4;
        }
    }

    if (flags & C_DATA_16) {
        if (pdiza->flags & (F_IMM_32 | F_IMM_16)) {
            pdiza->flags |= (F_DISP_16 | F_COP_IMM_DISP);
            pdiza->disp.disp_16 = *reinterpret_cast<uint16_t *>(pc);
        } else {
            pdiza->flags |= F_IMM_16;
            pdiza->imm.imm_16 = *reinterpret_cast<uint16_t *>(pc);
        }

        pc += 2;
    }

    if (flags & C_DATA_8) {
        if (pdiza->flags & (F_IMM_32 | F_IMM_16)) {
            pdiza->flags |= (F_DISP_8 | F_COP_IMM_DISP);
            pdiza->disp.disp_8 = *pc++;
        } else {
            pdiza->flags |= F_IMM_8;
            pdiza->imm.imm_8 = *pc++;
        }
    }

    if (flags & (C_REL_32 | C_REL_8)) pdiza->flags |= F_REL;

    pdiza->len =
        static_cast<uint8_t>(pc - pcode);  //вычисляем длину инструкции;

    return pdiza->len;
}
/**********************************************************************************\
* конец функи dizahex_disasm;
\**********************************************************************************/

/**********************************************************************************\
* вспомогательная внутренняя функа disp_asm;
* ассемблирование значений полей disp*;
* ВХОД:
* pcode - адрес инструкции;
* pdiza - указатель на объект структы DIZAHEX_STRUCT;
* ВЫХОД:
* (+) - адрес для дальнейшего ассемблирования;
\**********************************************************************************/
uint8_t *disp_asm(uint8_t *pcode, DIZAHEX_STRUCT *pdiza) {
    if (pdiza->flags & F_DISP_8) *pcode++ = pdiza->disp.disp_8;

    if (pdiza->flags & F_DISP_16) {
        *reinterpret_cast<uint16_t *>(pcode) = pdiza->disp.disp_16;
        pcode += 2;
    }

    if (pdiza->flags & F_DISP_32) {
        *reinterpret_cast<uint32_t *>(pcode) = pdiza->disp.disp_32;
        pcode += 4;
    }

    return pcode;
}
/**********************************************************************************\
* конец функи disp_asm;
\**********************************************************************************/

/**********************************************************************************\
* вспомогательная внутренняя функа imm_asm;
* ассемблирование значений полей imm*;
* ВХОД:
* pcode - адрес инструкции;
* pdiza - указатель на объект структы DIZAHEX_STRUCT;
* ВЫХОД:
* (+) - адрес для дальнейшего ассемблирования;
\**********************************************************************************/
uint8_t *imm_asm(uint8_t *pcode, DIZAHEX_STRUCT *pdiza) {
    if (pdiza->flags & F_IMM_8) *pcode++ = pdiza->imm.imm_8;

    if (pdiza->flags & F_IMM_16) {
        *reinterpret_cast<uint16_t *>(pcode) = pdiza->imm.imm_16;
        pcode += 2;
    }

    if (pdiza->flags & F_IMM_32) {
        *reinterpret_cast<uint32_t *>(pcode) = pdiza->imm.imm_32;
        pcode += 4;
    }

    if (pdiza->flags & F_IMM_64) {
        *reinterpret_cast<uint64_t *>(pcode) = pdiza->imm.imm_64;
        pcode += 8;
    }

    return pcode;
}
/**********************************************************************************\
* конец функи imm_asm;
\**********************************************************************************/

/**********************************************************************************\
* функа dizahex_asm;
* сборка инструкции из структуры DIZAHEX_STRUCT;
* ВХОД:
* pcode - адрес инструкции;
* pdiza - указатель на объект структы DIZAHEX_STRUCT;
* ВЫХОД:
* (+) - длина собранной инструкции;
\**********************************************************************************/
int dizahex_asm(uint8_t *pcode, DIZAHEX_STRUCT *pdiza) {
    uint8_t *pc = pcode;

    if (pdiza->flags & F_PFX_66) *pc++ = pdiza->pfx_66;
    if (pdiza->flags & F_PFX_67) *pc++ = pdiza->pfx_67;
    if (pdiza->flags & F_PFX_LOCK) *pc++ = pdiza->pfx_lock;
    if (pdiza->flags & F_PFX_REP) *pc++ = pdiza->pfx_rep;
    if (pdiza->flags & F_PFX_SEG) *pc++ = pdiza->pfx_seg;
    if (pdiza->flags & F_PFX_REX) *pc++ = pdiza->pfx_rex;

    *pc++ = pdiza->opcode;

    if (pdiza->opcode == 0x0F) *pc++ = pdiza->opcode_2;
    if (pdiza->flags & F_MODRM) *pc++ = pdiza->modrm;
    if (pdiza->flags & F_SIB) *pc++ = pdiza->sib;

    if (pdiza->flags & F_COP_IMM_DISP)  //если данный флаг установлен, тогда
                                        //меняем порядок обработки IMM и DISP:
                                        // Change Order of Processing (COP);
    {
        pc = imm_asm(pc, pdiza);
        pc = disp_asm(pc, pdiza);
    } else {
        pc = disp_asm(pc, pdiza);
        pc = imm_asm(pc, pdiza);
    }

    return (pc - pcode);
}
/**********************************************************************************\
* конец функи dizahex_asm;
\**********************************************************************************/
