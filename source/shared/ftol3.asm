%if __BITS__ == 32
segment .text
___isa_available   dd 0
_Int32ToUInt32     dq __float64__(0.0)
_Int32ToUInt32_2   dq __float64__(4294967296.0)
_onept5            dd __float32__(1.5)
_minus1_NaN        dd -1
global             __ftoui3, __ftoul3, __ftol3, __dtoui3, \
__dtoul3, __dtol3, __ultod3, __ltod3
__ftoui3:          cmp dword [___isa_available],0x6
jl ._ftoui3_default
vcvttss2usi eax,xmm0
ret
._ftoui3_default:
movd eax,xmm0
shl eax,0x1
jb ._ftoui3_neg
cmp eax,0x9E000000
jae ._ftoui3_notsafe
._ftoui3_cvt:
cvttss2si eax,xmm0
ret
._ftoui3_notsafe:
cmp eax,0x9F000000
jae ._ftoui3_NaN
shl eax,0x7
bts eax,0x1F
ret
._ftoui3_neg:
cmp eax,0x7F000000
jb ._ftoui3_cvt
._ftoui3_NaN:
cvttss2si ecx, dword [_minus1_NaN]
cmc
sbb eax,eax
ret
__ftoul3:
cmp dword [___isa_available],0x6
jl ._ftoul3_default
mov eax,0x1
kmovb k1,eax
vcvttps2uqq xmm0 {k1} {z}, xmm0
vmovd eax,xmm0
vpextrd edx,xmm0,0x1
ret
._ftoul3_default:
movd eax,xmm0
shl eax,0x1
jb ._ftoul3_neg
cmp eax,0x9E000000
jae ._ftoul3_notsafe
._ftoul3_cvt:
cvttss2si eax,xmm0
xor edx,edx
ret
._ftoul3_notsafe:
cmp eax,0xBF000000
jae ._ftoul3_NaN
mov ecx,eax
bts eax,0x18
shr ecx,0x18
shl eax,0x7
sub cl,0xBE
jns ._ftoul3_bigshift
xor edx,edx
shld edx,eax,cl
shl eax,cl
ret
._ftoul3_bigshift:
mov edx,eax
xor eax,eax
ret
._ftoul3_neg:
cmp eax,0x7F000000
jb ._ftoul3_cvt
._ftoul3_NaN:
cvttss2si ecx,dword [_minus1_NaN]
cmc
sbb eax,eax
cdq
ret
__ftol3:
cmp dword [___isa_available],0x6
jl ._ftol3_default
mov eax,0x1
kmovb k1,eax
vcvttps2qq xmm0 {k1} {z}, xmm0
vmovd eax,xmm0
vpextrd edx,xmm0,0x1
ret
._ftol3_default:
movd eax,xmm0
cdq
shl eax,0x1
cmp eax,0x9E000000
jae ._ftol3_notsafe
cvttss2si eax,xmm0
cdq
ret
._ftol3_notsafe:
cmp eax,0xBE000000
jae ._ftol3_mincheck
mov ecx,eax
bts eax,0x18
shr ecx,0x18
shl eax,0x7
sub cl,0xBE
xor eax,edx
sub eax,edx
shld edx,eax,cl
shl eax,cl
ret
._ftol3_mincheck:
jne ._ftol3_NaN
test edx,edx
js ._ftol3_retmin
._ftol3_NaN:
cvttss2si ecx,dword [_minus1_NaN]
._ftol3_retmin:
mov edx,0x80000000
xor eax,eax
ret
__dtoui3:
cmp dword [___isa_available],0x6
jl ._dtoui3_default
vcvttsd2usi eax,xmm0
ret
._dtoui3_default:
mov ecx,esp
add esp,0xFFFFFFF8
and esp,0xFFFFFFF8
movsd qword [esp],xmm0
mov eax,dword [esp]
mov edx,dword [esp+0x4]
mov esp,ecx
btr edx,0x1F
jb ._dtoui3_neg
cmp edx,0x41E00000
jae ._dtoui3_notsafe
cvttsd2si eax,xmm0
ret
._dtoui3_notsafe:
cmp edx,0x41F00000
jae ._dtoui3_NaN
test eax,0x1FFFFF
je ._dtoui3_exact
cvttss2si ecx,dword [_onept5]
._dtoui3_exact:
shrd eax,edx,0x15
bts eax,0x1F
ret
._dtoui3_neg:
cmp edx,0x3FF00000
jae ._dtoui3_NaN
cvttsd2si eax,xmm0
xor eax,eax
ret
._dtoui3_NaN:
cvttss2si ecx,dword [_minus1_NaN]
xor eax,eax
dec eax
ret
__dtoul3:
cmp dword [___isa_available],0x6
jl ._dtoul3_default
vmovq xmm0,xmm0
vcvttpd2uqq xmm0, xmm0
vmovd eax,xmm0
vpextrd edx,xmm0,0x1
ret
._dtoul3_default:
mov ecx,esp
add esp,0xFFFFFFF8
and esp,0xFFFFFFF8
movsd qword [esp],xmm0
mov eax,dword [esp]
mov edx,dword [esp+0x4]
mov esp,ecx
btr edx,0x1F
jb ._dtoul3_neg
cmp edx,0x41E00000
jae ._dtoul3_notsafe
cvttsd2si eax,xmm0
xor edx,edx
ret
._dtoul3_notsafe:
mov ecx,edx
bts edx,0x14
shr ecx,0x14
and edx,0x1FFFFF
sub ecx,0x433
jge ._dtoul3_large
neg ecx
push ebx
xor ebx,ebx
shrd ebx,eax,cl
je ._dtoul3_exact
cvttss2si ebx,dword [_onept5]
._dtoul3_exact:
pop ebx
shrd eax,edx,cl
shr edx,cl
ret
._dtoul3_large:
cmp ecx,0xC
jae ._dtoul3_NaN
shld edx,eax,cl
shl eax,cl
ret
._dtoul3_neg:
cmp edx,0x3FF00000
jae ._dtoul3_NaN
cvttsd2si eax,xmm0
xor eax,eax
xor edx,edx
ret
._dtoul3_NaN:
cvttss2si ecx,dword [_minus1_NaN]
xor eax,eax
dec eax
cdq
ret
__dtol3:
cmp dword [___isa_available],0x6
jl ._dtol3_default
vmovq xmm0,xmm0
vcvttpd2qq xmm0, xmm0
vmovd eax,xmm0
vpextrd edx,xmm0,0x1
ret
._dtol3_default:
mov ecx,esp
add esp,0xFFFFFFF8
and esp,0xFFFFFFF8
movsd qword [esp],xmm0
mov eax,dword [esp]
mov edx,dword [esp+0x4]
mov esp,ecx
btr edx,0x1F
sbb ecx,ecx
cmp edx,0x41E00000
jae ._dtol3_notsafe
cvttsd2si eax,xmm0
cdq
ret
._dtol3_getbits:
mov ecx,edx
bts edx,0x14
shr ecx,0x14
and edx,0x1FFFFF
sub ecx,0x433
jge ._dtol3_lshift
neg ecx
push ebx
xor ebx,ebx
shrd ebx,eax,cl
je ._dtol3_trunc
cvttss2si ebx,dword [_onept5]
._dtol3_trunc:
pop ebx
shrd eax,edx,cl
shr edx,cl
ret
._dtol3_lshift:
shld edx,eax,cl
shl eax,cl
ret
._dtol3_notsafe:
cmp edx,0x43E00000
jae ._dtol3_large
test ecx,ecx
je ._dtol3_getbits
call ._dtol3_getbits
neg eax
adc edx,0x0
neg edx
ret
._dtol3_large:
jecxz ._dtol3_NaN
ja ._dtol3_NaN
test eax,eax
je ._dtol3_retmin
._dtol3_NaN:
cvttss2si ecx,dword [_minus1_NaN]
._dtol3_retmin:
mov edx,0x80000000
xor eax,eax
ret
;times 7 nop
;lea strict ebx,[ebx]
__ultod3:
cmp dword [___isa_available],0x6
jl ._ultod3_default
vmovd xmm0,ecx
vpinsrd xmm0,xmm0,edx,0x1
vcvtuqq2pd xmm0, xmm0
ret
._ultod3_default:
xorps xmm0,xmm0
cvtsi2sd xmm0,ecx
shr ecx,0x1F
addsd xmm0,qword [ecx*8+_Int32ToUInt32]
test edx,edx
je ._ultod3_uint32
xorps xmm1,xmm1
cvtsi2sd xmm1,edx
shr edx,0x1F
addsd xmm1,qword [edx*8+_Int32ToUInt32]
mulsd xmm1,[_Int32ToUInt32_2]
addsd xmm0,xmm1
._ultod3_uint32:
ret
;times 15 nop
;lea strict esp,[esp]
;lea strict esp,[esp]
;nop
__ltod3:
cmp dword [___isa_available],0x6
jl ._ltod3_default
vmovd xmm0,ecx
vpinsrd xmm0,xmm0,edx,0x1
vcvtqq2pd xmm0, xmm0
ret
._ltod3_default:
xorps xmm1,xmm1
cvtsi2sd xmm1,edx
xorps xmm0,xmm0
cvtsi2sd xmm0,ecx
shr ecx,0x1F
mulsd xmm1,[_Int32ToUInt32_2]
addsd xmm0,qword [ecx*8+_Int32ToUInt32]
addsd xmm0,xmm1
ret
%endif
