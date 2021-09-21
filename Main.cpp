#include "CoreEngine.hpp"
#include "TestEngine.hpp"
#include "CoreLogger.hpp"

int main()
{
    try
    {
        TestEngine testEngine;
        CoreEngine coreEngine;
    }
    catch (...)
    {
        CoreLogger::PrintError("Caught exception!");
    }

    return 0;
}