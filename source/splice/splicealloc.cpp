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
#include "pch.h"
#include "splice/splicealloc.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
// Size of each memory block. (= page size of VirtualAlloc)
#define MEMORY_BLOCK_SIZE 0x1000

// Max range for seeking a memory block. (= 1024MB)
#define MAX_MEMORY_RANGE 0x40000000

// Memory protection flags to check the executable address.

#ifndef _WIN32
#include "os-linux.h"

inline int linProtect(uint32_t flProtect){
    int perm =0;
    if(flProtect & PAGE_READWRITE || flProtect & PAGE_WRITECOPY)
        perm|=PROT_READ | PROT_WRITE;
    if(flProtect & PAGE_READONLY)
        perm|=PROT_READ;
    if(flProtect & PAGE_EXECUTE)
        perm|=PROT_EXEC;
    if(flProtect & PAGE_EXECUTE_READ)
        perm|=PROT_READ|PROT_EXEC;
    if(flProtect & PAGE_EXECUTE_READWRITE || flProtect & PAGE_EXECUTE_WRITECOPY)
        perm|=PROT_READ|PROT_EXEC|PROT_WRITE;
    return perm;
}

inline uint32_t winProtect(int perm){
    if(perm & PROT_READ && perm & PROT_WRITE && perm & PROT_EXEC)
        return PAGE_EXECUTE_READWRITE;
    if(perm & PROT_READ && perm & PROT_WRITE)
        return PAGE_READWRITE;
    if(perm & PROT_READ && perm & PROT_EXEC)
        return PAGE_EXECUTE_READ;
    if(perm & PROT_READ)
        return PAGE_READONLY;
    if(perm & PROT_WRITE)
        return PAGE_READWRITE;
    if(perm & PROT_EXEC)
        return PAGE_EXECUTE;
    if(perm==PROT_NONE)
        return PAGE_NOACCESS;
    return PAGE_EXECUTE_READWRITE;
}

inline uintptr_t get_mask( unsigned long zero_bits )
{
    if (!zero_bits) return 0xffff;  /* allocations are aligned to 64K by default */
    if (zero_bits < page_shift) zero_bits = page_shift;
    if (zero_bits > 21) return 0;
    return (1 << zero_bits) - 1;
}

typedef struct _MEMORY_BASIC_INFORMATION { 
  void * BaseAddress; 
  void * AllocationBase; 
  uint32_t AllocationProtect; 
  uint32_t RegionSize; 
  uint32_t State; 
  uint32_t Protect; 
  uint32_t Type; 
  } MEMORY_BASIC_INFORMATION; 
typedef MEMORY_BASIC_INFORMATION *PMEMORY_BASIC_INFORMATION; 

size_t VirtualQuery(void * lpAddress,MEMORY_BASIC_INFORMATION * lpBuffer,size_t dwLength){
  map_iterator mi;
  unsigned long hi, low, perm;
  if (maps_init (&mi, getpid()) < 0)
    return 0;

  unsigned long offset;
  while (maps_next (&mi, &low, &hi, &offset, &perm)) {
    if((unsigned long)lpAddress >=low && (unsigned long)lpAddress <= hi ){
      perm=winProtect(perm);
      lpBuffer->BaseAddress=(void*)low;
      lpBuffer->AllocationBase=(void*)low;
      lpBuffer->AllocationProtect=perm;
      lpBuffer->RegionSize=hi-low;
      lpBuffer->State=MEM_COMMIT;
      lpBuffer->Protect=perm;
      lpBuffer->Type=MEM_PRIVATE;
      maps_close (&mi);
      return sizeof(MEMORY_BASIC_INFORMATION);
    }
  }
    lpBuffer->BaseAddress=(void*)0;
      lpBuffer->AllocationBase=(void*)0;
      lpBuffer->AllocationProtect=PAGE_NOACCESS;
      lpBuffer->RegionSize=0;
      lpBuffer->State=MEM_FREE;
      lpBuffer->Protect=perm;
      lpBuffer->Type=MEM_PRIVATE;
  maps_close (&mi);
  return sizeof(MEMORY_BASIC_INFORMATION);
}

char VirtualFree(
  void* lpAddress,size_t dwSize,unsigned
){
    if(munmap (lpAddress, dwSize))
        return 0;
    return 1;
}

void* VirtualAlloc(
 void*lpAddress,size_t dwSize,uint32_t  flAllocationType,uint32_t  flProtect
){
    int perm = linProtect(flProtect);
    intptr_t retn = (intptr_t)mmap (lpAddress, dwSize, perm,
         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    *(char*)retn=1;
    if(retn == -1)
        return 0;
    return (void*)retn;
}

char VirtualProtect(void * lpAddress, size_t dwSize, uint32_t flNewProtect,uint32_t * lpflOldProtect
){
    MEMORY_BASIC_INFORMATION mbi;
    if(lpflOldProtect){
        if(VirtualQuery(lpAddress,(MEMORY_BASIC_INFORMATION*)&mbi,sizeof(MEMORY_BASIC_INFORMATION))){
            *lpflOldProtect=winProtect(mbi.Protect);
        }else return 1;
    }
    if(mprotect((void*)((uintptr_t)lpAddress&-0x1000),(dwSize|0xFFF)+1,linProtect(flNewProtect))==-1)
        return 0;
    return 1;
}


#define PAGE_EXECUTE_FLAGS PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE
#else
#define PAGE_EXECUTE_FLAGS \
    (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)
#endif


// Memory slot.
typedef struct _MEMORY_SLOT
{
    union
    {
        struct _MEMORY_SLOT *pNext;
        UINT8 buffer[MEMORY_SLOT_SIZE];
    };
} MEMORY_SLOT, *PMEMORY_SLOT;

// Memory block info. Placed at the head of each block.
typedef struct _MEMORY_BLOCK
{
    struct _MEMORY_BLOCK *pNext;
    PMEMORY_SLOT pFree;         // First element of the free slot list.
    UINT usedCount;
} MEMORY_BLOCK, *PMEMORY_BLOCK;

//-------------------------------------------------------------------------
// Global Variables:
//-------------------------------------------------------------------------

// First element of the memory block list.
PMEMORY_BLOCK g_pMemoryBlocks;

//-------------------------------------------------------------------------
VOID InitializeBuffer(VOID)
{
    // Nothing to do for now.
}

//-------------------------------------------------------------------------
VOID UninitializeBuffer(VOID)
{
    PMEMORY_BLOCK pBlock = g_pMemoryBlocks;
    g_pMemoryBlocks = NULL;

    while (pBlock)
    {
        PMEMORY_BLOCK pNext = pBlock->pNext;
        VirtualFree(pBlock, 0, MEM_RELEASE);
        pBlock = pNext;
    }
}

//-------------------------------------------------------------------------
#if defined(_M_X64) || defined(__x86_64__)
static LPVOID FindPrevFreeRegion(LPVOID pAddress, LPVOID pMinAddr, DWORD dwAllocationGranularity)
{
    ULONG_PTR tryAddr = (ULONG_PTR)pAddress;

    // Round down to the allocation granularity.
    tryAddr -= tryAddr % dwAllocationGranularity;

    // Start from the previous allocation granularity multiply.
    tryAddr -= dwAllocationGranularity;

    while (tryAddr >= (ULONG_PTR)pMinAddr)
    {
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery((LPVOID)tryAddr, &mbi, sizeof(mbi)) == 0)
            break;

        if (mbi.State == MEM_FREE)
            return (LPVOID)tryAddr;

        if ((ULONG_PTR)mbi.AllocationBase < dwAllocationGranularity)
            break;

        tryAddr = (ULONG_PTR)mbi.AllocationBase - dwAllocationGranularity;
    }

    return NULL;
}
#endif

//-------------------------------------------------------------------------
#if defined(_M_X64) || defined(__x86_64__)
static LPVOID FindNextFreeRegion(LPVOID pAddress, LPVOID pMaxAddr, DWORD dwAllocationGranularity)
{
    ULONG_PTR tryAddr = (ULONG_PTR)pAddress;

    // Round down to the allocation granularity.
    tryAddr -= tryAddr % dwAllocationGranularity;

    // Start from the next allocation granularity multiply.
    tryAddr += dwAllocationGranularity;

    while (tryAddr <= (ULONG_PTR)pMaxAddr)
    {
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery((LPVOID)tryAddr, &mbi, sizeof(mbi)) == 0)
            break;

        if (mbi.State == MEM_FREE)
            return (LPVOID)tryAddr;

        tryAddr = (ULONG_PTR)mbi.BaseAddress + mbi.RegionSize;
        // Round up to the next allocation granularity.
        tryAddr += dwAllocationGranularity - 1;
        tryAddr -= tryAddr % dwAllocationGranularity;
    }

    return NULL;
}
#endif

//-------------------------------------------------------------------------
static PMEMORY_BLOCK GetMemoryBlock(LPVOID pOrigin)
{
    PMEMORY_BLOCK pBlock;
#if defined(_M_X64) || defined(__x86_64__)
    ULONG_PTR minAddr;
    ULONG_PTR maxAddr;
#ifdef _WIN32
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    DWORD dwAllocationGranularity = si.dwAllocationGranularity;
    minAddr = (ULONG_PTR)si.lpMinimumApplicationAddress;
    maxAddr = (ULONG_PTR)si.lpMaximumApplicationAddress;
#else
    DWORD dwAllocationGranularity = get_mask(0) + 1;
    minAddr = (ULONG_PTR)(void *)address_space_start;
    maxAddr = (ULONG_PTR)(char *)user_space_limit - 1;
#endif
    // pOrigin ± 512MB
    if ((ULONG_PTR)pOrigin > MAX_MEMORY_RANGE && minAddr < (ULONG_PTR)pOrigin - MAX_MEMORY_RANGE)
        minAddr = (ULONG_PTR)pOrigin - MAX_MEMORY_RANGE;

    if (maxAddr > (ULONG_PTR)pOrigin + MAX_MEMORY_RANGE)
        maxAddr = (ULONG_PTR)pOrigin + MAX_MEMORY_RANGE;

    // Make room for MEMORY_BLOCK_SIZE bytes.
    maxAddr -= MEMORY_BLOCK_SIZE - 1;
#endif

    // Look the registered blocks for a reachable one.
    for (pBlock = g_pMemoryBlocks; pBlock != NULL; pBlock = pBlock->pNext)
    {
#if defined(_M_X64) || defined(__x86_64__)
        // Ignore the blocks too far.
        if ((ULONG_PTR)pBlock < minAddr || (ULONG_PTR)pBlock >= maxAddr)
            continue;
#endif
        // The block has at least one unused slot.
        if (pBlock->pFree != NULL)
            return pBlock;
    }

#if defined(_M_X64) || defined(__x86_64__)
    // Alloc a new block above if not found.
    {
        LPVOID pAlloc = pOrigin;
        while ((ULONG_PTR)pAlloc >= minAddr)
        {
            pAlloc = FindPrevFreeRegion(pAlloc, (LPVOID)minAddr, dwAllocationGranularity);
            if (pAlloc == NULL)
                break;

            pBlock = (PMEMORY_BLOCK)VirtualAlloc(
                pAlloc, MEMORY_BLOCK_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (pBlock != NULL)
                break;
        }
    }
    // Alloc a new block below if not found.
    if (pBlock == NULL)
    {
        LPVOID pAlloc = pOrigin;
        while ((ULONG_PTR)pAlloc <= maxAddr)
        {
            pAlloc = FindNextFreeRegion(pAlloc, (LPVOID)maxAddr, dwAllocationGranularity);
            if (pAlloc == NULL)
                break;

            pBlock = (PMEMORY_BLOCK)VirtualAlloc(
                pAlloc, MEMORY_BLOCK_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (pBlock != NULL)
                break;
        }
    }
#else
    // In x86 mode, a memory block can be placed anywhere.
    pBlock = (PMEMORY_BLOCK)VirtualAlloc(
        NULL, MEMORY_BLOCK_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#endif
    if (pBlock != NULL)
    {
        // Build a linked list of all the slots.
        PMEMORY_SLOT pSlot = (PMEMORY_SLOT)pBlock + 1;
        pBlock->pFree = NULL;
        pBlock->usedCount = 0;
        do
        {
            pSlot->pNext = pBlock->pFree;
            pBlock->pFree = pSlot;
            pSlot++;
        } while ((ULONG_PTR)pSlot - (ULONG_PTR)pBlock <= MEMORY_BLOCK_SIZE - MEMORY_SLOT_SIZE);

        pBlock->pNext = g_pMemoryBlocks;
        g_pMemoryBlocks = pBlock;
    }
    return pBlock;
}

//-------------------------------------------------------------------------
LPVOID AllocateBuffer(LPVOID pOrigin)
{
    PMEMORY_SLOT  pSlot;
    PMEMORY_BLOCK pBlock = GetMemoryBlock(pOrigin);
    if (pBlock == NULL)
        return NULL;

    // Remove an unused slot from the list.
    pSlot = pBlock->pFree;
    pBlock->pFree = pSlot->pNext;
    pBlock->usedCount++;
#ifdef _DEBUG
    // Fill the slot with INT3 for debugging.
    memset(pSlot, 0xCC, sizeof(MEMORY_SLOT));
#endif
    return pSlot;
}

//-------------------------------------------------------------------------
VOID FreeBuffer(LPVOID pBuffer)
{
    PMEMORY_BLOCK pBlock = g_pMemoryBlocks;
    PMEMORY_BLOCK pPrev = NULL;
    ULONG_PTR pTargetBlock = ((ULONG_PTR)pBuffer / MEMORY_BLOCK_SIZE) * MEMORY_BLOCK_SIZE;

    while (pBlock != NULL)
    {
        if ((ULONG_PTR)pBlock == pTargetBlock)
        {
            PMEMORY_SLOT pSlot = (PMEMORY_SLOT)pBuffer;
#ifdef _DEBUG
            // Clear the released slot for debugging.
            memset(pSlot, 0x00, sizeof(*pSlot));
#endif
            // Restore the released slot to the list.
            pSlot->pNext = pBlock->pFree;
            pBlock->pFree = pSlot;
            pBlock->usedCount--;

            // Free if unused.
            if (pBlock->usedCount == 0)
            {
                if (pPrev)
                    pPrev->pNext = pBlock->pNext;
                else
                    g_pMemoryBlocks = pBlock->pNext;

                VirtualFree(pBlock, 0, MEM_RELEASE);
            }

            break;
        }

        pPrev = pBlock;
        pBlock = pBlock->pNext;
    }
}

//-------------------------------------------------------------------------
unsigned char IsExecutableAddress(LPVOID pAddress)
{
    MEMORY_BASIC_INFORMATION mi;
    VirtualQuery(pAddress, &mi, sizeof(mi));

    return (mi.State == MEM_COMMIT && (mi.Protect & PAGE_EXECUTE_FLAGS));
}
