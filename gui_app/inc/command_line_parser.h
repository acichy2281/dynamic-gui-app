#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include "stdafx.h"

class CommandLineParser_C
{
public:
    CommandLineParser_C(int& argc, char** argv);
    const std::string& GetCmdOption(const std::string& option) const;
    bool CmdOptionExists(const std::string& option) const;
private:
    std::vector <std::string> tokens;
};

#endif // COMMAND_LINE_PARSER_H