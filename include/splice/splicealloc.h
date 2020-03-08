/*
 *  MinHook - The Minimalistic API Hooking Library for x64/x86
 *  Copyright (C) 2009-2017 Tsuda Kageyu.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 *  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _SPLICEALLOC_H
#define _SPLICEALLOC_H
// Size of each memory slot.
#define MEMORY_SLOT_SIZE 128
/*#if defined(_M_X64) || defined(__x86_64__)
    #define MEMORY_SLOT_SIZE 128
#else
    #define MEMORY_SLOT_SIZE 32
#endif*/

#ifdef __cplusplus
extern "C" {
#endif
void   InitializeBuffer(void);
void   UninitializeBuffer(void);
void * AllocateBuffer(void * pOrigin);
void   FreeBuffer(void * pBuffer);
unsigned char   IsExecutableAddress(void * pAddress);
#ifndef _WIN32
#define MEM_COMMIT           0x1000
#define MEM_RESERVE          0x2000
#define MEM_PRIVATE         0x20000
#define MEM_RELEASE          0x8000
#define MEM_FREE            0x10000
#define PAGE_NOACCESS 0x01
#define PAGE_READONLY 0x02
#define PAGE_READWRITE 0x04
#define PAGE_WRITECOPY 0x08
#define PAGE_EXECUTE 0x10
#define PAGE_EXECUTE_READ 0x20
#define PAGE_EXECUTE_READWRITE 0x40
#define PAGE_EXECUTE_WRITECOPY 0x80
#define UINT8 uint8_t
#define UINT uint32_t
#define VOID void
#define LPVOID void *
#define DWORD uint32_t
#define ULONG_PTR uintptr_t
#define LPDWORD uint32_t *
#define PBYTE uint8_t *
char VirtualProtect(void * lpAddress, size_t dwSize, uint32_t flNewProtect,uint32_t * lpflOldProtect
);
#endif
#ifdef __i386__
#define page_shift 12
#define user_space_limit    0x7fff0000  /* top of the user address space */
#define address_space_start 0x110000    /* keep DOS area clear */
#elif defined(__x86_64__)
#define page_shift 12
#define user_space_limit   0x7fffffff0000
#define address_space_start 0x10000
#endif

#ifdef __cplusplus
}
#endif
#endif
