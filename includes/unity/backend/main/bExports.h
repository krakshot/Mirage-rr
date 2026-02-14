#pragma once
#include <Windows.h>
#include <retspoofer/retspoofer.h>
#include <runtime.hh>

namespace Pointers {
	struct U_Exports {
		void* (*il2cpp_domain_get)();
		void* (*il2cpp_thread_attach)(void* domain);
		void (*il2cpp_thread_detach)(void* thread);

		void* (*il2cpp_domain_assembly_open)(void* domain, const char* name);
		void* (*il2cpp_class_from_name)(void* image, const char* namespaze, const char* name);
		void* (*il2cpp_class_get_method_from_name)(void* klass, const char* name, int argsCount);
		void* (*il2cpp_class_get_field_from_name)(void* klass, const char* name);

		void* (*il2cpp_field_get_parent)(void* field);
		size_t(*il2cpp_field_get_offset)(void* field);
		void (*il2cpp_field_get_value)(void* obj, void* field, void* value);
		void (*il2cpp_field_set_value)(void* obj, void* field, void* value);
		void (*il2cpp_field_static_get_value)(void* field, void* value);
		void (*il2cpp_field_static_set_value)(void* field, void* value);

		void* (*il2cpp_runtime_invoke)(void* methodInfo, void* obj, void** args, void** exc);
		void* (*il2cpp_object_get_class)(void* obj);
		void* (*il2cpp_object_new)(void* klass);
		void* (*il2cpp_object_unbox)(void* obj);
		void* (*il2cpp_value_box)(void* klass, void* data);

		void* (*il2cpp_string_new)(const char* str);

		void Setup(HMODULE module);
	};
	extern U_Exports Exports;
}
