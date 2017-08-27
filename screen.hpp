#pragma once

#include <SFML/Graphics.hpp>


class Screen : public sf::Drawable {
public:
    virtual void processEvent(const sf::Event &event) = 0;
};
