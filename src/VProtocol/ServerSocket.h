#pragma once

#include <SFML/Network.hpp>

#include "ServerInfo.h"
#include "GameInfo.h"
#include "PlayerInfo.h"

class ServerSocket
{
public:
    // Info about network transfering
    sf::TcpSocket socket;
    bool ignoreAuxiliaries = true;
    
    // Info about connected server
    ServerInfo* connectedServer = 0; // Nullptr if not connected
    std::vector<GameInfo> serverGames;
    
    // Info about connected game
    GameInfo* connectedGame = 0; // Nullptr if not connected
    std::vector<PlayerInfo> gamePlayers;

    // Info about my client
    int myPlayerID = -1;
    
	bool connect(const ServerInfo& to, bool printLogs = 1);
	void disconnect();

	void updateGamesList();
	void updatePlayersList();

	bool findGameInfo(int gameID, GameInfo& writeInto);

	bool connect2Game(int gameID);

	void processPacketStack();

private:

    bool identificate();

    void errNoServResponse();

    static std::ostream& error();
};

extern ServerSocket serverSocket;