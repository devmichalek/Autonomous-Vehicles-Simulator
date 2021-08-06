#include "CoreEngine.hpp"
#include "TestEngine.hpp"

int main()
{
    TestEngine testEngine;

    CoreEngine coreEngine;
    if (!coreEngine.load())
    {
        coreEngine.errorLoop();
        return 1;
    }

    coreEngine.loop();
    return 0;
}