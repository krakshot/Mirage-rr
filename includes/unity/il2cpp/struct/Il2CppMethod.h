#pragma once
#include <backend/main/bExports.h>
#include <cstdint>

namespace IL2CPP {
	struct Il2CppObject;

	struct Il2CppMethod {
		uint64_t GetOffset();

		Il2CppObject* Invoke(void* object, void** args);
	};
}