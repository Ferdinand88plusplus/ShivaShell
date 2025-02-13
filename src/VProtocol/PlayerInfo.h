#pragma once

#include "VPI.h"
#include <string>

struct PlayerInfo
{
	unsigned char ID;
	unsigned char status;
	unsigned char worldID;
	short X, Y;
	std::string name;
	PlayerBody body;
};

