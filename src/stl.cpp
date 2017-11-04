#include "pch.h"
void* operator new(unsigned sz) { return malloc(sz); }

void operator delete(void* addr) { return free(addr); }
