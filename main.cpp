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

    sf::RectangleShape shape(sf::Vector2f(100, 100));
    shape.setPosition(50, 50);
    shape.setFillColor(sf::Color::Green);

    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
            }
        }
        window->clear();
        window->draw(shape);
        window->display();
    }

    return 0;
}
