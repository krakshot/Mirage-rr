#pragma once
#include <cstdint>

namespace IL2CPP {
	struct Il2CppImage;

	struct Il2CppAssembly {
		Il2CppImage* GetImage();
	};
}