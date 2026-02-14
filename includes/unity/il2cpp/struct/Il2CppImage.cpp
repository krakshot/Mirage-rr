#include "Il2CppImage.h"

namespace IL2CPP {
	Il2CppClass* Il2CppImage::GetClass(const char* namespaze, const char* name) {
		return (Il2CppClass*)spoof_call(var::unitySpoof, Pointers::Exports.il2cpp_class_from_name, (void*)this, namespaze, name);
	}
}