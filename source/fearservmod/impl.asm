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
mov eax,[_g_doConnectIpAdrTramp]
jmp eax

