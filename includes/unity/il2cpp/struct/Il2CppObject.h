#pragma once
#include <backend/main/bExports.h>

namespace IL2CPP {
	struct Il2CppClass;

	struct Il2CppObject {
		Il2CppClass* GetClass();

		void* Unbox();
	};
}