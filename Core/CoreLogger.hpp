#include <fstream>

class CoreLogger final
{
public:

	static void PrintWarning(std::string message);

	static void PrintError(std::string message);

	static void PrintSuccess(std::string message);

	static void PrintMessage(std::string message);

	static void Initialize();

private:

	static void PrintInternal(const char* prefix, std::string& message);

	static std::ofstream m_output;
};