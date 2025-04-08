/* Project includes */
#include "stdafx.h"
#include "command_line_parser.h"
#include "dynamic_gui.h"

void WidgetEventCb(WidgetDescriptor_T& widgetDesc, WidgetValueVariant_T val)
{
    std::cout << "Widget event callback: Widget ID = " << widgetDesc.widgetId << ", Value = ";
    std::visit([](auto&& arg) { std::cout << arg; }, val);
    std::cout << "\n";
}

int main(int argc, char** argv)
{
    CommandLineParser_C commandLineParser(argc, argv);
    DynamicGui_C app;

    DynamicGuiInitParams_T initParams;
    initParams.callbacks.onWidgetEventOccured = WidgetEventCb;
    initParams.callbacks.onWindowClose = []() { std::cout << "Window closed\n"; };
    if (false == app.InitializeGui(initParams))
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
        if (false == app.RunGui())
        {
            return -1;
        }
    }
    else
    {
        if (false == app.RunGui())
        {
            return -1;
        }
    }
    guiServerThread.join();

    return 0;
}