#pragma once

#include "VPacket.h"

#include <vector>

class InputVPacketManager
{
public:

	void receive();

	bool waitPacket(vph_t typeFilter, VPacket& writeBuffer);

	std::vector<VPacket> packetStack;

private:
	bool findPacket(vph_t typeFilter, VPacket& writeBuffer);

	VPacket* currentPacket = 0;
};



extern InputVPacketManager inputPacketsStream;