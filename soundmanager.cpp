#include "soundmanager.hpp"

void SoundManager::lowerMusicVolume(bool lower) {
    if (lower) {
        sounds[MENU].setVolume(50);
        sounds[LEVEL].setVolume(50);
    } else {
        sounds[MENU].setVolume(100);
        sounds[LEVEL].setVolume(100);
    }
}

SoundManager::SoundManager()
{
    menuSB.loadFromFile(SOUNDS_DIR + "menu.ogg");
    levelSB.loadFromFile(SOUNDS_DIR + "level.ogg");
    levelCompletedSB.loadFromFile(SOUNDS_DIR + "level_completed.ogg");
    gameOverSB.loadFromFile(SOUNDS_DIR + "game_over.ogg");

    sounds[MENU] = sf::Sound(menuSB);
    sounds[LEVEL] = sf::Sound(levelSB);
    sounds[LEVEL_COMPLETED] = sf::Sound(levelCompletedSB);
    sounds[GAME_OVER] = sf::Sound(gameOverSB);

    sounds[MENU].setLoop(true);
    sounds[LEVEL].setLoop(true);
}

SoundManager* SoundManager::instance = 0;

SoundManager& SoundManager::getInstance() {
    if (!instance) {
        instance = new SoundManager();
    }
    return *instance;
}

void SoundManager::destroyInstance() {
    if (instance) {
        delete instance;
        instance = 0;
    }
}

void SoundManager::changeMusic(Sound music) {
    if (musicPlaying == music) {
        return;
    }

    switch (music) {
    case NONE:
        sounds[musicPlaying].stop();
        break;

    case MENU:
    case LEVEL:
        if (musicPlaying != NONE) {
            sounds[musicPlaying].stop();
        }
        sounds[music].play();
        musicPlaying = music;
        break;

    default:
        break;
    }
}

void SoundManager::playEffect(Sound effect) {
    switch (effect) {
    case LEVEL_COMPLETED:
    case GAME_OVER:
        lowerMusicVolume(true);
        sounds[effect].play();
        effectPlaying = effect;
        break;

    default:
        break;
    }
}

void SoundManager::update() {
    if (effectPlaying != NONE && sounds[effectPlaying].getStatus() == sf::SoundSource::Stopped) {
        lowerMusicVolume(false);
        effectPlaying = NONE;
    }
}
