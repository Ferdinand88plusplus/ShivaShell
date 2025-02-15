#include "ServerSocket.h"

#include "InputVPacketManager.h"

#include "VPacket.h"

bool ServerSocket::connect(const ServerInfo &to, bool printLogs)
{
    if(connectedServer){
        disconnect();
    }

    if(socket.connect(to.IP, to.Port) != sf::Socket::Status::Done){
        return false;
    }

    if(!identificate()) {
        return false;
    }

    connectedServer = new ServerInfo(to);

    return true;
}

void ServerSocket::disconnect()
{
    VPacket closeSocketPacket(CLOSE_SOCKET);
    closeSocketPacket.sendToServer();

    socket.disconnect();

    if(connectedServer){
        delete connectedServer;
        connectedServer = 0;
    }
}

void ServerSocket::updateGamesList()
{
    if(!connectedServer){
        return;
    }

    VPacket getGamesQuery(GAMES_LIST_QUERY);
    getGamesQuery.sendToServer();

    VPacket gamesListResponse;
    if(!inputPacketsStream.waitPacket(GAMES_LIST_RESPONSE, gamesListResponse))
    {
        errNoServResponse();
        return;
    }  

    unsigned char numGames;
    gamesListResponse.readAny(numGames);

    serverGames.clear();

    for(int i = 0; i < numGames; i++)
    {
        GameInfo& game = serverGames.emplace_back();

        gamesListResponse.readAny(game.ID);
        gamesListResponse.readString(game.name);
    }
}

void ServerSocket::updatePlayersList()
{
    if(!connectedGame){
        return;
    }

    VPacket getPlayersQuery(TOTAL_PLAYERS_DATA_QUERY);
    getPlayersQuery.sendToServer();

    VPacket playersListResponse;
    if(!inputPacketsStream.waitPacket(TOTAL_LIST_OF_PLAYERS_DATA, playersListResponse))
    {
        errNoServResponse();
        return;
    }

    unsigned char numPlayers;
    playersListResponse.readAny(numPlayers);
    
    gamePlayers.clear();

    for(int i = 0; i < numPlayers; i++)
    {
        PlayerInfo& player = gamePlayers.emplace_back();

        playersListResponse.readAny(player.ID);
        playersListResponse.readAny(player.status);
		playersListResponse.readAny(player.worldID);
		playersListResponse.readAny(player.X);
		playersListResponse.readAny(player.Y);
		playersListResponse.readString(player.name);
		playersListResponse.readAny(player.body);
    }
}

bool ServerSocket::findGameInfo(int gameID, GameInfo &writeInto)
{
    updateGamesList();

    for(GameInfo& game : serverGames){
        std::cout<<game.ID<<'|'<<gameID<<'\n';
        if(game.ID == gameID) {
            writeInto = game;
            return true;
        }
    }

    return false;
}

bool ServerSocket::connect2Game(int gameID)
{
    if(!connectedServer){
        return false;
    }

    VPacket attach2GameQuery(ATTACH_TO_GAME);
    attach2GameQuery.writeAny(gameID);
    attach2GameQuery.sendToServer();

    VPacket attach2GameResponse;
    if(!inputPacketsStream.waitPacket(ATTACH_TO_GAME_RESPONSE, attach2GameResponse))
    {
        errNoServResponse();
        return false;
    }

    int resolvedGameID;
    bool isConfiguredGame;
    int birthTime;
    
    attach2GameResponse.readAny(resolvedGameID);
    attach2GameResponse.readAny(isConfiguredGame);
    attach2GameResponse.readAny(birthTime);
    attach2GameResponse.readAny(myPlayerID);

    GameInfo resolvedGameInfo;
    if(!findGameInfo(resolvedGameID, resolvedGameInfo)){
        error() << "Couldnt find resolved game info\n";
        return false;
    }

    connectedGame = new GameInfo(resolvedGameInfo);

    return true;
}

void ServerSocket::processPacketStack()
{
    for (int i = 0; i < inputPacketsStream.packetStack.size(); i++)
    {
        inputPacketsStream.receive();

        VPacket& packet = inputPacketsStream.packetStack.front();

        if(!(packet.type & AUXILIARY_EVENT))
        {
            continue;
        }

        if(ignoreAuxiliaries)
        {
            goto finishPacket;
        }

        switch(packet.type)
        {
        case PLAYERS_NAME:
            gamePlayers.emplace_back();

            packet.readAny(gamePlayers.back().ID);
            packet.readString(gamePlayers.back().name);
            break;
        }

finishPacket:
        inputPacketsStream.packetStack.erase(inputPacketsStream.packetStack.begin());
        i--;
    }
}

bool ServerSocket::identificate()
{
    static const std::string loginString = std::string("Vivat Sicher, Rock'n'Roll forever!!!");
    static constexpr unsigned char loginVersion = 2;

	if (socket.send(loginString.data(), loginString.size()+1) != sf::Socket::Status::Done) {
		return 0;
	}

    if(socket.send(&loginVersion, 1) != sf::Socket::Status::Done){
        return 0;
    }

	std::string serverAnswer;
    unsigned char serverVersion;
	
	serverAnswer.resize(26); // size of response string + serverVersion

	size_t receivedSize = 0;

	socket.receive(serverAnswer.data(), serverAnswer.size(), receivedSize);

	if (!receivedSize)
		return 0;

    // fenex.vangers.net and cloud 
    /*
    serverVersion = serverAnswer.back();

    if(serverVersion != loginVersion)
    {
        std::cout<<"WARNING: Server has different protocol version (my="<<(int)loginVersion<<";server="<<(int)serverVersion<<")\n";
    }
    */

	return receivedSize;
}

void ServerSocket::errNoServResponse()
{
    error() << "Server did not responsed\n";
}

std::ostream &ServerSocket::error()
{
    return std::cout << "Transfer failed: ";
}

ServerSocket serverSocket;