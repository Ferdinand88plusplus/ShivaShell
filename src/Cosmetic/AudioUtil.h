#pragma once

#include <SFML/Audio.hpp>
#include <thread>

enum TRACK_ID {
	TID_Calm,
	TID_NormalAttack,
	TID_DprAttackIntro,
	TID_DprAttackMain,
};

class AudioUtil
{
public:
	
	void init();

	void set(TRACK_ID track);

	sf::Sound stream;

private:

	void loadThemes();
	void subLoadTheme(const std::string& fname, sf::SoundBuffer& buffer);

	sf::SoundBuffer calmTheme;
	sf::SoundBuffer normalAttackTheme;
	sf::SoundBuffer dprAttackIntroTheme;
	sf::SoundBuffer dprAttackMainTheme;
};

extern AudioUtil audioUtil;

