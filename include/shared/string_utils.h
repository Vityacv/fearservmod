#ifndef _STRING_UTILS_H
#define _STRING_UTILS_H
#include "shared/common_macro.h"

#ifdef __cplusplus

#define _OBFUSCATE_STRINGS
#ifdef _OBFUSCATE_STRINGS
namespace str_obfuscate {
template <uintptr_t X>
struct EnsureCompileTime {
    enum : uintptr_t { Value = X };
};

// Use Compile-Time as seed
#define _CurrentSeed                                                     \
    ((__TIME__[7] - '0') * 1 + (__TIME__[6] - '0') * 10 +        \
     (__TIME__[4] - '0') * 60 + (__TIME__[3] - '0') * 600 +      \
     (__TIME__[1] - '0') * 3600 + (__TIME__[0] - '0') * 36000) + \
        (__LINE__ * 100000)

constexpr unsigned LinearCongruentGenerator(unsigned Rounds) {
    return 1013904223 + 1664525 * ((Rounds > 0)
                                       ? LinearCongruentGenerator(Rounds - 1)
                                       : _CurrentSeed & 0xFFFFFFFF);
}
#define Random() EnsureCompileTime<LinearCongruentGenerator(10)>::Value
#define RandomNumber(Min, Max) (Min + (Random() % (Max - Min + 1)))

template <unsigned... Pack>
struct IndexList {};

template <typename IndexList, unsigned Right>
struct Append;
template <unsigned... Left, unsigned Right>
struct Append<IndexList<Left...>, Right> {
    typedef IndexList<Left..., Right> Result;
};

template <unsigned N>
struct ConstructIndexList {
    typedef typename Append<typename ConstructIndexList<N - 1>::Result,
                            N - 1>::Result Result;
};
template <>
struct ConstructIndexList<0> {
    typedef IndexList<> Result;
};

inline const unsigned char XORKEY = static_cast<char>(RandomNumber(0, 0xFF));
_FORCE_INLINE constexpr char obfuscateChar(const char Character,
                                           unsigned Index) {
    return Character + (XORKEY + Index);
}
_FORCE_INLINE constexpr char returnChar(const char Character, unsigned Index) {
    return Character;
}
template <typename IndexList>
class obfuscate;
template <unsigned... Index>
class obfuscate<IndexList<Index...>> {
   private:
    char Value[sizeof...(Index)];
    char Value2[sizeof...(Index)];
    wchar_t end;

   public:
    _FORCE_INLINE constexpr obfuscate(const char *const String)
        : Value{obfuscateChar(String[Index], Index)...} {}

    _FORCE_INLINE char *deobfuscate() {
        deobfuscateFunc(Value, sizeof...(Index), XORKEY);
        return Value;
    }
    _FORCE_INLINE wchar_t *deobfuscateW() {
        deobfuscateWFunc(Value, sizeof...(Index), XORKEY);
        return (wchar_t *)Value;
    }
    static void regcall deobfuscateFunc(char *Value, uintptr_t i,
                                        unsigned char key) {
        Value[i] = 0;
        do {
            i--;
            Value[i] =
                (unsigned char)(*(unsigned char *)(Value + i) - (key + i));
        } while (i);
    }
    static void regcall deobfuscateWFunc(char *Value, uintptr_t i,
                                         unsigned char key) {
        *(wchar_t *)((wchar_t *)Value + i) = 0;
        do {
            i--;
            *(wchar_t *)((wchar_t *)Value + i) =
                (unsigned char)(*(unsigned char *)(Value + i) - (key + i));
        } while (i);
    }
    char *get() { return Value; }
};
}  // namespace str_obfuscate
#define _C(x)                                                         \
    (str_obfuscate::obfuscate<                                        \
         str_obfuscate::ConstructIndexList<sizeof(x) - 1>::Result>(x) \
         .deobfuscate())
#define _T(x)                                                         \
    (str_obfuscate::obfuscate<                                        \
         str_obfuscate::ConstructIndexList<sizeof(x) - 1>::Result>(reinterpret_cast<const char*>(x)) \
         .deobfuscateW())
#endif
#endif

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

#ifdef __cplusplus
}
#endif

//// compile-time hash helper function
// constexpr uint32_t hash_one(char c, const char* remain, unsigned long long
// value)
//{
//    return c == 0 ? value : hash_one(remain[0], remain + 1, (value * 31)+c);
//}

//// compile-time hash
// constexpr uint32_t hash_ct(const char* str)
//{
//    return hash_one(str[0], str + 1, 0);
//}

class StringUtil {
    // compile-time hash helper function
    template <typename T>
    _INLINE_TEMPLATE constexpr static uint32_t hash_one(char c, const T *remain,
                                                        uint32_t value);
    static constexpr size_t convertBufferSize = 65535*2; // should be enough for most cases
#ifdef _WIN32
    //std::unique_ptr<wchar_t> m_buffer;
    wchar_t m_buffer[convertBufferSize/sizeof(wchar_t)];
#endif
   public:
    // compile-time hash
    template <typename T = char>
    _INLINE_TEMPLATE constexpr static uint32_t hash_ct(const T *str);

    // run-time hash
    template <typename T = char>
    static uint32_t regcall hash_rt(T *str);
    static uint32_t regcall hash_rta(char *str);  // hash string
    static uint32_t regcall hash_rtai(char *str); // hash string case-insensitive
    static uint32_t regcall hash_rtas(char *str, uintptr_t sz); // hash chars
    static uint32_t regcall hash_rtasi(char *str,
                                uintptr_t sz); // hash chars case-insensitive
    static uint32_t regcall hash_rtw(wchar_t *str);   // hash wide string
    static uint32_t regcall hash_rtwi(wchar_t *str);  // hash wide string case-insensitive
    static uint32_t regcall hash_rtws(wchar_t *str, uintptr_t sz); // hash wide chars
    static uint32_t regcall hash_rtwsi(wchar_t *str,
                                uintptr_t sz); // hash wide chars case-insensitive
#ifdef _WIN32
    static StringUtil& instance();
    // converting from UTF8 to UTF16
    static int regcall toNative(const char *s, wchar_t *m, size_t sz);
    const wchar_t* regcall toNative(const char* s);
    // converting from UTF16 to UTF8
    static int regcall fromNative(const wchar_t *s, char *m, size_t sz);
    const char* regcall fromNative(const wchar_t* s);
    static char * regcall copyFromNative(const wchar_t *wb, char *m, size_t sz);
    static wchar_t * regcall copyToNative(const char *mb, wchar_t *wb, size_t sz);
    static size_t regcall getUtf8Size(char* s);
    static size_t regcall getUtf16Size(wchar_t* s);

#else
    // we just use UTF8 on linux
    static constexpr StringUtil& instance() {};
    const constexpr char* regcall toNative(const char* s) { return s; };
    const constexpr char* regcall fromNative(const char* s) {return s; };
#endif
};

template <typename T>
_INLINE_TEMPLATE constexpr uint32_t StringUtil::hash_one(char c, const T *remain,
                                                     uint32_t value) {
    return c == 0 ? value
                  : hash_one<T>(remain[0], remain + 1, (value * 31) + c);
}

template <typename T>
_INLINE_TEMPLATE constexpr uint32_t StringUtil::hash_ct(const T *str) {
    return hash_one<T>(str[0], str + 1, 0);
}

template<typename T> _OUTOFLINE_TEMPLATE uint32_t regcall StringUtil::hash_rt(T * str){
    uint32_t hash = 0;
    while (*str)
    {
        hash = hash * 31;
        hash += *reinterpret_cast<T*>(str++);
    }
    return hash;
}
#endif
