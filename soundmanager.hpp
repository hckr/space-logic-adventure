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

    std::map<Sound, sf::Sound> sounds = {
        { MENU, sf::Sound(menuSB) },
        { LEVEL, sf::Sound(levelSB) },
        { LEVEL_COMPLETED, sf::Sound(levelCompletedSB) },
        { GAME_OVER, sf::Sound(gameOverSB) }
    };

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
