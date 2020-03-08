/***************************************************************************************************\
*																									*
*									DIZAHEX DISASSEMBLER ENGINE										*
*																									*
*								(flags, structure and func-prototype)								*
*																									*
\***************************************************************************************************/



																			//pr0mix
																			//вирмэйкинг для себя - искусство вечно



#ifndef DIZAHEX_H
#define DIZAHEX_H



#define get_flags(table, code)	(table[table[code / 4] + (code % 4)])
#define get_modrm_mod(code)		(code >> 6)
#define get_modrm_reg(code)		((code & 0x38) >> 3)
#define get_modrm_rm(code)		(code & 7)
#define get_sib_scale			get_modrm_mod
#define get_sib_index			get_modrm_reg
#define get_sib_base			get_modrm_rm



#define DISASM_MODE_32		0x01
#define DISASM_MODE_64		0x02



#define F_MODRM				0x00000001
#define F_SIB				0x00000002
#define F_DISP_8			0x00000010
#define F_DISP_16			0x00000020
#define F_DISP_32			0x00000040
#define F_IMM_8				0x00000100
#define F_IMM_16			0x00000200
#define F_IMM_32			0x00000400
#define F_IMM_64			0x00000800
#define F_REL				0x00001000
#define F_COP_IMM_DISP		0x00002000
#define F_RIP_REL			0x00004000
#define F_PFX_66			0x00010000
#define F_PFX_67			0x00020000
#define F_PFX_SEG			0x00040000
#define F_PFX_REP			0x00080000
#define F_PFX_LOCK			0x00100000
#define F_PFX_REX			0x00200000
#define F_PFX_ANY_32		(F_PFX_66 | F_PFX_67 | F_PFX_SEG | F_PFX_REP | F_PFX_LOCK)
#define F_PFX_ANY_64		(F_PFX_66 | F_PFX_67 | F_PFX_SEG | F_PFX_REP | F_PFX_LOCK | F_PFX_REX)
#define F_ERROR				0x01000000



#pragma pack(push, 1)

typedef struct
{
	uint8_t		mode;
	uint8_t		len;
	uint8_t		pfx_66;
	uint8_t		pfx_67;
	uint8_t		pfx_seg;
	uint8_t		pfx_rep;
	uint8_t		pfx_lock;
	uint8_t		pfx_rex;
	uint8_t		opcode;
	uint8_t		opcode_2;
	uint8_t		modrm;
	uint8_t		sib;
	union
	{
		uint8_t		disp_8;
		uint16_t	disp_16;
		uint32_t	disp_32;

	}disp;
	union
	{
		uint8_t		imm_8;
		uint16_t	imm_16;
		uint32_t	imm_32;
		uint64_t	imm_64;
	}imm;
	uint32_t	flags;
}DIZAHEX_STRUCT;

#pragma pack(pop)



#ifdef __cplusplus
extern "C" {
#endif



int dizahex_disasm(uint8_t *pcode, DIZAHEX_STRUCT *pdiza);
int dizahex_asm(uint8_t *pcode, DIZAHEX_STRUCT *pdiza);


#ifdef __cplusplus
}
#endif



#endif /* DIZAHEX_H */
