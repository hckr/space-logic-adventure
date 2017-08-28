#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>

#include "tileset.hpp"
#include "level.hpp"
#include "menuscreen.hpp"


auto createCenteredWindow(int width, int height) {
    auto window = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), "Space Logic Adventure", sf::Style::Titlebar | sf::Style::Close);
    int posX = (sf::VideoMode::getDesktopMode().width - window->getSize().x) / 2,
        posY = (sf::VideoMode::getDesktopMode().height - window->getSize().y) / 2;
    window->setPosition(sf::Vector2i(posX, posY));
    return window;
}

std::unique_ptr<sf::RenderWindow> window;

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
    { Level::TileAppearance::PLAYER_FACED_TOP, Tileset::astronaut_NE },
    { Level::TileAppearance::PLAYER_FACED_BOTTOM, Tileset::astronaut_SW },
    { Level::TileAppearance::PLAYER_FACED_LEFT, Tileset::astronaut_NW },
    { Level::TileAppearance::PLAYER_FACED_RIGHT, Tileset::astronaut_SE },
    { Level::TileAppearance::FINISH_OVERLAY, Tileset::spaceCraft3_NE }
};

int main() {
    /*auto */window = createCenteredWindow(860, 700);
    
    sf::Texture background_tx;
    background_tx.loadFromFile("assets/background.png");
    background_tx.setRepeated(true);
    background_tx.setSmooth(true);
    sf::Sprite background_sp(background_tx);
    background_sp.setTextureRect({ 0, 0, static_cast<int>(window->getSize().x), static_cast<int>(window->getSize().y) });

    sf::Texture menu_background_tx(background_tx);
    sf::Sprite menu_background_sp(menu_background_tx);
    menu_background_sp.setTextureRect({ 0, 0, static_cast<int>(window->getSize().x), static_cast<int>(window->getSize().y) });
    menu_background_sp.setColor(sf::Color(150, 150, 150));

    sf::Font font;
    font.loadFromFile("assets/VT323-Regular.ttf");

    sf::Color menu_color(50, 200, 255);

    std::shared_ptr<Screen> currentScreen;
    auto menuScreen = std::make_shared<MenuScreen>(font, menu_background_sp, menu_color);
    currentScreen = menuScreen;

    std::function<void(Event)> eventReceiver = [&](Event event) {
        switch (event.type) {
        case Event::MENU_START_NEW_GAME:
        {
            auto level = std::make_shared<Level>("1.txt", "assets/tileset.png", tilesSpriteInfo, background_sp);
            level->setEventReceiver(eventReceiver);
            currentScreen = level;
        }
        break;
        case Event::GAME_OVER:
            menu_background_tx.create(window->getSize().x, window->getSize().y);
            menu_background_tx.update(*window);
            menuScreen->enableTryAgain();
            currentScreen = menuScreen;
        }
    };

    menuScreen->setEventReceiver(eventReceiver);

    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
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
