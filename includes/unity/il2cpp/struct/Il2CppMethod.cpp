#include "Il2CppMethod.h"

namespace IL2CPP {
	uint64_t Il2CppMethod::GetOffset() { auto methodPtr = *(void**)((uint64_t)this + 0x0); return (uint64_t)methodPtr; }

	Il2CppObject* Il2CppMethod::Invoke(void* object, void** args) {
		void** exc{};

		return (Il2CppObject*)spoof_call(var::unitySpoof, Pointers::Exports.il2cpp_runtime_invoke, (void*)this, object, args, exc);
	}
}