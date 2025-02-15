#pragma once

#include "VProtocol/ServerSocket.h"
#include "VProtocol/VPacket.h"
#include "VProtocol/InputVPacketManager.h"

#include "Cosmetic/AudioUtil.h"

#include "DefaultServersMap.h"
#include "IO/fFile.h"

#include <iostream>
#include <vector>
#include <cmath>

enum ShivaState {
	Calm,
	Attack,
	DprAttack,
};

namespace ShivaShell
{

	// Main run cycle functions
	void run();
	void init();
	void tick();

	// Inititalizating ShivaShell
	void createPassemblosTemplate();
	void createOverflowText();
	void loadHelpText();
	void printWelcomeText();

	// Receiving and parsing new command
	std::string getCommand();
	void convertBytes(std::string& string);
	void parseCommand(const std::string& commline);
	void routeCmdName(std::string& cmd);

	// Stage between command parsing and command executing
	bool isServerCanBeAttacked(); // Only for malwares

	// Commands executing; dc = Do Command
	void dc_Help();
	void dc_Shtd();
	void dc_Abrt();
	void dc_Pstdmp();
	void dc_Mtaux();
	void dc_Mtaud();

	void dcScn();
	void dcSdc();
	void dcSst();
	void dcSgl();
	void dcSmap();
	
	void dcGcn();
	void dcGpl();
	void dcGreg();
	void dcGms();

	void dcMmsw();
	void dcMurw();
	void dcMnni();
	void dcMmsi();
};

