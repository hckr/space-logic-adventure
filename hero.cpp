#include "hero.hpp"

#include "level.hpp"


Hero::Hero(Level *parent) {
    level = parent;
}

const sf::Vector2i& Hero::getPos() const {
    return pos;
}

void Hero::setPos(sf::Vector2i newPos) {
    pos = newPos;
}
