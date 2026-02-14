#pragma once
#include <vector>

namespace Structs::Mono {
	template <typename T>
	struct Array {
		void* klass;
		void* monitor;

		void* bounds;
		int max_length;
		T items[0];

		inline int GetLength() { return max_length; }
		inline T* GetPointer() { return (T* const)&items[0]; }

		template <typename Temp>
		inline Temp* CastedPointer() { return (Temp* const)&items[0]; }

		std::vector<T> ToVector() {
			std::vector<T> list{};
			for (int i = 0; i < max_length; i++) list.push_back(items[i]);
			return list;
		}
	};
}