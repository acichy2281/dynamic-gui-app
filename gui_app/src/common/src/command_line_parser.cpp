#include "stdafx.h"
#include "command_line_parser.h"

CommandLineParser_C::CommandLineParser_C(int& argc, char** argv) {
	for (int i = 1; i < argc; ++i)
		this->tokens.push_back(std::string(argv[i]));
}

const std::string& CommandLineParser_C::GetCmdOption(const std::string& option) const 
{
	std::vector<std::string>::const_iterator itr;
	itr = std::find(this->tokens.begin(), this->tokens.end(), option);
	if (itr != this->tokens.end() && ++itr != this->tokens.end()) 
    {
		return *itr;
	}
	static const std::string empty_string("");
	return empty_string;
}

bool CommandLineParser_C::CmdOptionExists(const std::string& option) const 
{
	return std::find(this->tokens.begin(), this->tokens.end(), option)
		!= this->tokens.end();
}