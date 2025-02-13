#include "InputVPacketManager.h"

#include "MainSocket.h"

void InputVPacketManager::receive()
{
	if (!currentPacket) {
		currentPacket = &packetStack.emplace_back();
	}

	mainSocket.setBlocking(0);
	if (currentPacket->readFromServerUsingSize()) {
		// erase empty packets (aka keepalive packets)
		if (currentPacket->empty()) {
			packetStack.pop_back();
		}
		currentPacket = 0;
	}
	mainSocket.setBlocking(1);
}

void InputVPacketManager::init()
{
}

bool InputVPacketManager::takePacket(vph_t typeFilter, VPacket& writeBuffer)
{
	for (int i = 0; i < packetStack.size(); i++) {
		if (packetStack[i].type != typeFilter)
			continue;

		void* ERASE_POINTER = &packetStack[i];

		size_t sz = packetStack.size();

		bool isInRange = sz > i && i >= 0;

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
		if (takePacket(typeFilter, writeBuffer)) {
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