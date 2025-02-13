#pragma once

#include "VProtocol/ServerConnector.h"
#include "VProtocol/VPacket.h"
#include "VProtocol/MainSocket.h"

#include "Cosmetic/AudioUtil.h"

#include "IO/PredefServersConfigParser.h"
#include "IO/fFile.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <cmath>

enum ShivaState {
	Calm,
	Attack,
	DprAttack,
};

class ShivaShell
{
public:

	void run();

private:
	
	fStringStream commlineParser;
	bool abortSession = false;

	ShivaState currentState = ShivaState::Calm;

	std::vector<PredefServerInfo> predefServers;
	PredefServerInfo* predefCurrentInfo = 0;
	
	std::string helpText;
	
	void init();
	void tick();

	void initPassemblosTemplate();
	void loadPredefServers();

	std::string getCommand();
	void parseCommand(const std::string& commline);
	void routeCmdName(std::string& cmd);

	bool malwareCheckServer();

	void openMassPacketThreads(int threadNum);

	// dc = Do Command

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

	void dcMgnn();
	void dcMnni();
	void dcMcmp();
	void dcMmsi();
};

