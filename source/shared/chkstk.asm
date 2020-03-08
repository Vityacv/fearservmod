segment .text
%define _PAGESIZE_ 0x1000
%if __BITS__ == 64
global __chkstk
__chkstk:
sub     rsp, 0x10
mov     [rsp], r10
mov     [rsp+0x8], r11
xor     r11, r11
lea     r10, [rsp+0x18]
sub     r10, rax
cmovb   r10, r11
mov     r11, [gs:0x10]
cmp     r10, r11
;bnd
jae     short .chkstk_exit_loop
and     r10w, 0xF000
.chkstk_loop:
lea     r11, [r11-_PAGESIZE_]
mov     byte [r11],0x0
cmp     r10, r11
;bnd
jne     short .chkstk_loop
.chkstk_exit_loop:
mov     r10, [rsp]
mov     r11, [rsp+0x8]
add     rsp, 0x10
;bnd
ret
%else
global __alloca_probe_8
global __alloca_probe_16
global __alloca_probe
global __chkstk
__alloca_probe_8:               ; 16 byte aligned alloca
push    ecx
lea     ecx, [esp+0x8]            ; TOS before entering this function
sub     ecx, eax                ; New TOS
and     ecx, (0x8 - 0x1)            ; Distance from 8 bit align (align down)
add     eax, ecx                ; Increase allocation Size
sbb     ecx, ecx                ; ecx = 0xFFFFFFFF if size wrapped around
or      eax, ecx                ; cap allocation size on wraparound
pop     ecx                     ; Restore ecx
jmp     __chkstk
__alloca_probe_16:              ; 16 byte aligned alloca
push    ecx
lea     ecx, [esp + 0x8]          ; TOS before entering this function
sub     ecx, eax                ; New TOS
and     ecx, (0x8 - 0x1)            ; Distance from 8 bit align (align down)
add     eax, ecx                ; Increase allocation Size
sbb     ecx, ecx                ; ecx = 0xFFFFFFFF if size wrapped around
or      eax, ecx                ; cap allocation size on wraparound
pop     ecx                     ; Restore ecx
;jmp     __chkstk
__chkstk:
__alloca_probe:
push    ecx

; Calculate new TOS.

lea     ecx, [esp + 0x8 - 0x4]      ; TOS before entering function + size for ret value
sub     ecx, eax                ; new TOS

; Handle allocation size that results in wraparound.
; Wraparound will result in StackOverflow exception.

sbb     eax, eax                ; 0 if CF==0, ~0 if CF==1
not     eax                     ; ~0 if TOS did not wrapped around, 0 otherwise
and     ecx, eax                ; set to 0 if wraparound

mov     eax, esp                ; current TOS
and     eax, -(_PAGESIZE_) ; Round down to current page boundary

.cs10:
cmp     ecx, eax                ; Is new TOS
;bnd
jb      short .cs20              ; in probed page?
mov     eax, ecx                ; yes.
pop     ecx
xchg    esp, eax                ; update esp
mov     eax, dword [eax]    ; get return address
mov     dword [esp], eax    ; and put it at new TOS
;bnd
ret

; Find next lower page and probe
.cs20:
sub     eax, _PAGESIZE_         ; decrease by PAGESIZE
test    dword [eax],eax     ; probe page.
jmp     short .cs10
%endif
