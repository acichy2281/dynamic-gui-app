#include "windows_platform_functions.h"
#include <windows.h>
#include <commdlg.h>

bool Windows_IsUserQuit()
{
    bool retVal = false;        
    if (_kbhit()) {
        // Read the pressed key
        char ch = _getch();
        if (ch == 'q' || ch == 'Q') {
            retVal = true;
            std::cout << "\nQuit key pressed. Exiting..." << std::endl;
        }
    }
    return retVal;
}

std::string Windows_OpenFileDialog() {
    OPENFILENAMEA ofn;
    char fileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "JSON Files (*.json)\0*.json\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt = "json";

    if (GetOpenFileName(&ofn)) {
        return std::string(fileName);
    }
    else {
        return "";
    }
}

std::string Windows_GetJSONFile()
{
    std::string filePath = Windows_OpenFileDialog();

    if (!filePath.empty()) {
        return filePath;
    }
    else {
        return "";
    }
}