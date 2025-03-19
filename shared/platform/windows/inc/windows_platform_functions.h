#ifndef _WINDOWS_PLATFORM_FUNCTIONS_H_
#define _WINDOWS_PLATFORM_FUNCTIONS_H_

/* System include */
#include <iostream>
#include <string>

/* Platform includes */
#include <conio.h>

bool Windows_IsUserQuit();
std::string Windows_GetJSONFile();
std::string Windows_OpenFileDialog();

#endif // _WINDOWS_PLATFORM_FUNCTIONS_H_