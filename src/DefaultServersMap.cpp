#include "DefaultServersMap.h"

void DefaultServersMap::load()
{
    OOConfig config;

	fresult loadResult = config.load("data/defaultServers.txt");
	if(loadResult != fFile::CODE_SUCCESS){
		std::cout << "DefaultServersMap error: Config loading failed with code '"<<fFile::strCode(loadResult)<<'\n';
		return;
	}

	config.parse();

	for(OOConfig::Object& object : config.outputArray){
		DefaultServerInfo& server = servers.emplace_back();

		server.name = object.getStringVal("name");
		server.port = object.getIntVal("port");
		server.engine = object.getStringVal("engine");
		server.host = object.getStringVal("host");
		server.description = object.getStringVal("description");
		
		if(object.hasFlag("deprecated")){
			server.flags |= DSF_Deprecated;
		}
	}
}

void DefaultServersMap::print()
{
	ServerInfo connectInfo;
    sf::Clock pingClock;
    
	serverSocket.disconnect();

	std::cout << "DEFAULT SERVERS MAP\n";

	for (DefaultServerInfo& server : servers) {

		connectInfo.IP = server.name;
		connectInfo.Port = server.port;

		if (server.flags & DSF_Deprecated)
			std::cout << '\t' << "[DEPRECATED]\n";

		std::cout
			<< '\t' << "name: " << server.name << ':' << std::to_string(server.port) << '\n'
			<< '\t' << "engine: " << server.engine << '\n'
			<< '\t' << "host: " << server.host << '\n'
			<< '\t' << "description: '" << server.description << "'\n";

		pingClock.restart();

		bool state = serverSocket.connect(connectInfo, 0);

		std::cout
			<< '\t' << "connection state: " << (state ? '+' : '-') << '\n'
			<< '\t' << "ping: " << pingClock.restart().asSeconds()<<'s' << '\n';

		std::cout << '\n';
	}
}
