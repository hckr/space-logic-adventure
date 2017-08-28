#pragma once

#include <SFML/Graphics.hpp>

class Level;


class Hero {
    sf::Vector2i pos;
    Level *level; // TODO probably won't be needed
public:
    Hero(Level *parent);
    const sf::Vector2i& getPos() const;
    void setPos(sf::Vector2i);
    enum Face {
        TOP,
        RIGHT,
        BOTTOM,
        LEFT,
        COUNT // utility
    } face = TOP;
};
