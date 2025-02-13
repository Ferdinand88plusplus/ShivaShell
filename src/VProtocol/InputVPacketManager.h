#pragma once

#include "VPacket.h"

#include <vector>
#include <thread>

class InputVPacketManager
{
public:

	void receive();

	void init();

	// returns result of search (false = failed, true = success)
	bool takePacket(vph_t typeFilter, VPacket& writeBuffer);

	bool waitPacket(vph_t typeFilter, VPacket& writeBuffer);

	std::vector<VPacket> packetStack;

	//std::vector<std::string> testStack;

private:

	VPacket* currentPacket = 0;
};


extern InputVPacketManager inputPacketsStream;