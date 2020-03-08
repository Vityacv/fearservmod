segment .text
%if __BITS__ == 64
	%define tword qword
	%define _M_X64 qword
	%define regbase 32
	%define _bits 8
	%define regcnt 16
	%define rest resq
	%define regsize reg_size - 2 * _bits ;2 - (hook + retadr)
	%define pushft pushfq
	%define popft popfq
	%define tax rax
	%define tcx rcx
	%define tdx rdx
	%define tbx rbx
	%define tsi rsi
	%define tdi rdi
	%define tsp rsp
	%define tbp rbp
%else
	%define trampoline _trampoline
	%define tword dword
	%define regbase 0
	%define _bits 4
	%define regcnt 8
	%define rest resd
	%define regsize reg_size - 2 * _bits
	%define pushft pushfd
	%define popft popfd
	%define tax eax
	%define tcx ecx
	%define tdx edx
	%define tbx ebx
	%define tsi esi
	%define tdi edi
	%define tsp esp
	%define tbp ebp
%endif

struc reg
	.origFunc rest 1
	.pt rest 1
	.state rest 1
	.argcnt rest 1
	%ifdef _M_X64
	._r15 rest 1
	._r14 rest 1
	._r13 rest 1
	._r12 rest 1
	._r11 rest 1
	._r10 rest 1
	._r9 rest 1
	._r8 rest 1
	%endif
	._tdi rest 1
	._tsi rest 1
	._tbp rest 1
	._tbx rest 1
	._tdx rest 1
	._tcx rest 1
	._tax rest 1
	.tflags rest 1
	.hook rest 1
	.retadr rest 1
endstruc

struc tramp
	.hookPoint rest 1
	.hookFunc rest 1
	.origFunc rest 1
	.origProtect resd 1
	.codebuf resb 30
	%ifdef _M_X64
	.jmpbuf resb 28
	%else
	.jmpbuf resb 14
	%endif
	.origLen resb 1
	.inuse resb 1
	alignb _bits
	.next rest 1
endstruc

%macro prologueSaveRegBegin 0
	pushft
	mov [tsp-_bits*2],tcx
	sub tsp,_bits*(4+regcnt-1)+regbase
	%ifdef	_M_X64
	lea tcx,[tsp+regbase]
	%else
	mov tcx,tsp
	%endif
	mov [tcx+reg._tax],tax
	mov [tcx+reg._tbx],tbx
	mov [tcx+reg._tbp],tbp
	mov [tcx+reg._tsi],tsi
	mov [tcx+reg._tdi],tdi
	%ifdef	_M_X64
	mov [tcx+reg._r8],r8
	mov [tcx+reg._r9],r9
	mov [tcx+reg._r10],r10
	mov [tcx+reg._r11],r11
	mov [tcx+reg._r12],r12
	mov [tcx+reg._r13],r13
	mov [tcx+reg._r14],r14
	mov [tcx+reg._r15],r15
	%endif
%endmacro

%macro epilogueRestoreReg 0
	add tsp,_bits*(4+regcnt-1)+regbase
	popft
	mov tax,[tcx+reg._tax]
	mov tdx,[tcx+reg._tdx]
	mov tbx,[tcx+reg._tbx]
	mov tbp,[tcx+reg._tbp]
	mov tsi,[tcx+reg._tsi]
	mov tdi,[tcx+reg._tdi]
	%ifdef	_M_X64
	mov r8,[tcx+reg._r8]
	mov r9,[tcx+reg._r9]
	mov r10,[tcx+reg._r10]
	mov r11,[tcx+reg._r11]
	mov r12,[tcx+reg._r12]
	mov r13,[tcx+reg._r13]
	mov r14,[tcx+reg._r14]
	mov r15,[tcx+reg._r15]
	%endif
%endmacro

global trampoline
trampoline:
	prologueSaveRegBegin
	xor tax,tax
	mov [tcx+reg.state],tax
	mov [tcx+reg.argcnt],tax
	mov [tcx+reg.pt],tdx
	mov tax,[tdx+tramp.origFunc]
	mov byte [tdx+tramp.inuse],1
	mov [tcx+reg.hook],tax
	mov [tcx+reg.origFunc],tax
	call [tdx+tramp.hookFunc]
	lea tcx,[tsp+regbase]
	mov tax,[tcx+reg.origFunc]
	mov [tcx+reg.hook],tax
	mov tax,[tcx+reg.pt]
	mov byte [tax+tramp.inuse],0
	mov tax,[tcx+reg.state]
	cmp tax,1
	jz retargcnt
	ja retwithoutorig
	retrestorereg:		
	epilogueRestoreReg
	mov tcx,[tcx+reg._tcx]
	ret
	retargcnt:
	mov tax,[tcx+reg.argcnt]
	lea tax,[(tax*_bits)+_bits+_bits] ;hook + return
	mov [tcx+reg.argcnt],tax
	retrestorereg2:
	epilogueRestoreReg
	mov tcx,[tsp-(regsize-reg.retadr)]
	add tsp,[tsp-(regsize-reg.argcnt)]
	jmp tcx
	retwithoutorig:
	mov tdx,[tcx+reg.pt]
	movzx tax,byte [tdx+tramp.origLen]
	add tax,[tdx+tramp.origFunc]
	mov [tcx+reg.hook],tax
	mov tax,[tcx+reg.argcnt]
	test tax,tax
	jnz retwithoutorigargcnt
	jmp retrestorereg
	retwithoutorigargcnt:
	lea tax,[(tax*_bits)+_bits] ;return
	mov [tcx+reg.argcnt],tax
	epilogueRestoreReg
	mov tcx,[tsp-(regsize-reg.hook)]
	add tsp,[tsp-(regsize-reg.argcnt)]
	jmp tcx
