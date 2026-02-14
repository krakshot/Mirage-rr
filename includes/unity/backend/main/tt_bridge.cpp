#include "tt_bridge.h"
#include "patterns.h"
#include <Windows.h>

namespace tt_bridge {

	void populate_apis(uintptr_t ga_base, uintptr_t up_base, std::map<std::string, uintptr_t>& out) {
		using namespace localp;

		if (!ga_base) {
			HMODULE ga = GetModuleHandleA("GameAssembly.dll");
			ga_base = ga ? (uintptr_t)ga : 0;
		}
		if (!up_base) {
			HMODULE up = GetModuleHandleA("UnityPlayer.dll");
			up_base = up ? (uintptr_t)up : 0;
		}

		uintptr_t search_base = up_base ? up_base : ga_base;
		if (!search_base) return;

		auto dr3ammap = scanpat(strings::api, search_base);
		if (!dr3ammap && ga_base && ga_base != search_base) {
			dr3ammap = scanpat(strings::api, ga_base);
		}
		if (!dr3ammap) return;

		auto start = reinterpret_cast<uint8_t*>(dr3ammap);
		for (auto p = start; p < start + 0x2000; ++p) {
			if (!match((uintptr_t)p, strings::leardx)) continue;
			if (!match((uintptr_t)(p + 12), strings::mov)) continue;
			auto n = (char*)(p + read<int>((uintptr_t)(p + 3)) + 7);
			auto s = p + 12 + read<int>((uintptr_t)(p + 15)) + 7;
			auto ptr = read<uintptr_t>((uintptr_t)s);
			auto off = (ga_base && ptr) ? ptr - ga_base : 0;
			if (n) {
				out[std::string(n)] = off;
			}
		}
	}

}
