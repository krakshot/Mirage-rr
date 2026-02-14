#include "il2cpp_utils.h"

namespace Utils {
	void* AttachThread() {
		auto pDomain = IL2CPP::Il2CppDomain::Get();
		if (pDomain) {
			auto pThread = pDomain->ThreadAttach();
			return (pThread) ? pThread : nullptr;
		}
		return nullptr;
	}

	IL2CPP::Il2CppImage* GetImage(const char* name) {
		auto pDomain = IL2CPP::Il2CppDomain::Get();
		if (pDomain) {
			auto pAssembly = pDomain->GetAssembly(name);
			return (pAssembly && pAssembly->GetImage()) ? pAssembly->GetImage() : nullptr;
		}
		return nullptr;
	}

	IL2CPP::Il2CppClass* GetClass(const char* imageName, const char* namespaze, const char* klassName) {
		auto pImage = GetImage(imageName);
		if (pImage) {
			auto pKlass = pImage->GetClass(namespaze, klassName);
			return (pKlass) ? pKlass : nullptr;
		}
		return nullptr;
	}

	IL2CPP::Il2CppMethod* GetMethod(const char* imageName, const char* namespaze, const char* klassName, const char* methodName, int argsCount) {
		auto pKlass = GetClass(imageName, namespaze, klassName);
		if (pKlass) {
			auto pMethod = pKlass->GetMethod(methodName, argsCount);
			return (pMethod) ? pMethod : nullptr;
		}
		return nullptr;
	}

	IL2CPP::Il2CppMethod* GetMethod(IL2CPP::Il2CppClass* klass, const char* methodName, int argsCount) {
		if (klass) {
			auto pMethod = klass->GetMethod(methodName, argsCount);
			return (pMethod) ? pMethod : nullptr;
		}
		return nullptr;
	}
}