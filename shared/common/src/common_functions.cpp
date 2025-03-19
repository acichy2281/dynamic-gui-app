#include "common_functions.h"

bool IsUserQuit()
{
#ifdef _WIN32
    return Windows_IsUserQuit();
#elif defined(__linux__)
    return Linux_IsUserQuit();
#else
    return false;
#endif
}

void SleepMs(int64_t sleepTime)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
}

std::string GetJSONFile()
{
#ifdef _WIN32
    return Windows_GetJSONFile();
#else
    return "";
#endif
}
