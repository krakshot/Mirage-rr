#include "Il2CppString.h"

namespace IL2CPP {
	Il2CppString* Il2CppString::New(const char* str) {
		return (Il2CppString*)spoof_call(var::unitySpoof, Pointers::Exports.il2cpp_string_new, str);
	}
}