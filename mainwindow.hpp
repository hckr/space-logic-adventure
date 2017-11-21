#pragma once

#include "level.hpp"
#include "eventreceiver.hpp"
#include "menuscreen.hpp"
#include "endscreen.hpp"
#include "soundmanager.hpp"
#include "creditsscreen.hpp"

class MainWindow : EventReceiver{

    struct LevelData {
        std::string fileName;
        float fieldLifetime;
        std::string code;
        std::string message;
    };

public:
    MainWindow(int width, int height, std::string title);
    void eventReceiver(Event event);

private:
    sf::RenderWindow window;
    Level::TileAppearanceToSpriteInfoMap_t tilesSpriteInfo;
    SoundManager& soundManager;
    sf::Texture background_tx;
    sf::Sprite background_sp;
    sf::Texture menu_background_tx;
    sf::Sprite menu_background_sp;
    sf::Texture tileset;
    sf::Font font;
    sf::Color lightBlue;
    sf::Color white;
    Screen *currentScreen;
    MenuScreen *menuScreen;
    EndScreen *endScreen;
    CreditsScreen *creditsScreen;
    std::vector<LevelData> levelsData;
    size_t currentLevelIndex;
    sf::Vector2f targetSize;

    void setCurrentScreen(Screen* newScreen);
    void loadLevelsData();
    void initTilesSpriteInfo();
};
