#include <windows.h>

bool Hook(void* toHook, void* ourFunc, int len) {
	if (len < 5) {
		return false;
	}
	DWORD curProtection;
	VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

	memset(toHook, 0x90, len); 

	DWORD relativeAddy = ((DWORD)ourFunc - (DWORD)toHook) - 5; 

	*(BYTE*)toHook = 0xE9; 
	*(DWORD*)((DWORD)toHook + 1) = relativeAddy; 

	DWORD temp;
	VirtualProtect(toHook, len, curProtection, &temp);

	return true;
}

DWORD jumpBackAddy;
void _declspec(naked)ourFunc(){ 

	_asm {
		//asm code
		add [esi], 2
		push edi
		mov edi, [esp + 14]

		//jmp to ori code
		jmp [jumpBackAddy]
	}
}

DWORD WINAPI MainThread(LPVOID param) {

	//MUST CHANGE{
	int hookLen = 7; 
	DWORD hookAddy = 0x4637E9;
	//MUST CHANGE}


	jumpBackAddy = hookAddy + hookLen; 

	Hook((void*)hookAddy, ourFunc, hookLen); 

	while (true) {
		if (GetAsyncKeyState(VK_ESCAPE)) break;
		Sleep(50);
	}

	FreeLibraryAndExitThread((HMODULE)param, 0);

	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved) {
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, MainThread, hModule, 0, 0);
		break;
	}

	return TRUE;
}
