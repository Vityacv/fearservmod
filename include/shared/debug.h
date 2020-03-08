#ifndef _DEBUG_H
#define _DEBUG_H
//#include "shared/string.h"
#if defined(NDEBUG) && !defined(_FORCE_DBGLOG)
#define DBGLOG(...) ((void)0);
#else
#ifdef WIN32
#define DBGLOG(fmt, ...)                            \
    {                                               \
        char __buf[1024];                           \
        _snprintf(__buf, 1000, fmt, ##__VA_ARGS__); \
        OutputDebugStringA(__buf);                  \
    }
#define DBGLOGW(fmt, ...)                            \
    {                                                \
        wchar_t __buf[1024];                         \
        _snwprintf(__buf, 1000, fmt, ##__VA_ARGS__); \
        OutputDebugStringW(__buf);                   \
    }

#else
#define DBGLOG(...) ((void)0);
#define DBGLOGW(...) ((void)0);
#endif
#endif
#endif
