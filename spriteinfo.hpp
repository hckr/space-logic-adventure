#pragma once

struct TexCoords {
    sf::Vector2f top_left;
    sf::Vector2f top_right;
    sf::Vector2f bottom_right;
    sf::Vector2f bottom_left;
};

struct SpriteInfo {
    int width;
    int height;
    TexCoords texCoords;
};
