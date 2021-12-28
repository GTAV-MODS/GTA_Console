#pragma once

class Command
{
public :
	Command() = default;
	~Command() = default;

	static void RegisterCommand(const char* command, void (*ptr)());
	static void ExecCommand(const char* command_line);
};

