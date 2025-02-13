#pragma once

#include <SFML/Network.hpp>

typedef unsigned short port_t;

struct ServerInfo
{
	std::string IP;
	port_t Port = 0;

	ServerInfo(const std::string& ip, port_t port) {
		IP = ip;
		Port = port;
	}
	ServerInfo() {};
};

