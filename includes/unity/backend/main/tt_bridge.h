#pragma once
#include <map>
#include <string>
#include <cstdint>


namespace tt_bridge {
	void populate_apis(uintptr_t ga_base, uintptr_t up_base, std::map<std::string, uintptr_t>& out);
}
