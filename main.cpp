#include <memory>

#include <SFML/Graphics.hpp>


auto createCenteredWindow(unsigned width, unsigned height) {
    auto window = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), "Space Logic Adventure");
    int posX = (sf::VideoMode::getDesktopMode().width - window->getSize().x) / 2,
        posY = (sf::VideoMode::getDesktopMode().height - window->getSize().y) / 2;
    window->setPosition(sf::Vector2i(posX, posY));
    return window;
}

int main() {
    auto window = createCenteredWindow(800, 600);
    
    sf::Texture background_tx;
    background_tx.loadFromFile("assets/background.png");
    background_tx.setRepeated(true);
    background_tx.setSmooth(true);
    sf::Sprite background_sp(background_tx);
    background_sp.setTextureRect({ 0, 0, window->getSize().x, window->getSize().y });

    sf::Texture tx1;
    tx1.loadFromFile("assets/metalTileConnectEnd_SE.png");
    tx1.setSmooth(true);
    sf::Sprite sp1(tx1);
    sp1.setPosition(sf::Vector2f(200, 200));
    sp1.setScale(0.5, 0.5);

    sf::Texture tx2;
    tx2.loadFromFile("assets/metalTileConnectStraight_NE.png");
    tx2.setSmooth(true);
    sf::Sprite sp2(tx2);
    sp2.setPosition(sf::Vector2f(234, 174));
    sp2.setScale(0.5, 0.5);

    sf::Texture tx3;
    tx3.loadFromFile("assets/metalTileConnectStraight_NE.png");
    tx3.setSmooth(true);
    sf::Sprite sp3(tx3);
    sp3.setPosition(sf::Vector2f(271, 148));
    sp3.setScale(0.5, 0.5);

    sf::Texture tx4;
    tx4.loadFromFile("assets/metalTileConnectCornerInner_SW.png");
    tx4.setSmooth(true);
    sf::Sprite sp4(tx4);
    sp4.setPosition(sf::Vector2f(308, 124));
    sp4.setScale(0.5, 0.5);

    sf::Texture tx5;
    tx5.loadFromFile("assets/alien_NE.png");
    tx5.setSmooth(true);
    sf::Sprite sp5(tx5);
    sp5.setPosition(sf::Vector2f(217, 193));
    sp5.setScale(0.7, 0.7);

    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
            }
        }
        window->draw(background_sp);
        window->draw(sp4);
        window->draw(sp3);
        window->draw(sp2);
        window->draw(sp1);
        window->draw(sp5);
        window->display();
    }

    return 0;
}
