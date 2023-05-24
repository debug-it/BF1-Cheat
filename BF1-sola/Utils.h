#pragma once

#include <Windows.h>
#include <psapi.h>
#include <cstdint>
#include <cstddef>
#include <sstream>
namespace Utils
{

	namespace Variables
	{
		unsigned __int64 ImageBase;
		HMODULE ImageModuleHandle;
		MODULEINFO ImageModuleInfo;
	}
	namespace Cheat
	{
		int menuCount = 0;
		bool menuOpen = false;
		bool boxesp = true;
		bool vischeck = true;
		bool aimbot = false;
		bool infoesp = false;
		bool healthesp = false;
		bool skeletonesp = false;
	}
}
