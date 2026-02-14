
#pragma once

#include "imgui.h"

namespace gui
{
namespace font
{
struct Fonts
{
	ImFont* base = nullptr;
	ImFont* title = nullptr;
};

const Fonts& Get();
void Init();
void Shutdown();
}
}
