#pragma once

#include "InputVPacketManager.h"

#include "ServerInfo.h"
#include "GameInfo.h"
#include "PlayerInfo.h"

#include <vector>

namespace ServerConnector
{
	
	extern ServerInfo targetServer;
	extern bool isConnected;
	extern std::vector<GameInfo> gamesList;
	extern std::vector<PlayerInfo> playersList;

	extern GameInfo currentGame;

	extern bool muteAuxPackets;

	extern int globalStationID;

	bool connect(const ServerInfo& to, bool printLogs = 1);
	void disconnect();

	void updateGamesList();
	void updatePlayersList();

	bool findGameInfo(int gameID, GameInfo& writeInto);

	bool connect2Game(int gameID);
	bool registerSocket(sf::TcpSocket& writeInto);

	void processPacketStack();

};
