#pragma once
#include <unity/structs/il2cpp.h>

namespace Helpers {
	template <typename T>
	class Field {
	private:
		Il2CppField* m_field{};
		void* m_object{};
	public:
		Field() = default;

		Field(Il2CppField* field) : m_field(field) {}

		Field& operator[](void* instance) { m_object = instance; return *this; }

		bool IsValid() { return (m_field != nullptr); }

		T Get() {
			T value{};
			if (IsValid()) (m_object) ? m_field->GetValue(m_object, &value) : m_field->GetStaticValue(&value);
			return value;
		}
		
		void Set(T value) {
			if (IsValid()) (m_object) ? m_field->SetValue(m_object, &value) : m_field->SetStaticValue(&value);
		}
	};
}