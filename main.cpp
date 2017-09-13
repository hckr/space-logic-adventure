#include <memory>
#include <functional>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <SFML/Graphics.hpp>

#include "tileset.hpp"
#include "level.hpp"
#include "menuscreen.hpp"
#include "endscreen.hpp"
#include "soundmanager.hpp"
#include "creditsscreen.hpp"


auto createCenteredWindow(int width, int height) {
    auto window = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), "Space Logic Adventure", sf::Style::Titlebar | sf::Style::Close);
    int posX = (sf::VideoMode::getDesktopMode().width - window->getSize().x) / 2,
        posY = (sf::VideoMode::getDesktopMode().height - window->getSize().y) / 2;
    window->setPosition(sf::Vector2i(posX, posY));
    return window;
}

Level::TileAppearanceToSpriteInfoMap_t tilesSpriteInfo {
    { Level::TileAppearance::FIELD_VERTICAL, Tileset::metalTileConnectStraight_NE },
    { Level::TileAppearance::FIELD_VERTICAL_OPENED_TOP, Tileset::metalTileConnectEnd_SE },
    { Level::TileAppearance::FIELD_VERTICAL_OPENED_BOTTOM, Tileset::metalTileConnectEnd_NE },
    { Level::TileAppearance::FIELD_HORIZONTAL, Tileset::metalTileConnectStraight_NW },
    { Level::TileAppearance::FIELD_HORIZONTAL_OPENED_LEFT, Tileset::metalTileConnectEnd_NW },
    { Level::TileAppearance::FIELD_HORIZONTAL_OPENED_RIGHT, Tileset::metalTileConnectEnd_SW },
    { Level::TileAppearance::FIELD_UP_RIGHT_TURN, Tileset::metalTileConnectCornerInner_SE },
    { Level::TileAppearance::FIELD_LEFT_UP_TURN, Tileset::metalTileConnectCornerInner_NW },
    { Level::TileAppearance::FIELD_DOWN_RIGHT_TURN, Tileset::metalTileConnectCornerInner_SW },
    { Level::TileAppearance::FIELD_LEFT_DOWN_TURN, Tileset::metalTileConnectCornerInner_NE },
    { Level::TileAppearance::FIELD_CLOSED_TOP, Tileset::metalTileConnectSide_SW },
    { Level::TileAppearance::FIELD_CLOSED_RIGHT, Tileset::metalTileConnectSide_NE },
    { Level::TileAppearance::FIELD_CLOSED_BOTTOM, Tileset::metalTileConnectSide_NW },
    { Level::TileAppearance::FIELD_CLOSED_LEFT, Tileset::metalTileConnectSide_SE },
    { Level::TileAppearance::FIELD_OPENED_ALL_SIDES, Tileset::metalTileConnectCenter_NE },

    { Level::TileAppearance::FENCE_TOP_RIGHT, Tileset::metalFenceCorner_SW },
    { Level::TileAppearance::FENCE_BOTTOM_LEFT, Tileset::metalFenceCorner_NW },

    { Level::TileAppearance::PLAYER_FACED_TOP, Tileset::astronaut_NE },
    { Level::TileAppearance::PLAYER_FACED_BOTTOM, Tileset::astronaut_SW },
    { Level::TileAppearance::PLAYER_FACED_LEFT, Tileset::astronaut_NW },
    { Level::TileAppearance::PLAYER_FACED_RIGHT, Tileset::astronaut_SE },

    { Level::TileAppearance::FINISH_OVERLAY, Tileset::spaceCraft3_NE }
};

int main() {
    size_t winWidth = 860,
           winHeight = 700;
    auto window = createCenteredWindow(winWidth, winHeight);
    window->setFramerateLimit(60);
    window->setKeyRepeatEnabled(false);

    SoundManager& soundManager = SoundManager::getInstance();

    sf::Texture background_tile_tx;
    background_tile_tx.loadFromFile("assets/background.png");
    background_tile_tx.setSmooth(true);
    sf::Sprite background_tile_sp(background_tile_tx);
    sf::RenderTexture background_rtx;
    background_rtx.create(winWidth, winHeight);
    background_rtx.setSmooth(true);
    for (size_t posX = 0; posX < winWidth; posX += background_tile_tx.getSize().x) {
        for (size_t posY = 0; posY < winHeight; posY += background_tile_tx.getSize().y) {
            background_tile_sp.setPosition(posX, posY);
            background_rtx.draw(background_tile_sp);
        }
    }
    background_rtx.display();
    sf::Sprite background_sp(background_rtx.getTexture());

    sf::Texture menu_background_tx(background_rtx.getTexture());
    sf::Sprite menu_background_sp(menu_background_tx);
    menu_background_sp.setColor(sf::Color(200, 200, 200));

    sf::Texture tileset;
    tileset.loadFromFile("assets/tileset.png");
    tileset.setSmooth(true);

    sf::Font font;
    font.loadFromFile("assets/VT323-Regular.ttf");

    sf::Color lightBlue(50, 200, 255);
    sf::Color white(255, 255, 255);

    std::shared_ptr<Screen> currentScreen;
    auto menuScreen = std::make_shared<MenuScreen>(font, menu_background_sp, lightBlue, white);
    auto endScreen = std::make_shared<EndScreen>(font, background_sp, white, lightBlue);
    auto creditsScreen = std::make_shared<CreditsScreen>(font, background_sp, lightBlue, white);

    auto setCurrentScreen = [&](std::shared_ptr<Screen> newScreen) {
        currentScreen = newScreen;
        currentScreen->onActivated();
        if (newScreen == menuScreen || newScreen == creditsScreen) {
            soundManager.changeMusic(SoundManager::MENU);
        } else {
            soundManager.changeMusic(SoundManager::LEVEL);
        }
    };

    setCurrentScreen(menuScreen);

    struct LevelData {
        std::string fileName;
        float fieldLifetime;
        std::string code;
        std::string message;
    };

    std::vector<LevelData> levelsData;
    std::ifstream file("assets/levels/_list.txt");
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        LevelData data;
        std::getline(ss, data.fileName, ';');
        std::string fieldLifetimeStr;
        std::getline(ss, fieldLifetimeStr, ';');
        data.fieldLifetime = std::stof(fieldLifetimeStr);
        std::getline(ss, data.code, ';');
        std::getline(ss, data.message, ';');
        levelsData.push_back(data);
    }
    size_t currentLevelIndex = 0;

    sf::Vector2f targetSize(window->getSize().x, window->getSize().y);

    std::function<void(Event)> eventReceiver = [&](Event event) {
        switch (event.type) {
        case Event::MENU_START_NEW_GAME:
            currentLevelIndex = -1;
            // fallthrough
        case Event::LEVEL_FINISHED:
            currentLevelIndex += 1;
            // fallthrough
        case Event::MENU_TRY_AGAIN:
            if (currentLevelIndex < levelsData.size()) {
                const LevelData &levelData = levelsData[currentLevelIndex];
                auto levelScreen = std::make_shared<Level>(levelData.fileName, levelData.fieldLifetime, levelData.code, levelData.message, tileset, tilesSpriteInfo, font, background_sp, white, lightBlue, targetSize);
                levelScreen->setEventReceiver(eventReceiver);
                setCurrentScreen(levelScreen);
            } else {
                setCurrentScreen(endScreen);
            }
            break;
        case Event::SHOW_MENU_WITH_TRY_AGAIN:
            menu_background_tx.create(window->getSize().x, window->getSize().y);
            menu_background_tx.update(*window);
            menuScreen->enableTryAgain(true);
            menuScreen->setActiveOption(MenuScreen::TRY_AGAIN);
            setCurrentScreen(menuScreen);
            break;
        case Event::SHOW_CLEAN_MENU:
            menu_background_tx = background_rtx.getTexture();
            menuScreen->enableTryAgain(false);
            menuScreen->setActiveOption(MenuScreen::START_NEW_GAME);
            setCurrentScreen(menuScreen);
            break;
        case Event::SHOW_MENU:
            setCurrentScreen(menuScreen);
            break;
        case Event::MENU_LEVEL_CODE:
        {
            auto levelDataIt = std::find_if(std::begin(levelsData), std::end(levelsData), [&](auto levelData) {
                return levelData.code == reinterpret_cast<char *>(event.data);
            });
            if (levelDataIt != levelsData.end()) {
                currentLevelIndex = levelDataIt - levelsData.begin();
                auto levelData = *levelDataIt;
                auto levelScreen = std::make_shared<Level>(levelData.fileName, levelData.fieldLifetime, levelData.code, levelData.message, tileset, tilesSpriteInfo, font, background_sp, white, lightBlue, targetSize);
                levelScreen->setEventReceiver(eventReceiver);
                setCurrentScreen(levelScreen);
            } else {
                menuScreen->clearLevelCode();
            }
            break;
        }
        case Event::MENU_CREDITS:
            setCurrentScreen(creditsScreen);
            break;
        case Event::MENU_QUIT:
            SoundManager::destroyInstance();
            window->close();
            break;
        }
    };

    menuScreen->setEventReceiver(eventReceiver);
    endScreen->setEventReceiver(eventReceiver);
    creditsScreen->setEventReceiver(eventReceiver);

    while (window->isOpen()) {

        soundManager.update();

        sf::Event event;
        while (window->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                SoundManager::destroyInstance();
                window->close();
                break;
            default:
                currentScreen->processEvent(event);
            }
        }

        currentScreen->update();

        window->draw(*currentScreen);
        window->display();
    }

    return 0;
}
