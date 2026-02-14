#pragma once
#include <Windows.h>

namespace memory {
	extern BOOLEAN maskCompare(PVOID buffer, LPCSTR pattern, LPCSTR mask);

	PBYTE findPattern(const char* moduleName, LPCSTR pattern, LPCSTR mask);
}