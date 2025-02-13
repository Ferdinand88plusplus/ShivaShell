#include "AudioUtil.h"

#include "../IO/OOConfig.h"

#include <iostream>

int numLoadingThreads = 0;
int numStartedThreads = 0;
sf::Time threadCycleSleepDelay = sf::seconds(0.1f);

void audioLoadingThread( std::string fname, sf::SoundBuffer* readInto) {
	readInto->loadFromFile(fname);
	numLoadingThreads--;
}

void AudioUtil::init()
{
	loadThemes();

	stream.setLoop(1);
	stream.setVolume(50.0f);

	set(TID_Calm);
}

void AudioUtil::set(TRACK_ID track)
{
	switch (track)
	{
	case TID_Calm:
		stream.setBuffer(calmTheme);
		break;
	case TID_NormalAttack:
		stream.setBuffer(normalAttackTheme);
		break;
	case TID_DprAttackIntro:
		stream.setBuffer(dprAttackIntroTheme);
		break;
	case TID_DprAttackMain:
		stream.setBuffer(dprAttackMainTheme);
		break;
	default:
		break;
	}

	stream.play();
}

#define LOADTHEMENAME(theme) subLoadTheme(musicNames.getStringVal(#theme, "ERRNO FLAG"), theme)

void AudioUtil::loadThemes()
{
	OOConfig config;

	fresult result = config.load("data/audioUtils.txt");
	if(result != fFile::CODE_SUCCESS){
		std::cout<<"Failed to load audioUtils config: "<<fFile::strCode(result)<<'\n';
		return;
	}

	config.parse();

	OOConfig::Object musicNames;

	if(!config.findObject("musicNames", musicNames)){
		std::cout<<"No music files configuration object!\n";
		return; 
	}

	LOADTHEMENAME(calmTheme);
	LOADTHEMENAME(normalAttackTheme);
	LOADTHEMENAME(dprAttackIntroTheme);
	LOADTHEMENAME(dprAttackMainTheme);

	std::cout << "Loading audio data...\n";

	while (1) {
		if (!numLoadingThreads) {
			break;
		}

		
		std::cout << "[" << numStartedThreads - numLoadingThreads << '/' << numStartedThreads << ']' << '\r';
		sf::sleep(threadCycleSleepDelay);
	}
}

void AudioUtil::subLoadTheme(const std::string& fname, sf::SoundBuffer& buffer)
{
	numLoadingThreads++;
	numStartedThreads++;

	std::string totalFname = "data/snd/" + fname;

	std::thread(audioLoadingThread, totalFname, &buffer).detach();
}

AudioUtil audioUtil;