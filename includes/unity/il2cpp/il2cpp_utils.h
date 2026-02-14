#pragma once
#include <il2cpp/struct/il2cpp.h>

namespace Utils {
	extern void* AttachThread();

	extern IL2CPP::Il2CppImage* GetImage(const char* imageName);
	extern IL2CPP::Il2CppClass* GetClass(const char* imageName, const char* namespaze, const char* klassName);

	extern IL2CPP::Il2CppMethod* GetMethod(const char* imageName, const char* namespaze, const char* klassName, const char* methodName, int argsCount);
	extern IL2CPP::Il2CppMethod* GetMethod(IL2CPP::Il2CppClass* klass, const char* methodName, int argsCount);
}