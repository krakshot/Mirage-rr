#pragma once
#include <backend/main/bExports.h>

namespace IL2CPP {
	struct Il2CppMethod;
	struct Il2CppField;
	struct Il2CppObject;

	struct Il2CppClass {
		Il2CppMethod* GetMethod(const char* name, int argsCount);
		Il2CppField* GetField(const char* name);

		Il2CppObject* CreateNewObject();
		Il2CppObject* BoxValue(void* data);
	};
}