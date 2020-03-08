#ifndef _COMMON_MACRO_H
#define _COMMON_MACRO_H

#define regcall __fastcall
#define stdcall __stdcall
#define cdeclcall __cdecl
#define objcall __thiscall


#ifndef _FORCE_INLINE
#ifdef _MSC_VER /* Visual Studio */
#define _FORCE_INLINE __forceinline
#ifdef __cplusplus
extern "C" {
#endif
//#ifndef _isnan
//int _isnan(double x);
//#endif
#ifdef __cplusplus
}
#endif
#else
#if defined(__cplusplus) || \
    defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L /* C99 */
#ifdef __GNUC__
#define _FORCE_INLINE inline __attribute__((always_inline))
#else
#define _FORCE_INLINE inline
#endif
#else
#define _FORCE_INLINE
#endif /* __STDC_VERSION__ */
#endif /* _MSC_VER */
#endif /* _FORCE_INLINE */

#ifndef _OUTOFLINE_TEMPLATE
#define _OUTOFLINE_TEMPLATE
#endif

#ifndef _INLINE_TEMPLATE
#define _INLINE_TEMPLATE inline
#endif

#ifdef _COMPILER_NOEXCEPT
#define _DECL_NOEXCEPT noexcept
#define _DECL_NOEXCEPT_EXPR(x) noexcept(x)
#ifdef _DECL_NOTHROW
#undef _DECL_NOTHROW /* override with C++11 noexcept if available */
#endif
#else
#define _DECL_NOEXCEPT
#define _DECL_NOEXCEPT_EXPR(x)
#endif
#ifndef _DECL_NOTHROW
#define _DECL_NOTHROW _DECL_NOEXCEPT
#endif
#ifndef _DECL_NOTHROW
#define _DECL_NOTHROW _DECL_NOEXCEPT
#endif

#if defined _COMPILER_CONSTEXPR
#if defined(__cpp_constexpr) && __cpp_constexpr - 0 >= 201304
#define _DECL_CONSTEXPR constexpr
#define _DECL_RELAXED_CONSTEXPR constexpr
#define _CONSTEXPR constexpr
#define _RELAXED_CONSTEXPR constexpr
#else
#define _DECL_CONSTEXPR constexpr
#define _DECL_RELAXED_CONSTEXPR
#define _CONSTEXPR constexpr
#define _RELAXED_CONSTEXPR const
#endif
#else
#define _DECL_CONSTEXPR
#define _DECL_RELAXED_CONSTEXPR
#define _CONSTEXPR const
#define _RELAXED_CONSTEXPR const
#endif


template <int>
struct IntegerForSize;
template <>
struct IntegerForSize<1> {
    typedef uint8_t Unsigned;
    typedef int8_t Signed;
};
template <>
struct IntegerForSize<2> {
    typedef uint16_t Unsigned;
    typedef int16_t Signed;
};
template <>
struct IntegerForSize<4> {
    typedef uint32_t Unsigned;
    typedef int32_t Signed;
};
template <>
struct IntegerForSize<8> {
    typedef uint64_t Unsigned;
    typedef int64_t Signed;
};
template <class T>
struct IntegerForSizeof : IntegerForSize<sizeof(T)> {};
typedef IntegerForSizeof<void *>::Unsigned uintptr;
typedef IntegerForSizeof<void *>::Signed ptrdiff;
typedef ptrdiff intptr;
using sizetype = IntegerForSizeof<size_t>::Signed;

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

template <typename T>
_DECL_CONSTEXPR inline const T &regcall _Min(const T &a, const T &b) {
    return (a < b) ? a : b;
}
template <typename T>
_DECL_CONSTEXPR inline const T &regcall _Max(const T &a, const T &b) {
    return (a < b) ? b : a;
}
void regcall _assert(const char *assertion, const char *file,
                     int line) _DECL_NOTHROW;
void regcall _assert_x(const char *where, const char *what, const char *file,
                       int line) _DECL_NOTHROW;
void *regcall Malloc(size_t size);
void regcall Free(void *ptr);
void *regcall Realloc(void *ptr, size_t size);
void *regcall ReallocAligned(void *oldptr, size_t newsize, size_t oldsize,
                             size_t alignment);
void regcall FreeAligned(void *ptr);
void regcall BadAlloc();
void *regcall MallocAligned(size_t size, size_t alignment);

template <typename ForwardIterator>
_OUTOFLINE_TEMPLATE void regcall DeleteAll(ForwardIterator begin,
                                           ForwardIterator end) {
    while (begin != end) {
        delete *begin;
        ++begin;
    }
}

template <typename Container>
inline void regcall DeleteAll(const Container &c) {
    DeleteAll(c.begin(), c.end());
}
#ifdef _WIN32
void stdcall FreeLibraryThread(void *hDllHandle);
__int64 regcall FileSize(const wchar_t *name);
#define Fopen _wfopen
#define Fprintf wprintf
#else
#define Fopen fopen
#define Fprintf printf
#endif
#endif
