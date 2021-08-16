#include "CoreLogger.hpp"
#include <random>
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
	std::random_device device;
	std::mt19937 engine(device());
	std::uniform_int_distribution<std::mt19937::result_type> distribution(0, 9);

	// Generate filename
	size_t attempt = 3;
	size_t hashLength = 64;
	while (attempt--)
	{
		std::string filename;
		filename.resize(hashLength);
		for (auto& character : filename)
			character = static_cast<char>(distribution(engine) + 48);

		m_output.open(filename + ".log");
		if (m_output.is_open())
		{
			auto now = std::chrono::system_clock::now();
			std::time_t cnow = std::chrono::system_clock::to_time_t(now);
			m_output << std::ctime(&cnow) << std::endl;
			break;
		}
	}
}

void CoreLogger::PrintInternal(const char* prefix, std::string& message)
{
	if (m_output.is_open())
		m_output << prefix << message << std::endl;
}
