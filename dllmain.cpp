#include <windows.h>
#include <iostream>
#include <psapi.h>
#include "memcury.h"
#include "EngineVersionBase.h"
// SDK FString Code
template<class T>
class TArray
{
public:
	T* Data;
	int Count;
	int Max;
};

class FString : public TArray<wchar_t>
{
public:

	inline const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		const auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
};




class FEngineVersion : FEngineVersionBase
{
public:



	/** Generates a version string */
	void ToString(EVersionComponent LastComponent = EVersionComponent::Branch) const
	{
		std::cout << Major << "." << Minor << "." << Patch << std::endl;

	};




private:

	/** Branch name. */
	FString Branch;
};

uint8_t* GetAddressPTR(uint8_t* ptr, uint8_t offset, uint8_t instr_size)
{
	return (ptr + *(int32_t*)(ptr + offset) + instr_size);
}

void* Result;
DWORD_PTR VersionCurrent_addr;
FEngineVersion* GetEngineCurrent(void* Result)
{
	return reinterpret_cast<struct FEngineVersion* (__fastcall*)(void*)>(VersionCurrent_addr)(Result);
}
DWORD _stdcall InitEngineVersion(LPVOID)
{
	DWORD_PTR BaseAddress = (DWORD_PTR)GetModuleHandle(NULL);
	MODULEINFO ModuleInfo;
	GetModuleInformation(GetCurrentProcess(), (HMODULE)BaseAddress, &ModuleInfo, sizeof(ModuleInfo));
	VersionCurrent_addr = (DWORD_PTR)(GetAddressPTR((uint8_t*)(Memcury::Scanner::FindStringRef(L"%sunreal-v%i-%s.dmp").ScanFor({ 0x48,0x8B,0xC8,0xE8 }, false).Get()) - 0x5, 0x1, 0x5));

	GetEngineCurrent(&Result)->ToString();
	return NULL;
}

BOOL __stdcall DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		DisableThreadLibraryCalls(hModule);

		AllocConsole();
		ShowWindow(GetConsoleWindow(), SW_SHOW);
		FILE* fp;
		freopen_s(&fp, "CONOIN$", "r", stdin);
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
		std::cout << "DLL Attached" << std::endl;
		CreateThread(0, 0, InitEngineVersion, 0, 0, 0);
		break;
	}
	case DLL_PROCESS_DETACH:
	{
		break;
	}
	}

	return TRUE;
}