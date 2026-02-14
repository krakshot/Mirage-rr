#include "Il2CppThread.h"

namespace IL2CPP {
	void Il2CppThread::Detach() {
		spoof_call(var::unitySpoof, Pointers::Exports.il2cpp_thread_detach, (void*)this);
	}
}