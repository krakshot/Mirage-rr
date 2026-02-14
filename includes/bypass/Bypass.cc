#include "Bypass.hh"
#include "../../src/utils/HookManager.hpp"
#include <Windows.h>
typedef void(__fastcall* tret)();
tret oret = nullptr;
void __fastcall hret() { return; }
namespace Bypass {
	bool active = false;
	void Setup()
	{

		if (!active)
		{

			constexpr auto Ref1 = 0x21AF2D0;
			constexpr auto Ref2 = 0x218CC40;
			constexpr auto Ref3 = 0x2196900;
			pHookManager->AddInline<tret>((uint64_t)GetModuleHandleA("Referee.dll") + Ref1, hret, &oret);
			pHookManager->AddInline<tret>((uint64_t)GetModuleHandleA("Referee.dll") + Ref2, hret, &oret);
			pHookManager->AddInline<tret>((uint64_t)GetModuleHandleA("Referee.dll") + Ref3, hret, &oret);
			active = true;
		}

	}
}