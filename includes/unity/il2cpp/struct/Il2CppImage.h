#pragma once
#include <backend/main/bExports.h>

namespace IL2CPP {
	struct Il2CppClass;

	struct Il2CppImage {
		Il2CppClass* GetClass(const char* namespaze, const char* name);
	};
}