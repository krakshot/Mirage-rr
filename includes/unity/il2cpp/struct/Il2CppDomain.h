#pragma once
#include <backend/main/bExports.h>

namespace IL2CPP {
	struct Il2CppThread;
	struct Il2CppAssembly;

	struct Il2CppDomain {
		static Il2CppDomain* Get();
		Il2CppThread* ThreadAttach();

		Il2CppAssembly* GetAssembly(const char* name);
	};
}