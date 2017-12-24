#include "pch.h"
uintptr_t getUTF8size(char* s) {
  return MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
}

uintptr_t getUTF16size(wchar_t* s) {
  return WideCharToMultiByte(CP_UTF8, 0, s, -1, NULL, 0, NULL, NULL);
}

wchar_t* __fastcall _conv2w(char* s, wchar_t* m) {
  uintptr_t sz = getUTF8size(s);
  MultiByteToWideChar(CP_UTF8, 0, s, -1, m, sz);
  return m;
}

char* __fastcall _conv2a(wchar_t* s, char* m) {
  uintptr_t sz = getUTF16size(s);
  WideCharToMultiByte(CP_UTF8, 0, s, -1, m, sz, NULL, NULL);
  return m;
}

// run-time hash
unsigned hash_rta(char * str)
{
    unsigned hash = MK_FNV32_OFFSET_BASIS;
    while (*str)
    {
        hash = hash ^ (unsigned)(*str++);
        hash = hash * MK_FNV32_PRIME;
    }

    return hash;
}

unsigned hash_rtai(char * str)
{
    unsigned hash = MK_FNV32_OFFSET_BASIS;
    while (*str)
    {
        hash = hash ^ (unsigned)(tolower(*str++));
        hash = hash * MK_FNV32_PRIME;
    }

    return hash;
}

unsigned hash_rtw(wchar_t * str)
{
    unsigned hash = MK_FNV32_OFFSET_BASIS;
    while (*str)
    {
        hash = hash ^ (unsigned)(*str++);
        hash = hash * MK_FNV32_PRIME;
    }

    return hash;
}

unsigned hash_rtwi(wchar_t * str)
{
    unsigned hash = MK_FNV32_OFFSET_BASIS;
    while (*str)
    {
        hash = hash ^ (unsigned)(towlower(*str++));
        hash = hash * MK_FNV32_PRIME;
    }

    return hash;
}

unsigned hash_rtas(char * str,uintptr_t sz)
{
    unsigned hash = MK_FNV32_OFFSET_BASIS;
    unsigned i=0;
    while (sz>i)
    {
        hash = hash ^ (unsigned)(*str++);
        hash = hash * MK_FNV32_PRIME;
        i++;
    }

    return hash;
}

unsigned hash_rtasi(char * str,uintptr_t sz)
{
    unsigned hash = MK_FNV32_OFFSET_BASIS;
    unsigned i=0;
    while (sz>i)
    {
        hash = hash ^ (unsigned)(tolower(*str++));
        hash = hash * MK_FNV32_PRIME;
        i++;
    }

    return hash;
}

unsigned hash_rtws(wchar_t * str,uintptr_t sz)
{
    unsigned hash = MK_FNV32_OFFSET_BASIS;
    unsigned i=0;
    while (sz>i)
    {
        hash = hash ^ (unsigned)(*str++);
        hash = hash * MK_FNV32_PRIME;
        i++;
    }

    return hash;
}

unsigned hash_rtwsi(wchar_t * str,uintptr_t sz)
{
    unsigned hash = MK_FNV32_OFFSET_BASIS;
    unsigned i=0;
    while (sz>i)
    {
        hash = hash ^ (unsigned)(towlower(*str++));
        hash = hash * MK_FNV32_PRIME;
        i++;
    }

    return hash;
}
