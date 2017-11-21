#include "mainwindow.hpp"

#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <SFML/Graphics.hpp>

#include "tileset.hpp"

MainWindow::MainWindow(int width, int height, std::string title)
:
    window(sf::VideoMode(width, height), title, sf::Style::Titlebar | sf::Style::Close),
    soundManager(SoundManager::getInstance()),
    background_sp(background_tx),
    menu_background_tx(background_tx),
    menu_background_sp(menu_background_tx),
    lightBlue(50, 200, 255),
    white(255, 255, 255),
    currentLevelIndex(0),
    targetSize(window.getSize().x, window.getSize().y)
{
    int posX = (sf::VideoMode::getDesktopMode().width - window.getSize().x) / 2,
        posY = (sf::VideoMode::getDesktopMode().height - window.getSize().y) / 2;
    window.setPosition(sf::Vector2i(posX, posY));
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

    initTilesSpriteInfo();

    background_tx.loadFromFile("assets/background.png");
    background_tx.setRepeated(true);
    background_tx.setSmooth(true);
    background_sp.setTextureRect({ 0, 0, static_cast<int>(window.getSize().x), static_cast<int>(window.getSize().y) });

    menu_background_sp.setTextureRect({ 0, 0, static_cast<int>(window.getSize().x), static_cast<int>(window.getSize().y) });
    menu_background_sp.setColor(sf::Color(200, 200, 200));

    tileset.loadFromFile("assets/tileset.png");
    tileset.setSmooth(true);

    font.loadFromFile("assets/VT323-Regular.ttf");

    menuScreen = new MenuScreen(font, menu_background_sp, lightBlue, white);
    endScreen = new EndScreen(font, background_sp, white, lightBlue);
    creditsScreen = new CreditsScreen(font, background_sp, lightBlue, white);

    menuScreen->setEventReceiver(this);
    endScreen->setEventReceiver(this);
    creditsScreen->setEventReceiver(this);

    setCurrentScreen(menuScreen);

    while (window.isOpen()) {

        soundManager.update();

        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                SoundManager::destroyInstance();
                window.close();
                break;
            default:
                currentScreen->processEvent(event);
            }
        }

        currentScreen->update();

        window.draw(*currentScreen);
        window.display();
    }
}

void MainWindow::setCurrentScreen(Screen* newScreen) {
    currentScreen = newScreen;
    currentScreen->onActivated();
    if (newScreen == menuScreen || newScreen == creditsScreen) {
        soundManager.changeMusic(SoundManager::MENU);
    } else {
        soundManager.changeMusic(SoundManager::LEVEL);
    }
}

void MainWindow::loadLevelsData() {
    std::ifstream file("assets/levels/_list.txt");
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        LevelData data;
        std::getline(ss, data.fileName, ';');
        std::string fieldLifetimeStr;
        std::getline(ss, fieldLifetimeStr, ';');
        data.fieldLifetime = atoi(fieldLifetimeStr.c_str());
        std::getline(ss, data.code, ';');
        std::getline(ss, data.message, ';');
        levelsData.push_back(data);
    }
}

void MainWindow::initTilesSpriteInfo() {
    tilesSpriteInfo[Level::TileAppearance::FIELD_VERTICAL] = Tileset::metalTileConnectStraight_NE;
    tilesSpriteInfo[Level::TileAppearance::FIELD_VERTICAL_OPENED_TOP] = Tileset::metalTileConnectEnd_SE;
    tilesSpriteInfo[Level::TileAppearance::FIELD_VERTICAL_OPENED_BOTTOM] = Tileset::metalTileConnectEnd_NE;
    tilesSpriteInfo[Level::TileAppearance::FIELD_HORIZONTAL] = Tileset::metalTileConnectStraight_NW;
    tilesSpriteInfo[Level::TileAppearance::FIELD_HORIZONTAL_OPENED_LEFT] = Tileset::metalTileConnectEnd_NW;
    tilesSpriteInfo[Level::TileAppearance::FIELD_HORIZONTAL_OPENED_RIGHT] = Tileset::metalTileConnectEnd_SW;
    tilesSpriteInfo[Level::TileAppearance::FIELD_UP_RIGHT_TURN] = Tileset::metalTileConnectCornerInner_SE;
    tilesSpriteInfo[Level::TileAppearance::FIELD_LEFT_UP_TURN] = Tileset::metalTileConnectCornerInner_NW;
    tilesSpriteInfo[Level::TileAppearance::FIELD_DOWN_RIGHT_TURN] = Tileset::metalTileConnectCornerInner_SW;
    tilesSpriteInfo[Level::TileAppearance::FIELD_LEFT_DOWN_TURN] = Tileset::metalTileConnectCornerInner_NE;
    tilesSpriteInfo[Level::TileAppearance::FIELD_CLOSED_TOP] = Tileset::metalTileConnectSide_SW;
    tilesSpriteInfo[Level::TileAppearance::FIELD_CLOSED_RIGHT] = Tileset::metalTileConnectSide_NE;
    tilesSpriteInfo[Level::TileAppearance::FIELD_CLOSED_BOTTOM] = Tileset::metalTileConnectSide_NW;
    tilesSpriteInfo[Level::TileAppearance::FIELD_CLOSED_LEFT] = Tileset::metalTileConnectSide_SE;
    tilesSpriteInfo[Level::TileAppearance::FIELD_OPENED_ALL_SIDES] = Tileset::metalTileConnectCenter_NE;

    tilesSpriteInfo[Level::TileAppearance::FENCE_TOP_RIGHT] = Tileset::metalFenceCorner_SW;
    tilesSpriteInfo[Level::TileAppearance::FENCE_BOTTOM_LEFT] = Tileset::metalFenceCorner_NW;

    tilesSpriteInfo[Level::TileAppearance::PLAYER_FACED_TOP] = Tileset::astronaut_NE;
    tilesSpriteInfo[Level::TileAppearance::PLAYER_FACED_BOTTOM] = Tileset::astronaut_SW;
    tilesSpriteInfo[Level::TileAppearance::PLAYER_FACED_LEFT] = Tileset::astronaut_NW;
    tilesSpriteInfo[Level::TileAppearance::PLAYER_FACED_RIGHT] = Tileset::astronaut_SE;

    tilesSpriteInfo[Level::TileAppearance::FINISH_OVERLAY] = Tileset::spaceCraft3_NE;
}

void MainWindow::eventReceiver(Event event) {
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
            auto levelScreen = new Level(levelData.fileName, levelData.fieldLifetime, levelData.code, levelData.message, tileset, tilesSpriteInfo, font, background_sp, white, lightBlue, targetSize);
            levelScreen->setEventReceiver(this);
            setCurrentScreen(levelScreen);
        } else {
            setCurrentScreen(endScreen);
        }
        break;
    case Event::SHOW_MENU_WITH_TRY_AGAIN:
        menu_background_tx.create(window.getSize().x, window.getSize().y);
        menu_background_tx.update(window);
        menuScreen->enableTryAgain(true);
        menuScreen->setActiveOption(MenuScreen::TRY_AGAIN);
        setCurrentScreen(menuScreen);
        break;
    case Event::SHOW_CLEAN_MENU:
        menu_background_tx = background_tx;
        menuScreen->enableTryAgain(false);
        menuScreen->setActiveOption(MenuScreen::START_NEW_GAME);
        setCurrentScreen(menuScreen);
        break;
    case Event::SHOW_MENU:
        setCurrentScreen(menuScreen);
        break;
    case Event::MENU_LEVEL_CODE:
    {
        auto levelDataIt = std::find_if(levelsData.begin(), levelsData.end(), [&](auto levelData) {
            return levelData.code == reinterpret_cast<char *>(event.data);
        });
        if (levelDataIt != levelsData.end()) {
            currentLevelIndex = levelDataIt - levelsData.begin();
            auto levelData = *levelDataIt;
            auto levelScreen = new Level(levelData.fileName, levelData.fieldLifetime, levelData.code, levelData.message, tileset, tilesSpriteInfo, font, background_sp, white, lightBlue, targetSize);
            levelScreen->setEventReceiver(this);
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
        window.close();
        break;
    }
}
