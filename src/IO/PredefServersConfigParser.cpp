#include "PredefServersConfigParser.h"

#include "OOConfig.h"

#include <iostream>

void PredefServersConfigParser::parse(std::vector<PredefServerInfo>& outputBuffer)
{
	OOConfig config;

	fresult loadResult = config.load(configName);
	if(loadResult != fFile::CODE_SUCCESS){
		error() << "Config loading failed with code '"<<fFile::strCode(loadResult)<<'\n';
		return;
	}

	config.parse();

	for(OOConfig::Object& object : config.outputArray){
		PredefServerInfo& server = outputBuffer.emplace_back();

		server.name = object.getStringVal("name");
		server.port = object.getIntVal("port");
		server.engine = object.getStringVal("engine");
		server.host = object.getStringVal("host");
		server.description = object.getStringVal("description");
		
		if(object.hasFlag("deprecated")){
			server.flags |= PDSF_Deprecated;
		}
	}
}

std::ostream& PredefServersConfigParser::error()
{
	return std::cout << "PDSCP Error: ";
}

