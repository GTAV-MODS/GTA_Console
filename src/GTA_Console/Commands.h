#pragma once

namespace Commands
{
	//Some global object :
	inline std::map <std::string_view, void (*)(std::vector <std::string>&)> CommandList;
	inline std::vector<std::string> lastArgs;
	inline bool callExecCommand;
	inline char InputBuf[256];

	//Function which manage the commands : 
	void InitCommands();
	void RegisterCommand(const char* command, void (*ptr)(std::vector<std::string>&));
	void ExecCommand();

	//Our commands function native : 
	void SpawnVehicle(const char* model, int R = 255, int G = 255, int B = 255);
	void SpawnPedsPattern(const int);
}