#pragma once
#include <unity/il2cpp/struct/il2cpp.h>

namespace Helpers {
	template <typename T>
	class Method {
	private:
		void* object{};
		Il2CppObject* result{};
		Il2CppMethod* methodInfo{};
	public:
		Method() = default;

		Method(Il2CppMethod* methodInfo) : methodInfo(methodInfo) {}

		Method(Method<T>& other) {
			object = other.object;
			result = other.result;
			methodInfo = other.methodInfo;
		}

		Method& operator[](void* instance) { object = instance; return *this; }
		
		Method& Invoke(void** args = nullptr) {
			result = methodInfo->Invoke(object, args);
			return *this;
		};

		T Get() { return (T)result; }
		T Unbox() { return *(T*)result->Unbox(); }
	};
}