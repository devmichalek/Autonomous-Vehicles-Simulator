#include "CoreLogger.hpp"
#include <algorithm>
#include <chrono>
#include <ctime>

std::ofstream CoreLogger::m_output;

void CoreLogger::PrintWarning(std::string message)
{
	PrintInternal("Warning: ", message);
}

void CoreLogger::PrintError(std::string message)
{
	PrintInternal("Error: ", message);
}

void CoreLogger::PrintSuccess(std::string message)
{
	PrintInternal("Success: ", message);
}

void CoreLogger::PrintMessage(std::string message)
{
	PrintInternal("Message: ", message);
}

void CoreLogger::Initialize()
{
	const auto now = std::chrono::system_clock::now();
	std::time_t cnow = std::chrono::system_clock::to_time_t(now);
	std::string filename = std::ctime(&cnow);
	std::replace(filename.begin(), filename.end(), ' ', '_');
	std::replace(filename.begin(), filename.end(), ':', '_');
	m_output.open(filename.substr(0, filename.size() - 1) + ".log");
}

void CoreLogger::PrintInternal(const char* prefix, std::string& message)
{
	if (m_output.is_open())
		m_output << prefix << message << std::endl;
}
