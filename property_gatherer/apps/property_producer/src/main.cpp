#include "stdafx.h"
#include "property_producer.h"
#include "command_line_parser.h"

int main(int argc, char** argv)
{
    CommandLineParser_C commandLineParser(argc, argv);
    PropertyProducerInitParams_C initParams;
    initParams.myInfo.destIp = "127.0.0.1";
    initParams.myInfo.destPort = 8002;
    if (true == commandLineParser.CmdOptionExists("-f") &&
        false == commandLineParser.GetCmdOption("-f").empty())
    {
        initParams.configFile = commandLineParser.GetCmdOption("-f");
    }

    PropertyProducerApp_C app(initParams);
    app.RunTest();
    return 0;
}