#ifndef FUNCS_H
#define FUNCS_H
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _T
#if defined(UNICODE)
#define _T(x) (wchar_t *)L##x
#else
#define _T(x) x
#endif
#endif

wchar_t *__fastcall _conv2w(char *s, wchar_t *m);
char *__fastcall _conv2a(wchar_t *s, char *m);
uintptr_t getUTF8size(char *s);
uintptr_t getUTF16size(wchar_t *s);
#ifdef __cplusplus
}
#endif
#define c2a(x) _conv2a(x, (char *)alloca(getUTF16size(x) + sizeof(char)))
#define c2w(x)                                                    \
  _conv2w(x, (wchar_t *)alloca(sizeof(wchar_t) * getUTF8size(x) + \
                               sizeof(wchar_t)))
#define c2a(x) _conv2a(x, (char *)alloca(getUTF16size(x) + sizeof(char)))
#define c2w(x)                                                    \
  _conv2w(x, (wchar_t *)alloca(sizeof(wchar_t) * getUTF8size(x) + \
                               sizeof(wchar_t)))

#define MK_FNV32_OFFSET_BASIS 0x811c9dc5;
#define MK_FNV32_PRIME 16777619;

constexpr static unsigned hash_ct(const char *str) {
  unsigned hash = MK_FNV32_OFFSET_BASIS;
  while (*str) {
    hash = hash ^ (unsigned)(*str++);
    hash = hash * MK_FNV32_PRIME;
  }

  return hash;
}

constexpr static unsigned hash_ct(wchar_t *str) {
  unsigned hash = MK_FNV32_OFFSET_BASIS;
  while (*str) {
    hash = hash ^ (unsigned)(*str++);
    hash = hash * MK_FNV32_PRIME;
  }

  return hash;
}

extern "C" unsigned hash_rta(char *str);
extern "C" unsigned hash_rtw(wchar_t *str);
extern "C" unsigned hash_rtws(wchar_t *str, uintptr_t sz);
extern "C" unsigned hash_rtas(char *str, uintptr_t sz);
#endif /* FUNCS_H */
