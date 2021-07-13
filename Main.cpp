#include "CoreEngine.hpp"
#include "TestEngine.hpp"

int main()
{
    TestEngine testEngine;
    CoreEngine coreEngine;
    coreEngine.load();
    coreEngine.loop();
    return 0;
}