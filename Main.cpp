#include "CEngine.hpp"
#include "TEngine.hpp"

int main()
{
    TEngine tengine;
    CEngine cengine;
    cengine.load();
    cengine.loop();
    return 0;
}