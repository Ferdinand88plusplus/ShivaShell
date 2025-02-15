#include "InputVPacketManager.h"

#include "ServerSocket.h"

void InputVPacketManager::receive()
{
	if (!currentPacket) {
		currentPacket = &packetStack.emplace_back();
	}

	serverSocket.socket.setBlocking(0);
	if (currentPacket->readFromServer()) {
		// erase empty packets
		if (currentPacket->empty()) {
			packetStack.pop_back();
		}
		currentPacket = 0;
	}
	serverSocket.socket.setBlocking(1);
}

bool InputVPacketManager::findPacket(vph_t typeFilter, VPacket& writeBuffer)
{
	for (int i = 0; i < packetStack.size(); i++) {
		if (packetStack[i].type != typeFilter)
			continue;

		writeBuffer = packetStack[i];
		packetStack.erase(packetStack.begin() + i);

		return true;
	}
	return false;
}

bool InputVPacketManager::waitPacket(vph_t typeFilter, VPacket& writeBuffer)
{
	static sf::Time waitingMaxTime = sf::seconds(5.0f);
	static sf::Clock waitingClock;

	waitingClock.restart();

	while (1) {
		if (findPacket(typeFilter, writeBuffer)) {
			return 1;
		}

		receive();

		if (waitingClock.getElapsedTime() > waitingMaxTime) {
			return 0;
		}
	}

	return false;
}

InputVPacketManager inputPacketsStream;