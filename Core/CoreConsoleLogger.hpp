#include <string>

class CoreConsoleLogger
{
public:

	static void PrintWarning(std::string message);

	static void PrintError(std::string message);

	static void PrintSuccess(std::string message);

	static void PrintMessage(std::string message);

private:

	static void PrintInternal(const char* prefix, std::string& message, unsigned short color);
};