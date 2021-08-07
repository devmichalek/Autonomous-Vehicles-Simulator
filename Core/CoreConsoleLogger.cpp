#include "CoreConsoleLogger.hpp"
#include <iostream>
#include <windows.h>

void CoreConsoleLogger::PrintWarning(std::string message)
{
	PrintInternal("Warning: ", message, 14);
}

void CoreConsoleLogger::PrintError(std::string message)
{
	PrintInternal("Error: ", message, 12);
}

void CoreConsoleLogger::PrintSuccess(std::string message)
{
	PrintInternal("Success: ", message, 10);
}

void CoreConsoleLogger::PrintMessage(std::string message)
{
	PrintInternal("Message: ", message, 15);
}

void CoreConsoleLogger::PrintInternal(const char* prefix, std::string& message, unsigned short color)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, color);
	std::cout << prefix;
	SetConsoleTextAttribute(handle, 7);
	std::cout << message << std::endl;
}