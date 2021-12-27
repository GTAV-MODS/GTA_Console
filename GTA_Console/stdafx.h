#pragma once

#include "Main.h"

#include "Memory/Memory.h"
#include "Memory/Handle.h"
#include "Memory/Hook.h"
#include "Memory/Hwnd.h"

#include "Util/Misc.h"
#include "Lib/scrThread.h"

#include "../vendor/scripthookv/inc/main.h"
#include "../vendor/scripthookv/inc/natives.h"

#include "../vendor/minhook/include/MinHook.h"

#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_internal.h"
#include "../vendor/imgui/backends/imgui_impl_dx11.h"
#include "../vendor/imgui/backends/imgui_impl_win32.h"

#include "Commands.h"
#include "Utils.h"
#include "Console.h"


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <Psapi.h>

#include <dxgi.h>
#include <d3d11.h>

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <cstring>
#include <algorithm>
#include <cstdio>



//Set the text color to GREEN : 
#define LOG_SUCCESS()                         \
{                                          \
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); \
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN); \
}

//Set the text color to RED : 
#define LOG_ERROR()                         \
{                                          \
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); \
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED); \
}

//Set the text color to INTENSITY : 
#define LOG_INFO()                         \
{                                          \
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); \
	SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY); \
}
