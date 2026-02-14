#include "memory.hh"
#include <Psapi.h>
#include <winscard.h>


BOOLEAN memory::maskCompare(PVOID buffer, LPCSTR pattern, LPCSTR mask) {
    for (auto b = reinterpret_cast<PBYTE>(buffer); *mask; ++pattern, ++mask, ++b) {
        if (*mask == 'x' && *reinterpret_cast<LPCBYTE>(pattern) != *b) {
            return FALSE;
        }
    }
    return TRUE;
}

PBYTE memory::findPattern(const char* moduleName, LPCSTR pattern, LPCSTR mask) {
    MODULEINFO info = { 0 };

    auto module = GetModuleHandleA(moduleName);
    if (!module)
        return NULL;

    GetModuleInformation(GetCurrentProcess(), module, &info, sizeof(info));

    info.SizeOfImage -= static_cast<DWORD>(strlen(mask));

    for (auto i = 0UL; i < info.SizeOfImage; ++i) {
        auto addr = reinterpret_cast<PBYTE>(info.lpBaseOfDll) + i;
        if (maskCompare(addr, pattern, mask)) {
            return addr;
        }
    }
    return NULL;
}