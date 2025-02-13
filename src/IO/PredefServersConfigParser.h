#pragma once

#include <fstream>
#include <sstream>

#include "../VProtocol/ServerInfo.h"
#include "../Tools/fStringStream.h"

enum PDSFlags {
	PDSF_Deprecated = 1 << 0,
};

struct PredefServerInfo {
	std::string name;
	std::string engine;
	std::string description;
	std::string host;
	port_t port;

	char flags = 0;

	PredefServerInfo() {};
};


class PredefServersConfigParser
{
public:

	void parse(std::vector<PredefServerInfo>& outputBuffer);

private:

	std::string configName = "data/predefservers.txt";

	std::ostream& error();

};

