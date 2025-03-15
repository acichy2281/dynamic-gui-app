/* Project includes */
#include "stdafx.h"
#include "command_line_parser.h"
#include "dynamic_gui.h"

int main(int argc, char** argv)
{
    CommandLineParser_C commandLineParser(argc, argv);
    DynamicGui_C app;

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
        std::string a = GetJSONFile();
        app.SetConfigFile(a);
        if (false == app.Run())
        {
            return -1;
        }
    }

    return 0;
}