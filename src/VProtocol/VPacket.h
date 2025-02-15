#pragma once

#include <SFML/Network.hpp>
#include <iostream>

#include "VPI.h"

// a packet that is vangers network using.
class VPacket : public std::string
{
public:

	vph_t type = 0;
	vpsz_t readOff = 0;

	/// SERVER IO MANIPULATIONS ///

	bool readFromServer();
	void sendToServer();

	template<typename read_t>
	bool readAny(read_t& into) {
		vpsz_t readSize = sizeof(read_t);
		if (readOff + readSize > size()) {
			ioError() << "Not enough data in packet to read <any> value.\n";
			return 0;
		}

		memcpy((void*)(&into), data() + readOff, readSize);
		readOff += readSize;
		return 1;
	}

	template<typename write_t>
	void writeAny(const write_t& from) {
		vpsz_t writeSize = sizeof(write_t);

		std::string writeData;
		writeData.assign((const char*)&from, writeSize);

		*this += writeData;
	}

	bool readString(std::string& into);

	/// CONSTRUCTORS & OPERATORS ///

	// Create empty packet
	VPacket() {};

	// Create packet with type
	VPacket(vph_t packetType) {
		type = packetType;
	}

	
private:

	bool subReceive(void* buffer, size_t bufsize);

	std::ostream& ioError() {
		std::cout << "I/O Error: ";
		return std::cout;
	}
};

