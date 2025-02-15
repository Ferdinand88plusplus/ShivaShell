#pragma once

#include <string>

// Put raw data into ::myData;
// tryRead* functions is returns true if parsed succesful, or false if parsing failed;
class fStringStream
{
public:

	std::string myData;
	size_t currentPos = 0;

	void resetString(const std::string& source);

	// reads the word splitted by spaces
	bool tryReadWord(std::string& output);

	// reads the string splitted by "
	bool tryReadString(std::string& output);

	// reads the word if didnt found ", otherwise - reads string.
	bool tryReadAny(std::string& output);

	// move currentpos until find something other than space
	void skipSpaces();

private:

	void readWord(std::string& output);
	void readString(std::string& output);

};

