//#if defined( PLATFORM_WIN32 )
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
//#include <Mmsystem.h>
//#endif

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <tchar.h>
#include <math.h>
#include "conv.h"

#define regcall __fastcall
#define stdcall __stdcall

#define exc asm volatile("int3")

#ifdef DEBUG
#error qq
#define DBGLOG(fmt, ...)                            \
  {                                                 \
    char str[1024];                                 \
    wchar_t buf[512];                               \
    sprintf(str, (const char *)fmt, ##__VA_ARGS__); \
    _conv2w(str, buf);                              \
    OutputDebugString(buf);                         \
  }

#else
#define DBGLOG(...) ((void)0)
#endif
