#include "stdafx.h"
#include "Main.h"
#include <cstdio>


/*
											*\(-_-)/*
						--------||Script made by Super.Cool.Ninja||--------
--------||Thanks a lot to Pongo1231 for having release this source code based with IMGUI hooked.||--------
					Pongo source code: https://github.com/pongo1231/V_ScriptViewer
*/

static bool ms_bDidInit = false; //Check if the thread has been reloaded.
static bool ms_bDidImguiInit = false; //Check if IMGUI has been init.

Console console;


//Change State (OPEN/CLOSE) CONSOLE CONTROL(F8) :
static bool ms_isConsoleOpen = false;
static bool ms_bHasConsoleOpenStateJustChanged = false;
static inline void SetConsoleOpenState(bool state)
{
	ms_isConsoleOpen = state;
	ms_bHasConsoleOpenStateJustChanged = true;
}

static LRESULT(*OG_WndProc)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static LRESULT HK_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ms_bDidImguiInit)
	{
		switch (uMsg)
		{
		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_F8:
				SetConsoleOpenState(!ms_isConsoleOpen);
				break;
			}
			break;
		}
	}
		
	if (ms_isConsoleOpen)
	{
		ImGuiIO& io = ImGui::GetIO();

		switch (uMsg)
		{
		case WM_LBUTTONDOWN:
			io.MouseDown[0] = true;

			break;
		case WM_LBUTTONUP:
			io.MouseDown[0] = false;

			break;
		case WM_MOUSEWHEEL:
			io.MouseWheel = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;

			break;
		case WM_KEYDOWN:
			if (wParam < 512)
			{
				io.KeysDown[wParam] = true;
			}

			break;
		case WM_KEYUP:
			if (wParam < 512)
			{
				io.KeysDown[wParam] = false;
			}

			break;
		case WM_CHAR:
			io.AddInputCharacter(wParam);
			break;
		}
	}

	return OG_WndProc(hWnd, uMsg, wParam, lParam);
}

static void** ms_pPresentVftEntryAddr = nullptr;
static HRESULT(*OG_OnPresence)(IDXGISwapChain* pSwapChain, UINT uiSyncInterval, UINT uiFlags);
static HRESULT HK_OnPresence(IDXGISwapChain* pSwapChain, UINT uiSyncInterval, UINT uiFlags)
{
	//CHECK IF THE IMGUI IS NOT INIT YET THEN WE HOOK AND ADD SOME STYLE TO OUR MAIN IMGUI INTERFACE :
	if (!ms_bDidImguiInit)
	{
		ID3D11Device* pDevice = nullptr;
		pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&pDevice));

		ID3D11DeviceContext* ms_pDeviceContext = nullptr;
		pDevice->GetImmediateContext(&ms_pDeviceContext);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(Memory::g_hWnd);
		ImGui_ImplDX11_Init(pDevice, ms_pDeviceContext);

		ms_bDidImguiInit = true;

		LOG_SUCCESS(); printf("Dear ImGui initialized and ready to use!\n");
		LOG_INFO(); printf("F8 to open the console.\n");
	}
	//CHECK IF THE CONSOLE IS OPEN THEN WE SETUP THE CONSOLE INTERFACE  :
	else if (ms_isConsoleOpen)
	{
		if (ms_bHasConsoleOpenStateJustChanged)
			ms_bHasConsoleOpenStateJustChanged = false;

		ShowCursor(true);

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::SetNextWindowSize({ 200.f, 100.f });

		console.ShowConsole(&ms_isConsoleOpen);
		
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	else
	{
		if (ms_bHasConsoleOpenStateJustChanged)
		{
			ms_bHasConsoleOpenStateJustChanged = false;
			ShowCursor(false);
		}
	}

	return OG_OnPresence(pSwapChain, uiSyncInterval, uiFlags);
}

static void** ms_pResizeBuffersAddr = nullptr;
static HRESULT(*OG_ResizeBuffers)(IDXGISwapChain* pSwapChain, UINT uiBufferCount, UINT uiWidth, UINT uiHeight, DXGI_FORMAT newFormat, UINT uiSwapChainFlags);
static HRESULT HK_ResizeBuffers(IDXGISwapChain* pSwapChain, UINT uiBufferCount, UINT uiWidth, UINT uiHeight, DXGI_FORMAT newFormat, UINT uiSwapChainFlags)
{
	if (ms_bDidImguiInit)
		ImGui_ImplDX11_InvalidateDeviceObjects();
	
	HRESULT hResult = OG_ResizeBuffers(pSwapChain, uiBufferCount, uiWidth, uiHeight, newFormat, uiSwapChainFlags);

	if (ms_bDidImguiInit)
		ImGui_ImplDX11_CreateDeviceObjects();

	return hResult;
}

static __int64(*OG_ScriptRunHook)(rage::scrThread* pScrThread);
static __int64 HK_ScriptRunHook(rage::scrThread* pScrThread)
{
	const auto& startTimestamp = Util::GetTimeMcS();
	__int64 result = OG_ScriptRunHook(pScrThread);
	DWORD64 qwExecutionTime = Util::GetTimeMcS() - startTimestamp;
	return OG_ScriptRunHook(pScrThread);
}

void Main::Uninit()
{
	if (ms_bDidImguiInit)
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();

		ImGui::DestroyContext();
	}

	if (ms_pPresentVftEntryAddr)
		Memory::Write<void*>(ms_pPresentVftEntryAddr, *OG_OnPresence);

	if (ms_pResizeBuffersAddr)
		Memory::Write<void*>(ms_pResizeBuffersAddr, *OG_ResizeBuffers);
}

static bool InitSwapChainHooks()
{
	// Create dummy device and swap chain to patch IDXGISwapChain's vftable
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};

	swapChainDesc.BufferDesc.Width = 800;
	swapChainDesc.BufferDesc.Height = 600;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = Memory::g_hWnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_10_0;
	IDXGISwapChain* pSwapChain = nullptr;

	auto D3D11CreateDeviceAndSwapChain = reinterpret_cast<HRESULT(*)(IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL*, UINT, UINT,
		const DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**)>
		(GetProcAddress(GetModuleHandle("d3d11.dll"), "D3D11CreateDeviceAndSwapChain"));

	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, NULL, NULL, NULL);

	if (!pSwapChain)
	{
		LOG_ERROR(); printf("Swap Chain was invalid, aborting! \n");
		return false;
	}

	Handle handle = Handle(*reinterpret_cast<DWORD64*>(pSwapChain));

	ms_pPresentVftEntryAddr = handle.At(64).Get<void*>();
	OG_OnPresence = *reinterpret_cast<HRESULT(**)(IDXGISwapChain*, UINT, UINT)>(ms_pPresentVftEntryAddr);

	Memory::Write<void*>(ms_pPresentVftEntryAddr, HK_OnPresence);

	LOG_SUCCESS(); printf("Hooked IDXGISwapChain::Present through vftable injection\n");

	ms_pResizeBuffersAddr = handle.At(104).Get<void*>();
	OG_ResizeBuffers = *reinterpret_cast<HRESULT(**)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT)>(ms_pResizeBuffersAddr);

	Memory::Write<void*>(ms_pResizeBuffersAddr, HK_ResizeBuffers);

	LOG_SUCCESS(); printf("Hooked IDXGISwapChain::ResizeBuffers through vftable injection\n");

	pSwapChain->Release();

	return true;
}

void Main::Loop()
{
	if (ms_bDidInit)
	{
		LOG_INFO(); printf("Main thread has restarted\n");
	}
	else
	{
		Memory::InitHooks();
		Handle handle;

		// Had no luck with SetWindowsHookEx so we're just going to straight up hook WndProc
		handle = Memory::FindPattern("48 8D 05 ? ? ? ? 33 C9 44 89 75 20");
		if (handle.IsValid())
		{
			Memory::AddHook(handle.At(2).Into().Get<void>(), HK_WndProc, &OG_WndProc);
			LOG_SUCCESS(); printf("Hooked WndProc\n");
		}

		handle = Memory::FindPattern("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 20 48 8D 81 D0 00 00 00");

		if (handle.IsValid())
		{
			Memory::AddHook(handle.Get<void>(), HK_ScriptRunHook, &OG_ScriptRunHook);
			LOG_SUCCESS(); printf("Hooked rage::scrThread::Run\n");
		}

		if (InitSwapChainHooks())
		{
			Memory::FinishHooks();

			if (rage::scrThread::ms_ppThreads && rage::scrThread::ms_pcwThreads)
			{
				ms_bDidInit = true;
				LOG_INFO(); printf("Main thread ready!\n");
			
				console.RegisterCommand("/help", [](std::vector<std::string> args) { console.AddLog("List of available commands :"); for (auto const& cmd : console.UserCommandList) console.AddLog("%s", (const char*)cmd.first.data()); });
				console.RegisterCommand("/test", [](std::vector<std::string> args) { console.AddLog("Hello world TEST"); });
				console.RegisterCommand("/clear", [](std::vector<std::string> args) {console.ClearLog(); });
			}
		}
	}

	while (true)
	{
		WAIT(0);

		if (!ms_bDidImguiInit)
			continue;

		if (ms_isConsoleOpen)
			DISABLE_ALL_CONTROL_ACTIONS(0);
	}
}