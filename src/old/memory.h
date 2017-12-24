#ifdef __cplusplus
extern "C" {
#endif

#ifndef inet_pton
int __stdcall inet_pton(INT Family, LPCSTR pStringBuf, PVOID pAddr);
#endif
#ifdef __cplusplus
}
#endif

#define GetModuleSize(x)                                               \
  (((PIMAGE_NT_HEADERS)((BYTE *)x + ((PIMAGE_DOS_HEADER)x)->e_lfanew)) \
       ->OptionalHeader.SizeOfImage)
void regcall memswap(unsigned char *src, unsigned char *dst, unsigned len);

#define X86_PAGE_SIZE 0x1000
void stdcall FreeLibraryThread(void *hDllHandle);

void *regcall memcpyl(void *d, const void *s, size_t n);
unsigned char *regcall searchBytes(unsigned char *pBuff, uintptr_t pBuffSize,
                                   char *pPattStr);
unsigned char *regcall scanBytes(unsigned char *pBuff, uintptr_t pBuffSize,
                                 char *pPattStr);
bool regcall isValidIpAddressPort(char *ipAddress);

void regcall unprotectCode(unsigned char *adr, unsigned sz = X86_PAGE_SIZE);

void regcall unprotectMem(unsigned char *adr, unsigned sz = X86_PAGE_SIZE);

typedef struct _pwchar {
  unsigned short len;
  unsigned short maxlen;
  wchar_t *buf;
} pwchar;
typedef struct _pchar {
  unsigned short len;
  unsigned short maxlen;
  char *buf;
} pchar;
unsigned __stdcall RtlUnicodeStringToInteger(pwchar *, unsigned, unsigned *);
