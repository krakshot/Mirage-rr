#pragma once
#include <il2cpp_utils.h>

namespace Helpers {
	struct Class {
	private:
		Il2CppClass* m_class{};
	public:
		Class() = default;

		Class(Il2CppClass* klass) : m_class(klass) {}

		Class(const char* imageName, const char* namespaze, const char* name) {
			m_class = Utils::GetClass(imageName, namespaze, name);
		}

		bool IsValid() { return (m_class != nullptr); }

		Il2CppClass* Get() { return m_class; }

		Il2CppObject* CreateNewObject() { return IsValid() ? m_class->CreateNewObject() : nullptr; }
		Il2CppObject* BoxValue(void* data) { return IsValid() ? m_class->BoxValue(data) : nullptr; }

		Il2CppMethod* GetMethod(const char* methodName, int argsCount) {
			return IsValid() ? m_class->GetMethod(methodName, argsCount) : nullptr;
		}

		Il2CppField* GetField(const char* fieldName) {
			return IsValid() ? m_class->GetField(fieldName) : nullptr;
		}
	};
}