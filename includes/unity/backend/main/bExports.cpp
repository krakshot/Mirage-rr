#include "bExports.h"
#include <winnt.h>
#include <vector>
#include "patterns.h"
#include <sstream>
#include "../../../../src/utils/logger/Logger.hpp"



namespace Pointers {
	U_Exports Exports;

	void U_Exports::Setup(HMODULE module) {
		uintptr_t ga_base = reinterpret_cast<uintptr_t>(module);
		void* domain_addr = reinterpret_cast<void*>(localp::scanpat(localp::strings::domain, ga_base));
		void* thread_addr = reinterpret_cast<void*>(localp::scanpat(localp::strings::thread, ga_base));

		il2cpp_domain_get = domain_addr ? (void* (*)())domain_addr : (void* (*)())reinterpret_cast<void*>(ga_base + 0xA0A600); 
		il2cpp_thread_attach = thread_addr ? (void* (*)(void*))thread_addr : (void* (*)(void*))spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_thread_attach");

		il2cpp_thread_detach = (void(*)(void*)) spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_thread_detach");

		il2cpp_domain_assembly_open = (void* (*)(void*, const char*)) spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_domain_assembly_open");
		il2cpp_class_from_name = (void* (*)(void*, const char*, const char*)) spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_class_from_name");
		il2cpp_class_get_method_from_name = (void* (*)(void*, const char*, int)) spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_class_get_method_from_name");
		il2cpp_class_get_field_from_name = (void* (*)(void*, const char*)) spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_class_get_field_from_name");

		il2cpp_field_get_parent = (void* (*)(void*)) spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_field_get_parent");
		il2cpp_field_get_offset = (size_t(*)(void*)) spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_field_get_offset");
		il2cpp_field_get_value = (void(*)(void*, void*, void*)) spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_field_get_value");
		il2cpp_field_set_value = (void(*)(void*, void*, void*)) spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_field_set_value");
		il2cpp_field_static_get_value = (void(*)(void*, void*)) spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_field_static_get_value");
		il2cpp_field_static_set_value = (void(*)(void*, void*)) spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_field_static_set_value");

		il2cpp_runtime_invoke = (void* (*)(void*, void*, void**, void**)) spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_runtime_invoke");
		il2cpp_object_get_class = (void* (*)(void*)) spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_object_get_class");
		il2cpp_object_new = (void* (*)(void*)) spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_object_new");
		il2cpp_object_unbox = (void* (*)(void*)) spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_object_unbox");
		il2cpp_value_box = (void* (*)(void*, void*)) spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_value_box");
		il2cpp_string_new = (void* (*)(const char*)) spoof_call(var::unitySpoof, GetProcAddress, module, "il2cpp_string_new");
	}
}