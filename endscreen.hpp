#pragma once

#include "screen.hpp"

class EndScreen : public Screen
{
    sf::Sprite &background_sp;

public:
    EndScreen(const sf::Font &font, sf::Sprite &background_sp, const sf::Color &fillColor, const sf::Color &outlineColor);
    virtual void processEvent(const sf::Event &event);

private:
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
