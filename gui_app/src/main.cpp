/* Project includes */
#include "stdafx.h"
#include "command_line_parser.h"
#include "dynamic_gui.h"

int main(int argc, char** argv)
{
    CommandLineParser_C commandLineParser(argc, argv);
    DynamicGui_C app;

    if (false == app.Initialize())
    {
        std::cerr << "Error: Failed to initialize GUI app\n";
        return -1;
    }
    
    GuiServerInitParams_T guiServerInitParams;
    guiServerInitParams.serverInfo.destIp = "127.0.0.1";
    guiServerInitParams.serverInfo.destPort = 8001;
    guiServerInitParams.clientInfo.destIp = "127.0.0.1";
    guiServerInitParams.clientInfo.destPort = 8000;
    guiServerInitParams.rxBufferSize = 2048;
    guiServerInitParams.spinLockSleepMs = 10;
    
    std::thread guiServerThread(&DynamicGui_C::RunGuiServer, std::ref(app), guiServerInitParams);

    if (true == commandLineParser.CmdOptionExists("-f") &&
        false == commandLineParser.GetCmdOption("-f").empty())
    {
        app.SetConfigFile(commandLineParser.GetCmdOption("-f"));
        if (false == app.Run())
        {
            return -1;
        }
    }
    else
    {
        if (false == app.Run())
        {
            return -1;
        }
    }
    guiServerThread.join();

    return 0;
}