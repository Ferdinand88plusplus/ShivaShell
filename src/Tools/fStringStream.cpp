#include "fStringStream.h"

#define CUR2END_CYCLE() (currentPos; currentPos < myData.size(); currentPos++)

bool fStringStream::tryReadWord(std::string& output)
{
	output.clear();

	skipSpaces();

	readWord(output);

	return output.size();
}

bool fStringStream::tryReadString(std::string& output)
{
	output.clear();

	skipSpaces();

	// string is MUST start by "
	if (myData[currentPos] != '"') {
		return 0;
	}

	readString(output);

	return output.size();
}

bool fStringStream::tryReadAny(std::string& output)
{
	output.clear();

	skipSpaces();

	if (myData[currentPos] == '"') {
		readString(output);
	}
	else {
		readWord(output);
	}

	return output.size();
}

void fStringStream::skipSpaces()
{
	char c;
	for CUR2END_CYCLE() {
		c = myData[currentPos];

		if (!iswspace(c)) {
			break;
		}
	}
}

void fStringStream::readWord(std::string& output)
{
	char c;
	for CUR2END_CYCLE() {
		c = myData[currentPos];

		if (iswspace(c)) {
			break;
		}

		output += c;
	}
}

void fStringStream::readString(std::string& output)
{
	char c;
	currentPos++;

	for CUR2END_CYCLE() {
		c = myData[currentPos];

		if (c == '"') {
			currentPos++;
			break;
		}

		output += c;
	}
}
