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
template<typename T>
void PrintVariant(const T& value)
{
    std::visit([](auto&& arg) {
        using Type = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<Type, uint8_t> || std::is_same_v<Type, int8_t>) {
            std::cout << +arg;  // The unary + promotes to int while preserving value
        } else {
            std::cout << arg;
        }
    }, value);
}

#endif // COMMON_FUNCTIONS_H