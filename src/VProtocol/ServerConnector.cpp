#include "ServerConnector.h"

#include "../Tools/SocketTools.h"
#include "VPacket.h"
#include "MainSocket.h"

#include <iostream>

// local server connector functions storage
namespace ServerConnector
{

	void processMessage(VPacket& messagePacket);

	/// ServerConnector error templates ///

	// Server IO Error - Uncomplete Packet
	void sIOErrUP();

	/// ServerConnector error baseplates ///
	static std::ostream& connectionError();
	static std::ostream& serverIOError();

	bool identificate();
}

// local server connector variables storage
namespace ServerConnector
{
	ServerInfo targetServer;
	bool isConnected = false;
	std::vector<GameInfo> gamesList;
	std::vector<PlayerInfo> playersList;
	GameInfo currentGame;

	bool muteAuxPackets = true;

	int globalStationID;

	sf::Time maxConnectionTime = sf::seconds(5.0f);
	sf::Time maxServerResponseTime = sf::seconds(7.0f);
	sf::Clock keepalivePacketClock;
}

bool ServerConnector::connect(const ServerInfo& to, bool printLogs)
{
	if (isConnected) {
		/*
		if (printLogs)
			connectionError() << "This server connector is alredy connected with another server.\n";
		return 0;
		*/

		disconnect();
	}

	sf::Socket::Status connectionStatus = mainSocket.connect(to.IP, to.Port, maxConnectionTime);

	if (connectionStatus != sf::Socket::Status::Done) {
		if (printLogs)
			connectionError() << "Status=" << ST::stringStatus[connectionStatus] << '\n';
		return 0;
	}

	if (!identificate()) {
		if (printLogs) 
			connectionError() << "Identification failed\n";
		return 0;
	}

	isConnected = true;

	targetServer = to;

	keepalivePacketClock.restart();

	return 1;
}

void ServerConnector::disconnect()
{

	isConnected = false;

	mainSocket.disconnect();
}

void ServerConnector::updateGamesList()
{
	gamesList.clear();

	VPacket gamesListQuery(GAMES_LIST_QUERY);
	gamesListQuery.sendToServer();

	VPacket gamesListResult;
	if (!inputPacketsStream.waitPacket(GAMES_LIST_RESPONSE, gamesListResult)) {
		return;
	}

	unsigned char numGames;

	if (!gamesListResult.readAny(numGames)) {
		sIOErrUP();
		return;
	}

	for (int i = 0; i < numGames; i++) {

		GameInfo& game = gamesList.emplace_back();
		
		if (!gamesListResult.readAny(game.ID)) {
			sIOErrUP();
			return;
		}

		if (!gamesListResult.readString(game.name)) {
			sIOErrUP();
			return;
		}
	}
}

void ServerConnector::updatePlayersList()
{
	VPacket playersListQuery(TOTAL_PLAYERS_DATA_QUERY);
	playersListQuery.sendToServer();

	VPacket playersListResponse;
	inputPacketsStream.waitPacket(TOTAL_LIST_OF_PLAYERS_DATA, playersListResponse);

	unsigned char numPlayers = 0;

	playersListResponse.readAny(numPlayers);
	
	playersList.clear();

	for (int i = 0; i < numPlayers; i++)
	{
		PlayerInfo& player = playersList.emplace_back();

		playersListResponse.readAny(player.ID);
		playersListResponse.readAny(player.status);
		playersListResponse.readAny(player.worldID);
		playersListResponse.readAny(player.X);
		playersListResponse.readAny(player.Y);
		playersListResponse.readString(player.name);
		playersListResponse.readAny(player.body);
	}
}

bool ServerConnector::findGameInfo(int gameID, GameInfo& writeInto)
{
	bool listUpdated = 0;

gamesSearch:
	for (GameInfo& game : gamesList) {
		if (game.ID == gameID) {
			writeInto = game;
			return 1;
		}
	}

	if (!listUpdated) {
		updateGamesList();
		listUpdated = 1;
		goto gamesSearch;
	}

	return 0;
}

bool ServerConnector::connect2Game(int gameID)
{
	VPacket attachQuery(ATTACH_TO_GAME);
	attachQuery.writeAny(gameID);
	attachQuery.sendToServer();

	VPacket attachResponse;
	inputPacketsStream.waitPacket(ATTACH_TO_GAME_RESPONSE, attachResponse);

	int totalGameID;

	if (!attachResponse.readAny(totalGameID)) {
		sIOErrUP();
		return 0;
	}

	bool configured;

	if (!attachResponse.readAny(configured)) {
		sIOErrUP();
		return 0;
	}

	short birthTimeOff;
	
	if (!attachResponse.readAny(birthTimeOff)) {
		sIOErrUP();
		return 0;
	}

	if (!attachResponse.readAny(globalStationID)) {
		sIOErrUP();
		return 0;
	}

	//totalGameID = gameID;

	if (!findGameInfo(totalGameID, currentGame)) {
		std::cout << "Failed to find game with ID " << totalGameID << '\n';
		return 0;
	}

	VPacket ztimeResponse;
	inputPacketsStream.takePacket(zTIME_RESPONSE, ztimeResponse);

	return 1;
}


bool ServerConnector::registerSocket(sf::TcpSocket& writeInto)
{

	const char* loginString = "Vivat Sicher, Rock'n'Roll forever!!!\0\2";

	if (writeInto.send(loginString, 38) != sf::Socket::Status::Done) {
		return 0;
	}

	std::string serverAnswer;
	
	serverAnswer.resize(26); // size of response string + one byte of responsed type

	size_t receivedSize = 0;

	writeInto.receive(serverAnswer.data(), serverAnswer.size(), receivedSize);

	if (!receivedSize) {
		return 0;
	}

	return 1;
}

void ServerConnector::processMessage(VPacket& messagePacket)
{
	int senderID;
	std::string text;

	messagePacket.readAny(senderID);
	messagePacket.readString(text);

	std::string senderName = "???";

	std::cout << "='";

	for (PlayerInfo& player : playersList) {
		if (player.ID == senderID) {
			senderName = player.name;
			break;
		}
	}

	std::cout << senderName << "'=>'" << text << "'\n";
}

void ServerConnector::processPacketStack()
{
	inputPacketsStream.receive();

	for (int i = 0; i < inputPacketsStream.packetStack.size(); i++)
	{
		auto stackBegin = inputPacketsStream.packetStack.begin();
		VPacket& packetHere = inputPacketsStream.packetStack[i];

		// keepalive packet
		if (packetHere.empty()) {
			continue;
		}

		// process only auxiliary events
		if (muteAuxPackets || !(packetHere.type & AUXILIARY_EVENT)) {
			continue;
		}

		switch (packetHere.type) {
		case PLAYERS_NAME:
			playersList.emplace_back();
			
			packetHere.readAny(playersList.back().ID);
			packetHere.readString(playersList.back().name);
			break;
		case DIRECT_RECEIVING:
			processMessage(packetHere);
			break;
		}

		inputPacketsStream.packetStack.erase(stackBegin + i);
		i--;
	}
	/*
	if (keepalivePacketClock.getElapsedTime() > maxServerResponseTime && isConnected) {
		connectionError() << "Server did not responsed with keep-alive packets too long. Autodisconnect.\n";
		disconnect();
	}*/
}

void ServerConnector::sIOErrUP()
{
	serverIOError() << "Server responsed with uncomplete packet.\n";
}

std::ostream& ServerConnector::connectionError()
{
	std::cout << "Connection with server failed; ";
	return std::cout;
}

std::ostream& ServerConnector::serverIOError()
{
	std::cout << "Server I/O transfer failed; ";
	return std::cout;
}

bool ServerConnector::identificate()
{
	return registerSocket(mainSocket);
}
