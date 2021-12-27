#include <stdafx.h>
#include <process.h>
#include <iostream>


unsigned int __stdcall showConsoleDebug(void* arg)
{
	AllocConsole();
	SetConsoleTitle("GTA PED SPAWNER BY SUPER COOL NINJA VERSION 1.0");
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	LOG_SUCCESS(); printf("Console debug mode thread started.\n");
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		GetModuleFileName(hInstance, g_szFileName, MAX_PATH);
		strcpy_s(g_szFileName, strrchr(g_szFileName, '\\') + 1);
		_beginthreadex(NULL, 0, &showConsoleDebug, NULL, 0, NULL);
		Memory::Init();
		scriptRegister(hInstance, Main::Loop);
		break;
	case DLL_PROCESS_DETACH:
		Main::Uninit();
		Memory::Uninit();
		scriptUnregister(hInstance);
		break;
	}

	return TRUE;
}