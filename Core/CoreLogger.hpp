#pragma once
#include <fstream>

class CoreLogger final
{
public:

	// Prints warning to the output file
	static void PrintWarning(std::string message);

	// Prints error to the output file
	static void PrintError(std::string message);

	// Prints success to the output file
	static void PrintSuccess(std::string message);

	// Prints simple message to the output file
	static void PrintMessage(std::string message);

	// Opens output log file for message redirection 
	static void Initialize();

	CoreLogger() = delete;

	CoreLogger(const CoreLogger&) = delete;

	const CoreLogger& operator=(const CoreLogger&) = delete;

private:

	// Internal print function implementation for all print function types
	static void PrintInternal(const char* prefix, std::string& message);

	static std::ofstream m_output;
};