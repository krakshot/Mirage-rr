#include "Il2CppObject.h"

namespace IL2CPP {
	Il2CppClass* Il2CppObject::GetClass() {
		return (Il2CppClass*)spoof_call(var::unitySpoof, Pointers::Exports.il2cpp_object_get_class, (void*)this);
	}

	void* Il2CppObject::Unbox() {
		return spoof_call(var::unitySpoof, Pointers::Exports.il2cpp_object_unbox, (void*)this);
	}
}