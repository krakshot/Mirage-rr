#include "Il2CppField.h"

namespace IL2CPP {
	Il2CppClass* Il2CppField::GetParent() {
		return (Il2CppClass*)spoof_call(var::unitySpoof, Pointers::Exports.il2cpp_field_get_parent, (void*)this);
	}

	size_t Il2CppField::GetOffset() {
		return spoof_call(var::unitySpoof, Pointers::Exports.il2cpp_field_get_offset, (void*)this);
	}

	void Il2CppField::GetValue(void* object, void* value) {
		return spoof_call(var::unitySpoof, Pointers::Exports.il2cpp_field_get_value, object, (void*)this, value);
	}

	void Il2CppField::SetValue(void* object, void* value) {
		return spoof_call(var::unitySpoof, Pointers::Exports.il2cpp_field_set_value, object, (void*)this, value);
	}

	void Il2CppField::GetStaticValue(void* value) {
		return spoof_call(var::unitySpoof, Pointers::Exports.il2cpp_field_static_get_value, (void*)this, value);
	}

	void Il2CppField::SetStaticValue(void* value) {
		return spoof_call(var::unitySpoof, Pointers::Exports.il2cpp_field_static_set_value, (void*)this, value);
	}
}