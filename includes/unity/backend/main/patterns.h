#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <string>
#include <map>

namespace localp {
	namespace strings {
		constexpr const char* domain = "48 83 EC 28 48 63 05 ? ? ? ? 48 8D 0D ? ? ? ? 4C 8B 05 ? ? ? ? 48 8D 15 ? ? ? ? 8B 0C 08 48 8B 44 24 28 48 C1 E1 06 48 03 CA 48 3B C1 73 0A 48 8B 44 24 28 48 3B C2 73 29 49 63 40 3C 42 8B 4C ? 50";
		constexpr const char* thread = "40 56 48 83 EC 20 48 8B F1 48 8B 0D ? ? ? ? 8B 09 FF 15 ? ? ? ? 48 85 C0 75 5F 48 89 5C 24 ? 48 89 7C 24 ? FF 15 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B D8 E8 ? ? ? ? 48 8B D3 48 8B C8 48 8B F8 E8 ? ? ? ? 48 8B 4F 10 48 8B D6 C7 41 ? ? ? ? ? 48 8B CF E8 ? ? ? ? 48 8B 5C 24 ? 48 8B C7 48 8B 7C 24 ? 48 83 C4 20";
		constexpr const char* methodptr = "48 83 EC ?? 44 8B CA 4C 8B D1";
		constexpr const char* methodinfo1 = "48 89 5C 24 ? 57 48 83 EC ? 48 8B 82 ? ? ? ? 48 8B F9 8B 40 ? 41 03 C0 4C 8B 05";
		constexpr const char* methodinfo2 = "48 89 5C 24 ? 57 48 83 EC ? 48 8B 42 ? 48 8B F9 8B 40 ? 41 03 C0 4C 8B 05";
		constexpr const char* api = "48 83 EC 28 E8 ? ? ? ? 48 89 05 ? ? ? ? 48 85 C0 ? ? 48 83";
		constexpr const char* leardx = "48 8D 15";
		constexpr const char* mov = "48 89 05";
	}

	template<typename T = uintptr_t>
	static T read(uintptr_t addr) {
		return *(T*)addr;
	}

	static std::vector<int> pattern_to_byte(const char* pattern) {
		auto bytes = std::vector<int>{};
		auto start = const_cast<char*>(pattern);
		auto end = const_cast<char*>(pattern) + strlen(pattern);
		for (auto current = start; current < end; ++current) {
			if (*current == '?') {
				++current;
				if (*current == '?') ++current;
				bytes.push_back(-1);
			}
			else {
				bytes.push_back(strtoul(current, &current, 16));
			}
		}
		return bytes;
	}

	template<typename T = uintptr_t>
	static T scanpat(const char* pattern, uintptr_t base) {
		if (!pattern || !base) return (T)0;
		auto dosHeader = (PIMAGE_DOS_HEADER)base;
		auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)base + dosHeader->e_lfanew);
		auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto patternBytes = pattern_to_byte(pattern);
		auto scanBytes = reinterpret_cast<std::uint8_t*>(base);
		auto s = patternBytes.size();
		auto d = patternBytes.data();

		for (auto i = 0ul; i < sizeOfImage - s; ++i) {
			bool found = true;
			for (auto j = 0ul; j < s; ++j) {
				if (scanBytes[i + j] != d[j] && d[j] != -1) {
					found = false;
					break;
				}
			}
			if (found) {
				return (T)&scanBytes[i];
			}
		}
		return (T)0;
	}

	static bool match(uintptr_t addr, const char* pattern) {
		auto bytes = pattern_to_byte(pattern);
		for (size_t i = 0; i < bytes.size(); i++) {
			if (bytes[i] != -1 && read<uint8_t>(addr + i) != bytes[i]) return false;
		}
		return true;
	}

	static std::map<std::string, uintptr_t> build_api_map(uintptr_t ga_base, uintptr_t up_base) {
		std::map<std::string, uintptr_t> apis;
		if (!up_base) return apis;

		auto dr3ammap = scanpat(strings::api, up_base);
		if (!dr3ammap) return apis;

		auto start = reinterpret_cast<uint8_t*>(dr3ammap);
		for (auto p = start; p < start + 0x2000; ++p) {
			if (!match((uintptr_t)p, strings::leardx)) continue;
			if (!match((uintptr_t)(p + 12), strings::mov)) continue;
			auto n = (char*)(p + read<int>((uintptr_t)(p + 3)) + 7);
			auto s = p + 12 + read<int>((uintptr_t)(p + 15)) + 7;
			auto ptr = read<uintptr_t>((uintptr_t)s);
			auto off = (ga_base && ptr) ? ptr - ga_base : 0;
			if (n) {
				apis[std::string(n)] = off;
			}
		}
		return apis;
	}
}
