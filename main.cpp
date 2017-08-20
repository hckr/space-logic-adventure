#include <memory>

#include <SFML/Graphics.hpp>
#include "tileset.hpp"
#include "level.hpp"

auto createCenteredWindow(int width, int height) {
    auto window = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), "Space Logic Adventure", sf::Style::Titlebar | sf::Style::Close);
    int posX = (sf::VideoMode::getDesktopMode().width - window->getSize().x) / 2,
        posY = (sf::VideoMode::getDesktopMode().height - window->getSize().y) / 2;
    window->setPosition(sf::Vector2i(posX, posY));
    return window;
}

void addSpriteToVertexArray(sf::VertexArray& arr, SpriteInfo spriteInfo, sf::Vector2f leftTopPos) {
    arr.append(sf::Vertex(leftTopPos, spriteInfo.texCoords.top_left));
    arr.append(sf::Vertex(sf::Vector2f(leftTopPos.x + spriteInfo.width, leftTopPos.y), spriteInfo.texCoords.top_right));
    arr.append(sf::Vertex(sf::Vector2f(leftTopPos.x + spriteInfo.width, leftTopPos.y + spriteInfo.height), spriteInfo.texCoords.bottom_right));
    arr.append(sf::Vertex(sf::Vector2f(leftTopPos.x, leftTopPos.y + spriteInfo.height), spriteInfo.texCoords.bottom_left));
}

int main() {
    auto window = createCenteredWindow(1024, 768);
    
    sf::Texture background_tx;
    background_tx.loadFromFile("assets/background.png");
    background_tx.setRepeated(true);
    background_tx.setSmooth(true);
    sf::Sprite background_sp(background_tx);
    background_sp.setTextureRect({ 0, 0, static_cast<int>(window->getSize().x), static_cast<int>(window->getSize().y) });

    sf::Texture tileset;
    tileset.loadFromFile("assets/tileset.png");
    tileset.setSmooth(true);

    sf::VertexArray vertices;
    vertices.setPrimitiveType(sf::Quads);
    addSpriteToVertexArray(vertices, Tileset::metalTileConnectCornerInner_SW, sf::Vector2f(308, 124));
    addSpriteToVertexArray(vertices, Tileset::metalTileConnectStraight_NE, sf::Vector2f(271, 148));
    addSpriteToVertexArray(vertices, Tileset::metalTileConnectStraight_NW, sf::Vector2f(345, 148));
    addSpriteToVertexArray(vertices, Tileset::metalTileConnectStraight_NE, sf::Vector2f(234, 174));
    addSpriteToVertexArray(vertices, Tileset::metalTileConnectEnd_SE, sf::Vector2f(200, 200));
    addSpriteToVertexArray(vertices, Tileset::alien_NE, sf::Vector2f(217, 193));

    Level("1.txt");

    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
            }
        }
        window->draw(background_sp);
        window->draw(vertices, sf::RenderStates(&tileset));
        window->display();
    }

    return 0;
}
