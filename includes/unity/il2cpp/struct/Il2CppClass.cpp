#include "Il2CppClass.h"

namespace IL2CPP {
	Il2CppMethod* Il2CppClass::GetMethod(const char* name, int argsCount) {
		return (Il2CppMethod*)spoof_call(var::unitySpoof, Pointers::Exports.il2cpp_class_get_method_from_name, (void*)this, name, argsCount);
	}

	Il2CppField* Il2CppClass::GetField(const char* name) {
		return (Il2CppField*)spoof_call(var::unitySpoof, Pointers::Exports.il2cpp_class_get_field_from_name, (void*)this, name);
	}

	Il2CppObject* Il2CppClass::CreateNewObject() {
		return (Il2CppObject*)spoof_call(var::unitySpoof, Pointers::Exports.il2cpp_object_new, (void*)this);
	}

	Il2CppObject* Il2CppClass::BoxValue(void* data) {
		return (Il2CppObject*)spoof_call(var::unitySpoof, Pointers::Exports.il2cpp_value_box, (void*)this, data);
	}
}