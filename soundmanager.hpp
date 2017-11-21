#pragma once

#include <map>
#include <string>

#include <SFML/Audio.hpp>

const std::string SOUNDS_DIR = "assets/sounds/";


class SoundManager
{
public:

    enum Sound {
        NONE,

        // music:
        MENU,
        LEVEL,

        // effects:
        LEVEL_COMPLETED,
        GAME_OVER
    };

private:

    sf::SoundBuffer menuSB;
    sf::SoundBuffer levelSB;
    sf::SoundBuffer levelCompletedSB;
    sf::SoundBuffer gameOverSB;

    std::map<Sound, sf::Sound> sounds;

    Sound effectPlaying = NONE;
    Sound musicPlaying = NONE;
    static SoundManager *instance;

    void lowerMusicVolume(bool lower = true);
    SoundManager();

public:

    static SoundManager& getInstance();

    // https://github.com/SFML/SFML/issues/970
    static void destroyInstance();

    void changeMusic(Sound music);
    void playEffect(Sound effect);
    void update();
};
