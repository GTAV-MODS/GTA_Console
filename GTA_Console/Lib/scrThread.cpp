#include <stdafx.h>

static void OnHook()
{
	Handle handle;

	handle = Memory::FindPattern("48 8B 05 ? ? ? ? 48 89 0C 06");
	if (handle.IsValid())
	{
		rage::scrThread::ms_ppThreads = handle.At(2).Into().Value<rage::scrThread**>();
		LOG_INFO(); printf("Found rage::scrThread::sm_Threads\n");
	}

	handle = Memory::FindPattern("66 89 3D ? ? ? ? 85 F6");
	if (handle.IsValid())
	{
		rage::scrThread::ms_pcwThreads = handle.At(2).Into().Get<WORD>();
		LOG_INFO(); printf("Found rage::scrThread::_sm_cwThreads\n");
	}

	handle = Memory::FindPattern("48 89 05 ? ? ? ? EB 07 48 89 1D ? ? ? ? 66 89 35 ? ? ? ? 85 FF");
	if (handle.IsValid())
	{
		rage::scrThread::ms_pStacks = handle.At(2).Into().Value<rage::_scrStack*>();
		LOG_INFO(); printf("Found rage::scrThread::sm_Stacks\n");
	}

	handle = Memory::FindPattern("66 89 35 ? ? ? ? 85 FF");
	if (handle.IsValid())
	{
		rage::scrThread::ms_pcwStacks = handle.At(2).Into().Get<WORD>();
		LOG_INFO(); printf("Found rage::scrThread::_sm_cwStacks\n");
	}
}

static RegisterHook hook(OnHook);