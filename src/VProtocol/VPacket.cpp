#include "VPacket.h"

#include "ServerSocket.h"

sf::Time vpMaxReceiveTime = sf::seconds(5.0f);

bool VPacket::readFromServer()
{
	vpsz_t sz = 0;

	if (!subReceive(&sz, sizeof(vpsz_t))) {
		return 0;
	}

	if (sz == 0) { 
		return 1; 
	}

	resize(sz);

	subReceive(data(), sz);

	readAny(type);

	return 1;
}

void VPacket::sendToServer()
{
	vpsz_t sz = size() + sizeof(vph_t);
	sf::Socket::Status status;
	status = serverSocket.socket.send(&sz, sizeof(vpsz_t));
	status = serverSocket.socket.send(&type, sizeof(vph_t));
	if (size()) {
		serverSocket.socket.send(data(), size());
	}
}

bool VPacket::readString(std::string& into)
{
	vpsz_t endOfString = readOff;
	for (vpsz_t i = readOff; i < size(); i++) {
		if (!at(i)) {
			endOfString = i+1;
			break;
		}
	}

	vpsz_t stringLen = endOfString - readOff;

	if (!stringLen) {
		ioError() << "Requested string is too small or does not even exists in the packet.\n";
		return 0;
	}

	into.assign(data() + readOff, stringLen - 1);

	readOff += stringLen;

	return 1;
}

bool VPacket::subReceive(void* buffer, size_t bufsize)
{
	size_t receivedSize;
	sf::Socket::Status receiveStatus = serverSocket.socket.receive(buffer, bufsize, receivedSize);
	
	switch (receiveStatus) {
	default:
		return receivedSize == bufsize;
	case sf::Socket::Status::Error:
	case sf::Socket::Status::Disconnected:
		return 0;
	}
}
