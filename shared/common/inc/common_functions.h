#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

/* System includes */
#include <thread>

/* Platform Includes */
#ifdef _WIN32
#include "windows_platform_functions.h"
#elif defined(__linux__)
#include "linux_platform_functions.h"
#else
#error "Unsupported platform"
#endif

bool IsUserQuit();
void SleepMs(int64_t sleepTime);
std::string GetJSONFile();

#endif // COMMON_FUNCTIONS_H